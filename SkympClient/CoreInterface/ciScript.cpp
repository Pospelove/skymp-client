#include "../stdafx.h"
#include "CoreInterface.h"

extern "C" {
#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
}
#include <LuaBridge\LuaBridge.h>

#include "../ImGui/imgui.h"
#include "../Overlay/ImGuiManager.h"

enum class Thread
{
	Unknown,
	DirectX,
	ClientLogic,
};


class GContextM
{
public:
	static dlf_mutex &GetMutex()
	{
		static GContextM gcm;
		return *gcm.mPtr;
	}

private:
	GContextM()
	{
		static dlf_mutex m = { "ci_script_gcontext" };
		m.set_limit(750);
		mPtr = &m;
	}

	dlf_mutex *mPtr;
};

auto &gContextM = GContextM::GetMutex();

void s()
{
	gContextM.set_limit(5000);
}

struct ci::Script::Impl
{
	lua_State *L = nullptr;
	std::string scriptName;
	bool isValid = false;
	std::string lastError = "";
	std::map<std::string,std::function<void(luabridge::LuaRef)>> on;
	Thread thr = Thread::Unknown;
};

ci::Script::Impl *gContext = nullptr;

class ImGuiHook
{
public:
	static ImGuiHook *GetSingleton()
	{
		static ImGuiHook hook;
		return &hook;
	}

	void SignalAdd(size_t scriptPtr, std::function<void()> fn)
	{
		std::lock_guard<dlf_mutex> l(m);
		this->allHooks[scriptPtr] = fn;
	}

	void SignalRemove(size_t scriptPtr)
	{
		std::lock_guard<dlf_mutex> l(m);
		this->allHooks.erase(scriptPtr);
	}

private:
	ImGuiHook()
	{
		ImGuiManager::SetGlobalRenderHook([this] {
			std::lock_guard<dlf_mutex> l(m);
			try {
				for (auto &pair : this->allHooks)
					pair.second();
			}
			catch (...) {
			}
		});
	}

	std::map<size_t, std::function<void()>> allHooks;
	dlf_mutex m{"ci_script_imguihook"};
};

ci::Script::Script(const std::string &scriptName)
{
	pImpl.reset(new Impl);
	pImpl->scriptName = scriptName;

	pImpl->L = luaL_newstate();
	auto L = pImpl->L;

	luaL_openlibs(L);
	lua_pcall(L, 0, 0, 0);

	this->Register();

	{
		
		bool dofileSuccess;
		{
			std::lock_guard<dlf_mutex> l(gContextM);
			pImpl->thr = Thread::ClientLogic;
			gContext = pImpl.get();
			const bool doStringSuccess = !luaL_dostring(L, "skymp.ci.console.__index = function(table, key)\
				return function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)\
				local args = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 }\
				local str = key\
				for i = 1, #args do\
					if args[i] ~= nil then\
						str = str .. ' ' ..tostring(args[i])\
						end\
						end\
						skymp.ci.executeCommand(0, str)\
						end\
						end");
			if (!doStringSuccess)
			{
				const std::string err = lua_tostring(L, -1);
				ci::Log("FATAL:Lua luaL_dostring %s", err.data());
				std::exit(0);
			}
			dofileSuccess = !luaL_dofile(L, pImpl->scriptName.data());
		}
		if (dofileSuccess)
		{
			pImpl->isValid = true;
		}
		else
		{
			pImpl->lastError = lua_tostring(L, -1);
		}
	}

	// Register for 'render' event
	ImGuiHook::GetSingleton()->SignalAdd((size_t)this, [this] {
		std::lock_guard<dlf_mutex> l(gContextM);
		pImpl->thr = Thread::DirectX;
		gContext = pImpl.get();
		try {
			pImpl->on.at("render")(luabridge::newTable(gContext->L));
		}
		catch (...) {
		}
	});
}

ci::Script::~Script()
{
	ImGuiHook::GetSingleton()->SignalRemove((size_t)this); // do it before lua_State destroyed

	// TODO: destroy the lua_State
}

bool ci::Script::IsValid() const
{
	return pImpl->isValid;
}

std::string ci::Script::GetLastError() const
{
	return pImpl->lastError;
}

void ci::Script::TriggerEvent(const std::string &eventName, const std::string &t)
{
	static auto thrid = GetCurrentThreadId();
	if (thrid != GetCurrentThreadId())
	{
		ErrorHandling::SendError("DBG:Lua TriggerEvent() Bad thread %d %d", thrid, GetCurrentThreadId());
		thrid = GetCurrentThreadId();
	}

	std::lock_guard<dlf_mutex> l(gContextM);
	pImpl->thr = Thread::ClientLogic;
	gContext = pImpl.get();
	try {
		const auto luaCode = "return " + t;
		const bool suc = !luaL_dostring(gContext->L, luaCode.data());
		const auto evn = suc ? luabridge::LuaRef::fromStack(gContext->L, -1) : luabridge::newTable(gContext->L);
		if (pImpl->on.count(eventName))
			pImpl->on[eventName](evn);
	}
	catch (const std::exception &e) {
		ErrorHandling::SendError("ERROR:Lua TriggerEvent('%s') %s", eventName.data(), e.what());
	}
}

