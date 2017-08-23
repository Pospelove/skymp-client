#include "../stdafx.h"
#include "Costile2.h"
#include "Skyrim/BSScript/BSScriptIFunctionArguments.h" // BSScript::ZeroFunctionArguments

#include <queue>

namespace Costile2
{
	Signal<void(TESObjectREFR *, std::string)> OnAnimationEvent;
	BSScript::IStackCallbackFunctorPtr nullCallback;
	BSScript::ZeroFunctionArguments zeroArgs;


	std::recursive_mutex mutex;
	const size_t count = 16;

	struct SessionData
	{
		SessionData() {}

		explicit SessionData(std::string realFuncName) : realFuncName(realFuncName)
		{
		}

		const std::string realFuncName;

		SInt32 ints[count];
		UInt32 uints[count];
		UInt8 bools[count];
		float floats[count];
		BSFixedString strings[count];
		UInt32 forms[count];
		BSScript::IStackCallbackFunctorPtr callback;
		BSScript::BSScriptObjectPtr scriptObject;
	};
	std::map<SInt32, SessionData> data;
	std::map<std::string, std::list<SInt32>> sessionIDs;

	void SetSessionCallback(SInt32 session, BSScript::IStackCallbackFunctorPtr callback)
	{
		try
		{
			data[session].callback = callback;
		}
		catch (...)
		{
		}
	}

	bool CallSimpleFunction(const char *name, BSScript::IStackCallbackFunctorPtr callback)
	{
		if (!g_skyrimVM)
			return false;
		auto vmState = g_skyrimVM->GetState();
		if (!vmState)
			return false;
		if (name)
			return vmState->CallStaticFunction("Costile2", name, &zeroArgs, callback);
		return false;
	}

	void GC(SInt32 maxid)
	{
		for (SInt32 id = 0; id != maxid; ++id)
			data.erase(id);
	}

	SInt32 CreateSession(std::string realFuncName)
	{
		static SInt32 id = 0;
		++id;
		if (id == 2147483000)
			id = 1;

		if (id > 0 && id % 10000 == 0)
			GC(id - 1);

		const bool exist = sessionIDs.find(realFuncName) != sessionIDs.end();
		if (!exist)
			sessionIDs.insert({ realFuncName, {} });

		sessionIDs[realFuncName].push_back(id);
		data.insert({ id, SessionData(realFuncName) });
		return id;
	}

