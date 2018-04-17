#include "../stdafx.h"
#include "../Sync/LookData.h"
#include "ciGUI.h"
#include "ciOther.h"
#include "ciItemType.h"
#include "ciMain.h"
#include <stdlib.h>
#include <mutex>
#include "../ScriptDragon/enums.h"
#include "../Costile/CDScript.h"

#include "../Sync/AnimData.h"
#include "../Sync/SyncOptions.h"

#include "../Overlay\Loadscreen.h"

#include "Skyrim\FileIO\TESDataHandler.h"

extern LockTimer closeCursorMenuLock;

class CIAccess
{
public:
	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};

namespace ci
{

	void SetSyncOption(std::string o, std::string val)
	{
		// threadsafe
		static auto so = SyncOptions::GetSingleton();
		so->Set(o, val);
	}

	void SetVolume(unsigned long volume)
	{
		auto setVolume = [](DWORD v) {
			WAVEFORMATEX    wfx = { 0 };

			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.cbSize = 0;
			wfx.nChannels = 1;
			wfx.nSamplesPerSec = 1000;
			wfx.wBitsPerSample = 16;
			wfx.nBlockAlign = wfx.wBitsPerSample / 8;
			wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

			auto hEvent = CreateEvent(0, TRUE, FALSE, 0);
			if (hEvent)
			{
				HWAVEOUT out;
				waveOutOpen(&out,
					WAVE_MAPPER,
					&wfx,
					(DWORD)0,
					(DWORD)GetModuleHandle(NULL),
					CALLBACK_NULL);
				waveOutSetVolume(out, v);
			}
		};
		return setVolume(volume);
	}