// Should be included here
#include "ciScriptTypes.h"
#include "ciScriptAPI.h"

void dummy1()
{
}

void ci::Script::Register()
{
	luabridge::getGlobalNamespace(pImpl->L)
		.beginNamespace("skymp")
		.addFunction("on", api::on)

		.beginNamespace("ci").beginNamespace("console")
		.addFunction("__index", dummy1)
		.endNamespace().endNamespace()

		.beginNamespace("ci")
		// ciOther.cpp
		.addFunction("printNote", api::_ci_::printNote)
		.addFunction("setSyncOption", api::_ci_::setSyncOption)
		.addFunction("setVolume", api::_ci_::setVolume)
		.addFunction("isKeyPressed", api::_ci_::isKeyPressed)
		.addFunction("hotLoadPlugin", api::_ci_::hotLoadPlugin)
		.addFunction("debugMoveFarFarAway", api::_ci_::debugMoveFarFarAway)
		.addFunction("executeCommand", api::_ci_::executeCommand)
		.addFunction("registerAnimation", api::_ci_::registerAnimation)
		.addFunction("isCombatAnim", api::_ci_::isCombatAnim)
		.addFunction("showRaceMenu", api::_ci_::showRaceMenu)
		.addFunction("showMainMenu", api::_ci_::showMainMenu)
		.addFunction("setTimer", api::_ci_::setTimer)
		.addFunction("isLoadScreenOpen", api::_ci_::isLoadScreenOpen)
		.addFunction("isInPause", api::_ci_::isInPause)
		.addFunction("isWorldspace", api::_ci_::isWorldspace)
		.addFunction("isCell", api::_ci_::isCell)
		.addFunction("setWeather", api::_ci_::setWeather)
		.addFunction("setWeatherType", api::_ci_::setWeatherType)
		.addFunction("setGlobalVariable", api::_ci_::setGlobalVariable)
		.addFunction("log", api::_ci_::log)
		.addFunction("traceCDCalls", api::_ci_::traceCDCalls)
		.addFunction("removeAllKeywords", api::_ci_::removeAllKeywords)
		.addFunction("addKeyword", api::_ci_::addKeyword)
		.addFunction("showSkympLogo", api::_ci_::showSkympLogo)
		.addFunction("hideSkympLogo", api::_ci_::hideSkympLogo)
		.addFunction("setControlEnabled", api::_ci_::setControlEnabled)
		.endNamespace()

		.beginNamespace("ci").beginClass<api::_ci_::Text3D>("Text3D")
		.addConstructor<void (*)(std::string, float, float, float)>()
		.addFunction("destroy", &api::_ci_::Text3D::destroy)
		.addFunction("setText", &api::_ci_::Text3D::setText)
		.addFunction("setPos", &api::_ci_::Text3D::setPos)
		.addFunction("setFontHeight", &api::_ci_::Text3D::setFontHeight)
		.addFunction("getText", &api::_ci_::Text3D::getText)
		.addFunction("getX", &api::_ci_::Text3D::getX)
		.addFunction("getY", &api::_ci_::Text3D::getY)
		.addFunction("getZ", &api::_ci_::Text3D::getZ)
		.addFunction("setVisible", &api::_ci_::Text3D::setVisible)
		.addFunction("setDrawDistance", &api::_ci_::Text3D::setDrawDistance)
		.endClass().endNamespace()

		.beginNamespace("ci").beginClass<api::_ci_::Actor>("Actor")
		.addConstructor<void(*)(std::string, float, float, float)>()
		.addStaticFunction("getLocalPlayer", &api::_ci_::Actor::getLocalPlayer)
		.addFunction("destroy", &api::_ci_::Actor::destroy)
		.addFunction("getMovement", &api::_ci_::Actor::getMovement)
		.addFunction("applyMovement", &api::_ci_::Actor::applyMovement)
		.addFunction("getLook", &api::_ci_::Actor::getLook)
		.addFunction("applyLook", &api::_ci_::Actor::applyLook)
		.endClass().endNamespace()

		.beginNamespace("ci").beginNamespace("config")
		.addFunction("__index", api::_ci_::config::__index)
		.endNamespace().endNamespace()

		.beginNamespace("ci").beginNamespace("chat")
		.addFunction("addMessage", api::_ci_::chat::addMessage)
		.addFunction("clear", api::_ci_::chat::clear)
		.endNamespace().endNamespace()

		.beginNamespace("gui")
		.addFunction("window", api::gui::window)
		.addFunction("endWindow", api::gui::endWindow)
		.addFunction("button", api::gui::button)
		.endNamespace()


		.endNamespace();
}