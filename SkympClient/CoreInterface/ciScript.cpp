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

struct ci::Script::Impl
{
	lua_State *L = nullptr;
	std::string scriptName;
	bool isValid = false;
	std::string lastError = "";
	std::map<std::string,std::function<void()>> on;
	Thread thr = Thread::Unknown;
	dlf_mutex m{ "ci_script_impl" };
};

ci::Script::Impl *gContext = nullptr;

inline Thread GetContextThread()
{
	std::lock_guard<dlf_mutex> l(gContext->m);
	return gContext ? gContext->thr : Thread::Unknown;
}

class ContextGuard
{
public:
	ContextGuard(std::unique_ptr<ci::Script::Impl> &ptr, Thread thr)
	{
		ptr->m.lock();
		gContext = ptr.get();
		gContext->thr = thr;
	}

	~ContextGuard()
	{
		gContext->thr = Thread::Unknown;
		gContext->m.unlock();
		gContext = nullptr;
	}
};

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
		ContextGuard cg(pImpl, Thread::ClientLogic);
		pImpl->thr = Thread::ClientLogic;
		const auto dofileSuccess = !luaL_dofile(L, pImpl->scriptName.data());
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
		ContextGuard cg(pImpl, Thread::DirectX);
		try {
			pImpl->on.at("render")();
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

namespace api
{
	void write(const std::string &s)
	{
		ci::Chat::AddMessage(StringToWstring(s));
	}

	void on(const std::string &eventName, luabridge::LuaRef fn)
	{
		if (gContext == nullptr)
			return;

		if (fn.isFunction())
		{
			gContext->on[eventName] = [eventName, fn] { 
				try {
					fn();
				}
				catch (std::exception &e) {
					ErrorHandling::SendError("FATAL:Lua skymp.on('%s', ...) %s", eventName.data(), e.what());
					std::exit(0);
				}
			};
		}
		else
		{
			gContext->on.erase(eventName);
		}
	}

	namespace gui
	{
#define IMGUIDEF(type, luaFuncDeclaration, lambdaToReturn) type luaFuncDeclaration {if (gContext == nullptr) return type(); if (GetContextThread() == Thread::DirectX) return lambdaToReturn(); ErrorHandling::SendError("FATAL:Lua gui member called from non-DirectX thread"); std::exit(0); }

		IMGUIDEF(bool, begin(std::string name, ImGuiWindowFlags flags), [&] {
			bool p_open = true;
			ImGui::Begin(name.data(), &p_open, flags);
			return p_open;
		});

		IMGUIDEF(void, endWindow(), [&] {
			ImGui::End();
		});

		IMGUIDEF(bool, button(std::string text), [&] {
			return ImGui::Button(text.data());
		});
	}
}

void ci::Script::Register()
{
	luabridge::getGlobalNamespace(pImpl->L)
		.beginNamespace("skymp")
		.addFunction("write", api::write)
		.addFunction("on", api::on)

		.beginNamespace("gui")
		.addFunction("begin", api::gui::begin)
		.addFunction("endWindow", api::gui::endWindow)
		.addFunction("button", api::gui::button)
		.endNamespace()


		.endNamespace();
}