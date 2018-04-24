#pragma once
// To be included into ciScript.cpp

// Is it the part of ClientLogic?? It requires g_config for example
// However config will be placed to namespace ci
#include "../ClientLogic/Config.h"

namespace api
{
	void on(const std::string &eventName, luabridge::LuaRef fn)
	{
		if (gContext == nullptr)
			return;

		if (fn.isFunction())
		{
			auto cppFn = [eventName, fn] {
				try {
					fn();
				}
				catch (std::exception &e) {
					ErrorHandling::SendError("FATAL:Lua skymp.on('%s', ...) %s", eventName.data(), e.what());
					std::exit(0);
				}
			};

			if (gContext->on[eventName] == nullptr)
			{
				gContext->on[eventName] = cppFn;
			}
			else
			{
				auto prevHandlers = gContext->on[eventName];
				gContext->on[eventName] = [prevHandlers, cppFn] {
					prevHandlers();
					cppFn();
				};
			}
		}
		else
		{
			gContext->on.erase(eventName);
		}
	}

	namespace _ci_
	{
#define ASSERT_THREAD(thr) {if (gContext == nullptr) {ErrorHandling::SendError("FATAL:Lua gContext was nullptr"); std::exit(0);} if (gContext->thr != thr) {ErrorHandling::SendError("FATAL:Lua ci member called from non-ClientLogic thread"); std::exit(0);} }
//#define CIDEF(type, luaFuncDeclaration, lambdaToReturn) type luaFuncDeclaration ASSERT_THREAD(Thread::ClientLogic, type())

		void printNote(const std::string &s) {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::Chat::AddMessage(StringToWstring(s), true);
		}

		void setSyncOption(const std::string &option, const std::string &val) {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::SetSyncOption(option, val);
		}

		void setVolume(uint32_t volume) {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::SetVolume(volume);
		}

		bool isKeyPressed(int32_t vkcode) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsKeyPressed(vkcode);
		}

		void hotLoadPlugin(const std::string &binary) {
			ASSERT_THREAD(Thread::ClientLogic);
			const std::vector<char> b = { binary.begin(), binary.end() };
			ci::HotLoadPlugin(b);
		}

		void debugMoveFarFarAway() {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::DebugMoveFarFarAway();
		}

		void executeCommand(int32_t target, const std::string &cmd) {
			ASSERT_THREAD(Thread::ClientLogic);
			if (target == 0)
				ci::ExecuteCommand(ci::CommandType::Console, cmd);
			if (target == 1)
				ci::ExecuteCommand(ci::CommandType::CDScript, cmd);
		}

		void registerAnimation(const std::string &animName, uint32_t newID) {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::RegisterAnimation(animName, newID);
		}

		bool isCombatAnim(uint32_t animID) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsCombatAnimID(animID);
		}

		void showRaceMenu() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::ShowRaceMenu();
		}

		void showMainMenu() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::ShowMainMenu();
		}

		void setTimer(uint32_t ms, luabridge::LuaRef fn) {
			ASSERT_THREAD(Thread::ClientLogic);
			ci::SetTimer(ms, [=] {
				try {
					fn();
				}
				catch (const std::exception &e) {
					ErrorHandling::SendError("ERROR:Lua SetTimer() %s", e.what());
				}
			});

		}

		bool isLoadScreenOpen() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsLoadScreenOpen();
		}

		bool isInPause() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsInPause();
		}

		bool isWorldspace(uint32_t id) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsWorldSpace(id);
		}

		bool isCell(uint32_t id) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::IsCell(id);
		}

		void setWeather(uint32_t wid) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::SetWeather(wid);
		}

		void setWeatherType(uint8_t type) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::SetWeatherType(type);
		}

		void setGlobalVariable(uint32_t globalID, float val) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::SetGlobalVariable(globalID, val);
		}

		void log(std::string text) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::Log(text);
		}

		void traceCDCalls(bool set) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::TraceCDCalls(set);
		}

		void removeAllKeywords(uint32_t baseID) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::RemoveAllKeywords(baseID);
		}

		void addKeyword(uint32_t baseID, std::string str) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::AddKeyword(baseID, str);
		}

		void showSkympLogo() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::ShowSkympLogo();
		}

		void hideSkympLogo() {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::HideSkympLogo();
		}

		void setControlEnabled(const std::string &controlName, bool set) {
			ASSERT_THREAD(Thread::ClientLogic);
			return ci::SetControlEnabled(controlName, set);
		}

		class Text3D
		{
		public:
			Text3D(std::string str, float x, float y, float z) {
				ASSERT_THREAD(Thread::ClientLogic);
				text = new ci::Text3D(StringToWstring(str), { x,y,z });
			}

			void destroy() {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
				{
					delete text;
					text = nullptr;
				}
			}

			void setText(std::string str) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
					text->SetText(StringToWstring(str));
			}

			void setPos(float x, float y, float z) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
					text->SetPos({ x,y,z });
			}

			void setFontHeight(uint32_t height) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
					text->SetFontHeight(height);
			}

			std::string getText() const {
				ASSERT_THREAD(Thread::ClientLogic);
				return text ? WstringToString(text->GetText()) : "";
			}

			float getX() const {
				return getPos().x;
			}

			float getY() const {
				return getPos().y;
			}

			float getZ() const {
				return getPos().z;
			}

			void setVisible(bool v) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
					text->SetVisible(v);
			}

			bool isVisible() const {
				ASSERT_THREAD(Thread::ClientLogic);
				return text && text->IsVisible();
			}

			void setDrawDistance(float distance) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (text)
					text->SetDrawDistance(distance);
			}

			float getDrawDistance() const {
				ASSERT_THREAD(Thread::ClientLogic);
				return text ? text->GetDrawDistance() : 0.0f;
			}

		private:
			NiPoint3 getPos() const {
				ASSERT_THREAD(Thread::ClientLogic);
				return text ? text->GetPos() : NiPoint3(0, 0, 0);
			}

			ci::Text3D *text;
		};

		namespace config
		{

			std::string __index(luabridge::LuaRef table, const std::string &key) {
				ASSERT_THREAD(Thread::ClientLogic);
				try {
					return g_config.at(key);
				}
				catch (...) {
					return "";
				}
			}
		}

		namespace chat
		{
			void addMessage(const std::string &s) {
				ASSERT_THREAD(Thread::ClientLogic);
				if (s == " ") // prevent chat cleaning
					ci::Chat::AddMessage(StringToWstring(s + s), false);
				else
					ci::Chat::AddMessage(StringToWstring(s), false);
			}

			void clear() {
				ASSERT_THREAD(Thread::ClientLogic);
				ci::Chat::AddMessage(L" ");
			}
		}
	}

	namespace gui
	{

		bool window(std::string name, ImGuiWindowFlags flags) {
			ASSERT_THREAD(Thread::DirectX);
			bool p_open = true;
			ImGui::Begin(name.data(), &p_open, flags);
			return p_open;
		};

		void endWindow() {
			ASSERT_THREAD(Thread::DirectX);
			ImGui::End();
		};

		bool button(std::string text) {
			ASSERT_THREAD(Thread::DirectX);
			return ImGui::Button(text.data());
		};
	}
}