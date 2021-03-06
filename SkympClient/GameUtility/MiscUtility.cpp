#include "../stdafx.h"
#include "MiscUtility.h"

#include <SKSE/NiNodes.h>
#include <SKSE/NiObjects.h>
#include <SKSE/GameData.h>

namespace Utility
{
	uint32_t NewFormID()
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> l(mutex);
		static uint32_t id = ~0;
		do
		{
			id--;
		} while (LookupFormByID(id));
		return id;
	}

	std::vector<FormID> GetLoadedRefs(FormID worldSpaceID)
	{
		static std::map<TESWorldSpace *, std::vector<FormID>> data;

		static BSFixedString fsMainMenu = "Main Menu";
		if (MenuManager::GetSingleton()->IsMenuOpen(fsMainMenu))
			data.clear();

		if (data.empty())
		{
			for (uint32_t id = 0; id <= 0x00104217; ++id)
			{
				auto ws = (TESWorldSpace *)LookupFormByID(id);
				if (ws && ws->formType == FormType::WorldSpace)
					data.insert({ ws, {} });

			}
			for (uint32_t id = 0; id <= 0x00104217; ++id)
			{
				auto ref = (TESObjectREFR *)LookupFormByID(id);
				if (ref && ref->formType == FormType::Reference)
				{
					void *refWp = ref->GetWorldSpace();
					for (auto it = data.begin(); it != data.end(); ++it)
						if (refWp == it->first)
						{
							it->second.push_back(ref->formID);
							std::unique(it->second.begin(), it->second.end());
						}
				}
			}
		}

		return data[(TESWorldSpace *)LookupFormByID(worldSpaceID)];
	}

	bool IsForegroundProcess()
	{
		static auto isForegroundProcess = [](DWORD pid)
		{
			HWND hwnd = GetForegroundWindow();
			if (hwnd == NULL)
				return false;
			DWORD foregroundPid;
			if (GetWindowThreadProcessId(hwnd, &foregroundPid) == 0)
				return false;
			return (foregroundPid == pid);
		};
		return isForegroundProcess(GetCurrentProcessId());
	}

	namespace Private
	{
		struct HandleData {
			unsigned long process_id;
			HWND best_handle;
		};

		BOOL IsMainWindow(HWND handle) {
			return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
		}

		BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
			auto &data = *(HandleData*)lParam;
			unsigned long process_id = 0;
			GetWindowThreadProcessId(handle, &process_id);
			if (data.process_id != process_id || !IsMainWindow(handle)) {
				return TRUE;
			}
			data.best_handle = handle;
			return FALSE;
		}
	}

	HWND FindMainWindow() {
		using namespace Private;
		HandleData data;
		data.process_id = GetCurrentProcessId();
		data.best_handle = NULL;
		EnumWindows(EnumWindowsCallback, (LPARAM)&data);
		return data.best_handle;
	}

	NiAVObject * ResolveNode(TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		if (!obj) return NULL;

		NiAVObject	* result = (NiAVObject	*)obj->GetNiNode();

		// special-case for the player, switch between first/third-person
		PlayerCharacter * player = obj == g_thePlayer ? g_thePlayer : nullptr;
		if (player && player->loadedState)
			result = firstPerson ? player->firstPersonSkeleton : player->loadedState->node;

		// name lookup
		if (obj && nodeName.c_str()[0] && result)
			result = result->GetObjectByName(nodeName);

		return result;
	}

	NiPoint3 GetNodeWorldPosition(TESObjectREFR *obj, BSFixedString nodeName, bool firstPerson)
	{
		NiAVObject	*object = ResolveNode(obj, nodeName, firstPerson);
		return object ? object->m_worldTransform.pos : NiPoint3(0, 0, 0);
	}

	void MuteItemsSound()
	{
		DWORD oldProtect = 0;
		if (VirtualProtect((void *)0x00B9DE83, 16, PAGE_EXECUTE_READWRITE, &oldProtect))
		{
			static auto ptr = ((uint8_t *)0x00B9DE83);
			for (int i = 0; i != 5; ++i)
				ptr[i] = 0x90;
			VirtualProtect((void *)0x00B9DE83, 16, oldProtect, &oldProtect);
		}
	}

	TESObjectSTAT *GetCastingArt(EffectSetting *effect)
	{
		static auto base = LookupFormByID(ID_TESObjectSTAT::Fireball01HandEffects0000);

		static std::map<EffectSetting *, TESObjectSTAT *> statics;

		if (statics[effect] == nullptr)
		{
			statics[effect] = FormHeap_Allocate<TESObjectSTAT>();
			memcpy(statics[effect], base, sizeof TESObjectSTAT);
			statics[effect]->formID = 0;
			statics[effect]->SetFormID(Utility::NewFormID(), true);

			auto castingArt = effect->properties.castingArt;

			auto asModel = dynamic_cast<TESModelTextureSwap *>(statics[effect]);
			auto baseAsModel = dynamic_cast<TESModelTextureSwap *>(castingArt);
			memcpy(asModel, baseAsModel, sizeof *(asModel));

		}

		return statics[effect];
	}

	std::set<BGSKeywordForm *> clear;
	int32_t maxKeywords = 256;

	typedef std::map <std::string, BGSKeyword *> KeywordCache;
	static dlf_mutex s_keywordCacheLock{ "miscutility_keywordcache" }, kdLock{ "miscutility_kd"};
	static KeywordCache s_keywordCache;

	BGSKeyword *GetKeyword(void *staticFuncTag, BSFixedString keyword)
	{
		std::lock_guard<dlf_mutex> l(s_keywordCacheLock);

		if (s_keywordCache.empty()) {
			DataHandler* pDataHandler = DataHandler::GetSingleton();
			BSTArray<BGSKeyword*>& keywords = pDataHandler->keywords;
			for (UInt32 n = 0; n < keywords.GetSize(); n++) {
				BGSKeyword* pKeyword = NULL;
				keywords.GetNthItem(n, pKeyword);
				if (pKeyword) {
					s_keywordCache.insert(KeywordCache::value_type(BSFixedString(pKeyword->keyword.Get()).c_str(), pKeyword));
				}
			}
		}


		KeywordCache::iterator it = s_keywordCache.find(std::string(keyword.c_str()));
		BGSKeyword* pKeyword = (it != s_keywordCache.end()) ? it->second : NULL;
		return pKeyword;
	}

	BGSKeyword *FindKeyword(std::string keywordText)
	{
		return GetKeyword(nullptr, keywordText.data());
	}

	void RemoveAllKeywords(BGSKeywordForm *form)
	{
		std::lock_guard<dlf_mutex> l(kdLock);

		form->keywords = nullptr;
		AddKeyword(form, "");
	}

	void AddKeyword(BGSKeywordForm *form, std::string keywordStr)
	{
		std::lock_guard<dlf_mutex> l(kdLock);

		if (form->keywords == nullptr || clear.find(form) == clear.end())
		{
			form->numKeywords = 0;
			form->keywords = new BGSKeyword	*[maxKeywords];
			clear.insert(form);
		}

		if (form->numKeywords >= maxKeywords - 1)
		{
			return ErrorHandling::SendError("ERROR:MiscUtility keywords limit reached");
		}

		if (keywordStr.empty())
			return;

		auto k = GetKeyword(nullptr, keywordStr.data());
		if (k != nullptr)
		{
			form->keywords[form->numKeywords] = k;
			++form->numKeywords;
		}
	}
}