	std::wstring GetDateString()
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		std::stringstream ss;
		ss << std::to_string(st.wMinute) + "-"
			+ std::to_string(st.wHour) + "."
			+ std::to_string(st.wDay) + "."
			+ std::to_string(st.wMonth) + "."
			+ std::to_string(st.wYear);
		ss.flush();
		return StringToWstring(ss.str());
	}

	void ExecuteConsoleCommandImpl(std::string cmd)
	{
		if (!closeCursorMenuLock.TryLock(ExecuteConsoleCommandImpl))
		{
			ci::SetTimer(100, [=] {
				ExecuteConsoleCommandImpl(cmd);
			});
		}
		else
			sd::ExecuteConsoleCommand(const_cast<char*>(cmd.c_str()), sd::GetConsoleSelectedRef());
	}

	void ExecuteConsoleCommand(const std::wstring &consoleCmdString)
	{
		if (consoleCmdString.empty())
			return;

		if (consoleCmdString == L"skymp qnnu")
		{
			SET_TIMER(0, [] {
				g_thePlayer->QueueNiNodeUpdate(false);
			});
			return;
		}

		static std::mutex consoleMutex;
		std::lock_guard<std::mutex> l(consoleMutex);

		ExecuteConsoleCommandImpl(WstringToString(consoleCmdString));
	}

	bool IsKeyPressed(uint32_t vkcode)
	{
		// doesn't not require thread sync
		return sd::GetKeyPressed((BYTE)vkcode, false);
	}

	void HotLoadPlugin(const char *name)
	{
		static std::mutex m;
		std::lock_guard<std::mutex> l(m);
		TESDataHandler::GetSingleton()->HotLoadPlugin(name);
		ci::Log("DBG:Other loaded %s.esp", name);
	}

	void HotLoadPlugin(const std::vector<char> &pluginBinary)
	{	
		static bool sranded = false;
		if (!sranded)
		{
			sranded = true;
			srand(108);
		}
		std::string randstr = "_skymp" + std::to_string(rand());
		std::string file = (std::string)"Data/" + randstr + ".esp";
		std::ofstream of(file);
		for (auto ch : pluginBinary)
			of << ch;
		of.close();

		ci::HotLoadPlugin((new std::string(randstr))->data());
	}
	
	void DebugMoveFarFarAway()
	{
		SET_TIMER_LIGHT(1, [] {
			auto refr = WorldCleaner::GetSingleton()->FindFarObject();
			if (refr != nullptr)
			{
				const auto pos = cd::GetPosition(refr);
				ci::LocalPlayer::GetSingleton()->SetPos(pos);
			}
		});
	}

	void ExecuteCommand(CommandType cmdType, const std::wstring &cmdString)
	{
		switch (cmdType)
		{
		case CommandType::Console:
			return ExecuteConsoleCommand(cmdString);
		case CommandType::CDScript:
			return cd::Script::Context().Eval(WstringToString(cmdString));
		}
	}

	void ExecuteCommand(CommandType cmdType, const std::string &cmdString)
	{
		return ExecuteCommand(cmdType, StringToWstring(cmdString));
	}

	void RegisterAnimation(std::string animationName, uint32_t animationID)
	{
		SET_TIMER_LIGHT(1, [=] {
			AnimData_::RegisterAnimation(animationName, animationID);
		});
	}

	bool IsCombatAnimID(uint32_t animationID)
	{
		return animationID <= 21;
	}

	void SetTimer(uint32_t ms, std::function<void()> fn)
	{
		if (ms == 0)
		{
			std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
			return fn();
		}

		std::thread([=] {
			Sleep(ms);
			std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
			fn();
		}).detach();
	}

	clock_t greyFaceFixMoment = ~0;

	void ShowRaceMenu()
	{
		SET_TIMER(0, [] {
			SET_TIMER(300, [] {
				ILookSynchronizer::GetV17()->ShowRaceMenu();
				SET_TIMER(0, [] {
					sd::ForceFirstPerson();
					SET_TIMER(300, [] {
						AnimData_::Register();
					});
				});
			});
		});
	}

	void ShowMainMenu()
	{
		static const BSFixedString fsMainMenu = "Main Menu";
		if (MenuManager::GetSingleton()->IsMenuOpen(fsMainMenu))
			return;
		SET_TIMER(0, [] {
			MenuManager::GetSingleton()->OpenMenu(fsMainMenu);
		});
	}


	UInt32 updateRateMS = 0;
	void SetUpdateRate(uint32_t ms)
	{
		updateRateMS = ms;
	}

	bool IsLoadScreenOpen()
	{
		static auto menuManager = MenuManager::GetSingleton();
		static BSFixedString *fsLoadingMenuPtr = new BSFixedString("Loading Menu");
		return menuManager->IsMenuOpen(*fsLoadingMenuPtr);
	}

	bool IsInPause()
	{
		return *((unsigned long *)0x1B3E428) != 0;
	}

	bool IsInDebug()
	{
		return false;
	}

	bool IsWorldSpace(uint32_t formID)
	{
		auto form = LookupFormByID(formID);
		return form && form->formType == FormType::WorldSpace;
	}

	bool IsCell(uint32_t formID)
	{
		auto form = LookupFormByID(formID);
		return form && form->formType == FormType::Cell;
	}

	void SetWeather(uint32_t weatherID)
	{
		struct TWeather
		{
			int id;
			char *name;
		};

		static TWeather weather_ids[] = {
			{ ID_TESWeather::DefaultWeather, "DefaultWeather" },
			{ ID_TESWeather::DefaultWeather, "DefaultWeather" },
			{ ID_TESWeather::DefaultWeather, "DefaultWeather" },
			{ ID_TESWeather::DefaultWeather, "DefaultWeather" },
			{ ID_TESWeather::DefaultWeather, "DefaultWeather" },
			{ ID_TESWeather::SovngardeDark, "SovngardeDark" },
			{ ID_TESWeather::RiftenOvercastFog, "RiftenOvercastFog" },
			{ ID_TESWeather::EditorCloudPreview, "EditorCloudPreview" },
			{ ID_TESWeather::SkyrimClear_A, "SkyrimClear_A" },
			{ ID_TESWeather::SkyrimCloudy_A, "SkyrimCloudy_A" },
			/*ID_TESWeather::SkyrimClearSN_A,
			ID_TESWeather::SkyrimCloudySN_A,
			ID_TESWeather::SkyrimClearTU_A,
			ID_TESWeather::SkyrimCloudyTU_A,
			ID_TESWeather::SkyrimClearFF_A,
			ID_TESWeather::SkyrimCloudyFF_A,
			ID_TESWeather::SkyrimClearRE_A,
			ID_TESWeather::SkyrimCloudyRE_A,
			ID_TESWeather::SkyrimClearCO_A,
			ID_TESWeather::SkyrimCloudyCO_A,
			ID_TESWeather::SkyrimClearMA_A,
			ID_TESWeather::SkyrimCloudyMA_A,
			ID_TESWeather::SkyrimClearVT_A,
			ID_TESWeather::SkyrimCloudyVT_A,
			ID_TESWeather::FXWthrInvertWindowsWinterhold,*/ // 20
			{ ID_TESWeather::SovngardeClear, "SovngardeClear" },
			{ ID_TESWeather::FXSkyrimStormBlowingGrass, "FXSkyrimStormBlowingGrass" },
			{ ID_TESWeather::SkyrimCloudyVT, "SkyrimCloudyVT" },
			{ ID_TESWeather::SkyrimFogVT, "SkyrimFogVT" },
			{ ID_TESWeather::SkyrimOvercastRainVT, "SkyrimOvercastRainVT" }, // 25
																			 /*ID_TESWeather::SkyrimClearVT,
																			 ID_TESWeather::SkyrimCloudySN,
																			 ID_TESWeather::SkyrimClearSN,*/   // 28
			{ ID_TESWeather::SkyrimCloudyTU, "SkyrimCloudyTU" },
			{ ID_TESWeather::SkyrimOvercastRainTU, "SkyrimOvercastRainTU" }, // 30
			{ ID_TESWeather::SkyrimStormRainTU, "SkyrimStormRainTU" },
			{ ID_TESWeather::SkyrimClearTU, "SkyrimClearTU" },
			//ID_TESWeather::SkyrimCloudyFF,
			{ ID_TESWeather::SkyrimFogFF, "SkyrimFogFF" },
			/*ID_TESWeather::SkyrimOvercastRainFF,
			ID_TESWeather::SkyrimStormRainFF,*/
			{ ID_TESWeather::SkyrimClearFF, "SkyrimClearFF" },
			/*ID_TESWeather::SkyrimCloudyRE,
			ID_TESWeather::SkyrimFogRE,
			ID_TESWeather::SkyrimOvercastRainRE, // 40
			ID_TESWeather::SkyrimClearRE,*/
			/*ID_TESWeather::SkyrimCloudyCO,
			ID_TESWeather::SkyrimFogCO,
			ID_TESWeather::SkyrimClearCO,
			ID_TESWeather::SkyrimCloudyMA,
			ID_TESWeather::SkyrimFogMA,
			ID_TESWeather::SkyrimOvercastRainMA,
			ID_TESWeather::SkyrimClearMA,*/
			{ ID_TESWeather::KarthspireRedoubtFog, "KarthspireRedoubtFog" },
			{ ID_TESWeather::SkyrimDA02Weather, "SkyrimDA02Weather" }, // 50
			{ ID_TESWeather::SolitudeBluePalaceFog, "SolitudeBluePalaceFog" },
			{ ID_TESWeather::SolitudeBluePalaceFogNMARE, "SolitudeBluePalaceFogNMARE" },
			{ ID_TESWeather::SolitudeBluePalaceFogFEAR, "SolitudeBluePalaceFogFEAR" },
			{ ID_TESWeather::SolitudeBluePalaceFogARENA, "SolitudeBluePalaceFogARENA" },
			{ ID_TESWeather::BloatedMansGrottoFog, "BloatedMansGrottoFog" },// 55
			{ ID_TESWeather::SkuldafnCloudy, "SkuldafnCloudy" },
			{ ID_TESWeather::SkyrimMQ206weather, "SkyrimMQ206weather" },
			//ID_TESWeather::FXWthrInvertLightMarkarth,
			//ID_TESWeather::FXWthrInvertWindowsWindhelm2,
			//ID_TESWeather::HelgenAttackWeather, // 60
			//ID_TESWeather::FXWthrInvertLightsSolitude, 
			//ID_TESWeather::FXWthrInvertLightsWhiterun,
			//ID_TESWeather::FXWthrInvertWindowsWhiterun,
			{ ID_TESWeather::FXMagicStormRain, "FXMagicStormRain" },
			{ ID_TESWeather::SkyrimOvercastWar, "SkyrimOvercastWar" },
			{ ID_TESWeather::SkyrimStormSnow, "SkyrimStormSnow" },
			{ ID_TESWeather::SkyrimStormRain, "SkyrimStormRain" },
			{ ID_TESWeather::SkyrimOvercastRain, "SkyrimOvercastRain" },
			{ ID_TESWeather::SkyrimFog, "SkyrimFog" },
			//ID_TESWeather::FXWthrSunlightWhite,
			{ ID_TESWeather::FXWthrSunlight, "FXWthrSunlight" },
			{ ID_TESWeather::BlackreachWeather, "BlackreachWeather" },
			//ID_TESWeather::FXWthrInvertWindowsWindhelm,
			//ID_TESWeather::WorldMapWeather,
			{ ID_TESWeather::SovngardeFog, "SovngardeFog" },
			//ID_TESWeather::FXWthrInvertDayNighWarm,
			//ID_TESWeather::FXWthrCaveBluePaleLight,
			{ ID_TESWeather::FXWthrCaveBlueSkylight, "FXWthrCaveBlueSkylight" },
			//ID_TESWeather::FXWthrInvertDayNight,
			{ ID_TESWeather::SkyrimOvercastSnow, "SkyrimOvercastSnow" },
			//ID_TESWeather::TESTCloudyRain,
			//ID_TESWeather::SkyrimCloudy,
			{ ID_TESWeather::SkyrimClear, "SkyrimClear" }
		};
		SET_TIMER_LIGHT(10, [=] {
			int weather_size = sizeof(weather_ids) / sizeof(weather_ids[0]);
			if (weatherID < weather_size)
			{
				TESForm *weather = sd::Game::GetFormById(weather_ids[weatherID].id);
				if (weather)
				{
					sd::Weather::ForceActive((TESWeather *)weather, TRUE);
				}
			}
		});
	}

	void SetWeatherType(uint8_t type)
	{
		SET_TIMER_LIGHT(10, [=] {
			auto weather = sd::FindWeather(type);
			if (weather != nullptr)
				ci::SetWeather(weather->formID);
		});
	}

	void SetGlobalVariable(uint32_t globalID, float val)
	{
		SET_TIMER_LIGHT(10, [=] {
			auto global = (TESGlobal *)LookupFormByID(globalID);
			if (global && global->formType == FormType::Global)
			{
				sd::SetValue(global, val);
			}
		});
	}

	struct Mutex::Impl 
	{
		dlf_mutex mutex{"ci_other_mutex"};
	};

	Mutex::Mutex() : pimpl(new Impl) 
	{
	}

	Mutex::~Mutex()
	{
		delete pimpl;
	}

	void Mutex::lock() {
		return pimpl->mutex.lock();
	}

	void Mutex::unlock() {
		return pimpl->mutex.unlock();
	}

	bool Mutex::try_lock() {
		return pimpl->mutex.try_lock();
	}

	void Log(const std::wstring &str) {
		Log(WstringToString(str));
	}

	void Log(const std::string &str) {
		ErrorHandling::SendError(str.data());
	}

	void Log(const char *fmt, ...) {
		va_list arg;
		char buffer[4096];
		va_start(arg, fmt);
		sprintf_s(buffer, fmt, arg);
		va_end(arg);
		Log(std::string(buffer));
	}

	void TraceCDCalls(bool trace) {
		_putenv_s("IsTraceCDCalls", trace ? "1" : "0");
	}

	void RemoveAllKeywords(uint32_t baseID)
	{
		if (baseID == 0x6e9c2)
			return;

		const auto form = LookupFormByID(baseID);
		const auto kForm = DYNAMIC_CAST<BGSKeywordForm *, TESForm *>(form);
		if (kForm != nullptr)
			Utility::RemoveAllKeywords(kForm);
		else
			ci::Log("ERROR:Other bad keyword form");
	}

	void AddKeyword(uint32_t baseID, std::string keywordString)
	{
		if (baseID == 0x6e9c2)
			return;

		const auto form = LookupFormByID(baseID);
		const auto kForm = DYNAMIC_CAST<BGSKeywordForm *, TESForm *>(form);
		if (kForm != nullptr)
			Utility::AddKeyword(kForm, keywordString);
		else
			ci::Log("ERROR:Other bad keyword form");
	}

	static std::shared_ptr<Loadscreen> ls;

	void ShowSkympLogo()
	{
		ls.reset(new Loadscreen("MyGUI\\logo.png"));
	}

	void HideSkympLogo()
	{
		ls.reset();
	}

	void SetControlEnabled(std::string controlName, bool enable)
	{
		Control control = Control::COUNT;
		if (controlName == "Movement")
			control = Control::Movement;
		if (controlName == "Fighting")
			control = Control::Fighting;
		if (controlName == "CamSwitch");
			control = Control::CamSwitch;
		if (controlName == "Looking")
			control = Control::Looking;
		if (controlName == "Sneaking")
			control = Control::Sneaking;
		if (controlName == "Menu")
			control = Control::Menu;
		if (controlName == "Activate")
			control = Control::Activate;
		if (controlName == "JournalTabs")
			control = Control::JournalTabs;
		if (controlName == "SaveGame")
			control = Control::SaveGame;
		if (controlName == "Wait")
			control = Control::Wait;
		if (controlName == "FastTravel")
			control = Control::FastTravel;
		if (controlName == "Console")
			control = Control::Console;
		if (controlName == "BeastForm")
			control = Control::BeastForm;

		if (control != Control::COUNT)
			PlayerControls_::SetEnabled(control, enable);
	}
}