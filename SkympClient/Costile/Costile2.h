#pragma once
#include "../Common/Signal.h"

namespace Costile2
{
	extern Signal<void(TESObjectREFR *, std::string)> OnAnimationEvent;

	bool CallSimpleFunction(const char *constexprFuncName, BSScript::IStackCallbackFunctorPtr callback = nullptr);
	SInt32 CreateSession(std::string realFuncName);
	void SetSessionCallback(SInt32 session, BSScript::IStackCallbackFunctorPtr callback);
	void SetInt(SInt32 session, SInt32 idx, SInt32 val);
	void SetUInt(SInt32 session, SInt32 idx, UInt32 val);
	void SetBool(SInt32 session, SInt32 idx, bool val);
	void SetFloat(SInt32 session, SInt32 idx, float val);
	void SetString(SInt32 session, SInt32 idx, const std::string &val);
	void SetTESForm(SInt32 session, SInt32 idx, TESForm *val);
	//void SetTESForm(SInt32 session, SInt32 idx, BSScript::BSScriptObjectPtr val);
	void SetTESForm(SInt32 session, SInt32 idx, RefHandle val);
	void FinalisePacking(SInt32 session);
	void CreateStaticString(const std::string &source, int32_t strID);

	// Функции для вызова из Papyrus:
	float GetPi(); // Для теста
	SInt32 GetSession(BSFixedString realFuncName);
	SInt32 GetInt(SInt32 session, SInt32 idx);
	UInt32 GetUInt(SInt32 session, SInt32 idx);
	bool GetBool(SInt32 session, SInt32 idx);
	float GetFloat(SInt32 session, SInt32 idx);
	BSFixedString GetString(SInt32 session, SInt32 idx);
	TESForm *GetTESForm(SInt32 session, SInt32 idx);
	bool ReturnTESForm(SInt32 session, TESForm *result);
	void ReturnInt(SInt32 session, SInt32 result);
	void ReturnFloat(SInt32 session, float result);
	void ReturnBool(SInt32 session, bool result);
	void ReturnString(SInt32 session, BSFixedString result);
	void OnAnimEvent(TESObjectREFR *src, BSFixedString eventName);
	BSFixedString GetStringTest();
	BSFixedString GetStaticString(SInt32 id);

	// Зарегистрировать ф-ции выше в виртуальной машине Skyrim
	void Register();
}

			