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
	void ExecuteConsoleCommandImpl(std::string cmd)
	{
		if (!closeCursorMenuLock.TryLock(ExecuteConsoleCommandImpl))
		{
			ci::SetTimer(100, [=] {
				ExecuteConsoleCommandImpl(cmd);
			});
		}
		else
			sd::ExecuteConsoleCommand(const_cast<char*>(cmd.c_str()), nullptr);
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
						cd::SendAnimationEvent(g_thePlayer, "Skymp_Register");
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
		auto menuManager = MenuManager::GetSingleton();
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
		SET_TIMER(0, [=] {
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
		SET_TIMER(0, [=] {
			auto weather = sd::FindWeather(type);
			if (weather != nullptr)
				ci::SetWeather(weather->formID);
		});
	}

	void SetGlobalVariable(uint32_t globalID, float val)
	{
		SET_TIMER(0, [=] {
			auto global = (TESGlobal *)LookupFormByID(globalID);
			if (global && global->formType == FormType::Global)
			{
				sd::SetValue(global, val);
			}
		});
	}

	struct Mutex::Impl 
	{
		dlf_mutex mutex;
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
}