	void SetInt(SInt32 session, SInt32 idx, SInt32 val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
				data[session].ints[idx] = val;
		}
		catch (...)
		{
		}
	}

	void SetUInt(SInt32 session, SInt32 idx, UInt32 val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
				data[session].uints[idx] = val;
		}
		catch (...)
		{
		}
	}

	void SetBool(SInt32 session, SInt32 idx, bool val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
				data[session].bools[idx] = val;
		}
		catch (...)
		{
		}
	}

	void SetFloat(SInt32 session, SInt32 idx, float val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
				data[session].floats[idx] = val;
		}
		catch (...)
		{
		}
	}

	void SetString(SInt32 session, SInt32 idx, const std::string &val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
			{
				static std::map<std::string, char *> map;
				if (map.find(val) == map.end())
				{
					map[val] = new char[val.size() + 1];
					map[val][val.size()] = NULL;
					strcpy(map[val], val.data());
				}
				data[session].strings[idx] = map[val];
			}
		}
		catch (...)
		{
		}
	}

	void SetTESForm(SInt32 session, SInt32 idx, TESForm *val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count && val != nullptr)
			{
				data[session].forms[idx] = val->formID;
			}
		}
		catch (...)
		{
		}
	}

	void FinalisePacking(SInt32 session)
	{
		std::lock_guard<std::recursive_mutex> l(mutex);
		if (data[session].forms[0])
		{
			auto realFuncName = data[session].realFuncName;

			if (realFuncName != "ObjectReference_TranslateTo"
				&& realFuncName != "Actor_KeepOffsetFromActor"
				//&& realFuncName != "Debug_SendAnimationEvent"
				)
				return;

			auto &IDsForThisFunc = sessionIDs[realFuncName];


			if (realFuncName != "Debug_SendAnimationEvent")
			{
				IDsForThisFunc.remove_if([session](const SInt32 &anySession) {
					if (data[session].forms[0] == data[anySession].forms[0])
						return true;
					else
						return false;
				});
				IDsForThisFunc.push_back(session);
			}


			/*class Pr
			{
			public:
				bool operator()(const SInt32 &s1, const SInt32 &s2)
				{
					auto form1 = (TESObjectREFR *)LookupFormByID(data[s1].forms[0]),
						form2 = (TESObjectREFR *)LookupFormByID(data[s2].forms[0]);
					if (form1->formType != FormType::Reference)
						return false;
					if (form2->formType != FormType::Reference)
						return true;
					const float dist1 = (cd::GetPosition(g_thePlayer) - cd::GetPosition(form1)).Length(),
						dist2 = (cd::GetPosition(g_thePlayer) - cd::GetPosition(form2)).Length();
					return dist1 < dist2;
				}
			};

			const std::set<SInt32, Pr> sessionIDsSorted = { sessionIDs[realFuncName].begin(), sessionIDs[realFuncName].end() };
			sessionIDs[realFuncName] = {};
			sessionIDs[realFuncName] = {
				sessionIDsSorted.begin(), sessionIDsSorted.end()
			};*/
		}
	}

	void SetTESForm(SInt32 session, SInt32 idx, RefHandle val)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			if (idx >= 0 && idx < count)
			{
				data[session].forms[idx] = val;
			}
		}
		catch (...)
		{
		}
	}

	float GetPi()
	{
		return 3.14;
	}

	SInt32 GetSession(BSFixedString realFuncName)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			auto &q = sessionIDs[realFuncName.c_str()];
			if (!q.empty())
			{
				auto result = q.front();
				q.pop_front();
				return result;
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	SInt32 GetInt(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return data[session].ints[idx];
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	UInt32 GetUInt(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return data[session].uints[idx];
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	bool GetBool(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return data[session].bools[idx] != NULL;
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	float GetFloat(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return data[session].floats[idx];
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	BSFixedString GetString(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return data[session].strings[idx];
			}
		}
		catch (...)
		{
		}
		return "";
	}

	TESForm *GetTESForm(SInt32 session, SInt32 idx)
	{
		try
		{
			if (idx >= 0 && idx < count)
			{
				std::lock_guard<std::recursive_mutex> l(mutex);
				return LookupFormByID(data[session].forms[idx]);
			}
		}
		catch (...)
		{
		}
		return 0;
	}

	bool ReturnTESForm(SInt32 session, TESForm *result)
	{
		try
		{
			BSScript::BSScriptVariable var;
			if (result)
			{
				auto policy = g_skyrimVM->GetState()->GetHandlePolicy();
				auto handle = policy->Create((UInt32)result->formType, result);
				g_skyrimVM->GetState()->ResolveScriptObject(handle, "Form", data[session].scriptObject);
				var.SetObject(data[session].scriptObject);
			}
			else
				var.SetNone();
			data[session].callback->Unk_01(var);
			return false;
		}
		catch (...)
		{
			try
			{
				BSScript::BSScriptVariable var;
				var.SetNone();
				data[session].callback->Unk_01(var);
			}
			catch (...)
			{
				Timer::Set(0, [=] {
					sd::PrintNote("Warning: Would crash (1)");
				});
				return true;
			}
			Timer::Set(0, [=] {
				sd::PrintNote("Warning: Would crash");
			});
		}
		return true;
	}

	void ReturnInt(SInt32 session, SInt32 result)
	{
		try
		{
			BSScript::BSScriptVariable var;
			var.SetInt(result);
			data[session].callback->Unk_01(var);
		}
		catch (...)
		{
		}
	}

	void ReturnFloat(SInt32 session, float result)
	{
		try
		{
			BSScript::BSScriptVariable var;
			var.type = var.kType_Float;
			var.data.f = result;
			data[session].callback->Unk_01(var);
		}
		catch (...)
		{
		}
	}

	void ReturnBool(SInt32 session, bool result)
	{
		try
		{
			BSScript::BSScriptVariable var;
			var.type = var.kType_Bool;
			var.data.b = result;
			data[session].callback->Unk_01(var);
		}
		catch (...)
		{
		}
	}

	void ReturnString(SInt32 session, BSFixedString result)
	{
		try
		{
			BSScript::BSScriptVariable var;
			var.type = var.kType_String;
			var.data.str = *new BSFixedString(result);
			data[session].callback->Unk_01(var);
		}
		catch (...)
		{
		}
	}

	void OnAnimEvent(TESObjectREFR *src, BSFixedString eventName)
	{
		return OnAnimationEvent(src, eventName.c_str());
	}

	void Register()
	{
		auto vmState = g_skyrimVM->GetState();
		SKSEScript::RegisterFunction("Costile2", "GetPi", Costile2::GetPi, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetInt", Costile2::GetInt, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetUInt", Costile2::GetUInt, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetBool", Costile2::GetUInt, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetFloat", Costile2::GetFloat, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetString", Costile2::GetString, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetTESForm", Costile2::GetTESForm, vmState);
		SKSEScript::RegisterFunction("Costile2", "GetSession", Costile2::GetSession, vmState);
		SKSEScript::RegisterFunction("Costile2", "ReturnTESForm", Costile2::ReturnTESForm, vmState);
		SKSEScript::RegisterFunction("Costile2", "ReturnString", Costile2::ReturnString, vmState);
		SKSEScript::RegisterFunction("Costile2", "ReturnInt", Costile2::ReturnInt, vmState);
		SKSEScript::RegisterFunction("Costile2", "ReturnFloat", Costile2::ReturnFloat, vmState);
		SKSEScript::RegisterFunction("Costile2", "ReturnBool", Costile2::ReturnBool, vmState);
		SKSEScript::RegisterFunction("Costile2", "OnAnimEvent", Costile2::OnAnimEvent , vmState);
	}
}