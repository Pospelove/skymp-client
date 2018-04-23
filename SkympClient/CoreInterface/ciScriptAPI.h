#pragma once
// To be included into ciScript.cpp

namespace api
{
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

	namespace ci
	{
		void write(const std::string &s)
		{
			::ci::Chat::AddMessage(StringToWstring(s));
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