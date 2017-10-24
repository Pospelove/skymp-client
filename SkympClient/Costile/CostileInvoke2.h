#pragma once
#include "Costile2.h"
#include "Value.h"
#include "Skyrim\BSScript\BSScriptIFunctionArguments.h"

#include <memory>
#include <thread>

#define CAPTURE_LOCKS						FALSE

namespace ci
{
	namespace Chat
	{
		void AddMessage(const std::wstring &message, bool isNotification = false);
	}
} // nice architectural solution

inline bool IsTraceCDCalls() {
	return getenv("IsTraceCDCalls") != nullptr && !strcmp(getenv("IsTraceCDCalls"), "1");
}

struct CostileArgument
{
	enum Type {
		String = 's',
		Float = 'f',
		Int = 'i',
		Object = 'o',
		Bool = 'b'
	};
	char type;
	std::string data;
};

struct Any
{
public:
	Any(BSScript::BSScriptVariable var)
		: var(var)
	{
	}

	explicit operator void() {
		return void();
	}

	explicit operator SInt32() {
		return var.data.i;
	}

	explicit operator UInt32() {
		return var.data.u;
	}

	explicit operator float() {
		return var.data.f;
	}

	explicit operator std::string() {
		return var.data.str;
	}

	explicit operator bool() {
		return var.data.b;
	}

	template <class T>
	explicit operator cd::Value<T>()
	{
		cd::Value<T> result;
		const auto scriptObject = var.data.object;
		if (scriptObject && var.data.u != NULL)
		{
			auto vmState = g_skyrimVM->GetState();
			auto policy = vmState->GetHandlePolicy();
			BSSpinLockGuard l(policy->m_lock);
			result = (T *)policy->Resolve<TESForm>(scriptObject->GetHandle());
		}
		return result;
	}

	explicit operator CostileArgument()
	{
		CostileArgument result;
		switch (var.type)
		{
		case BSScript::BSScriptType::kType_Int:
			result.type = CostileArgument::Int;
			result.data = std::to_string((int32_t)*this);
			break;
		case BSScript::BSScriptType::kType_Float:
			result.type = CostileArgument::Float;
			result.data = std::to_string((float)*this);
			break;
		case BSScript::BSScriptType::kType_String:
			result.type = CostileArgument::Float;
			result.data = std::string(*this);
			break;
		case BSScript::BSScriptType::kType_Object:
			result.type = CostileArgument::Object;
			result.data = std::to_string(((cd::Value<TESForm>)*this).GetFormID());
			break;
		case BSScript::BSScriptType::kType_None:
			result.type = CostileArgument::Object;
			result.data = "0";
		}
		return result;
	}

	const BSScript::BSScriptVariable var;
};

struct CallState
{
	BSScript::BSScriptVariable result = BSScript::BSScriptVariable();
	bool gotResult = false;
};

inline CallState *GetCallState()
{
	static CallState callState;
	return &callState;
}


template <class ResultT>
class CostileCallback : public BSScript::IStackCallbackFunctor
{
public:
	virtual ~CostileCallback() {};
	virtual bool Unk_02(void) override { return FALSE; };
	virtual void Unk_03(BSScript::BSScriptObjectPtr &arg1) override {};

	CostileCallback(std::function<void(ResultT)> callback, bool isAsync)
		: callback(callback), isAsync(isAsync)
	{
	}

	virtual void Unk_01(BSScript::BSScriptVariable var) override
	{
		try
		{
			if (!isAsync)
			{
				GetCallState()->result = var;
				GetCallState()->gotResult = true;
			}
			this->callback((ResultT)Any(var));
		}
		catch (...)
		{
		}
	};

private:
	const std::function<void(ResultT)> callback;
	const bool isAsync;
};

class CostileCallbackVoid : public BSScript::IStackCallbackFunctor
{
public:
	virtual ~CostileCallbackVoid() {};
	virtual bool Unk_02(void) override { return FALSE; };
	virtual void Unk_03(BSScript::BSScriptObjectPtr &arg1) override {};

	CostileCallbackVoid(std::function<void()> callback, bool isAsync)
		: callback(callback), isAsync(isAsync)
	{
	}

	virtual void Unk_01(BSScript::BSScriptVariable var) override
	{
		try
		{
			if (!isAsync)
			{
				GetCallState()->result = var;
				GetCallState()->gotResult = true;
			}
			if (this->callback)
				this->callback();
		}
		catch (...)
		{
		}
	};

private:
	const std::function<void()> callback;
	const bool isAsync;
};

template <class T1, class ... TN>
inline void Pack(
	SInt32 session,
	SInt32 idx,
	T1 first,
	TN ... nth)
{
	PackSingle(session, idx, first);
	Pack(session, idx + 1, nth ...);
}

inline void Pack(SInt32 session, SInt32 idx)
{
	Costile2::FinalisePacking(session);
}

template <class T>
inline void PackSingle(
	SInt32 session,
	SInt32 idx,
	T value)
{
	PackSingle<const cd::IValue &>(session, idx, value); //redirect
}

template <>
inline void PackSingle<SInt32>(
	SInt32 session,
	SInt32 idx,
	SInt32 value)
{
	Costile2::SetInt(session, idx, value);
}

template <>
inline void PackSingle<UInt32>(
	SInt32 session,
	SInt32 idx,
	UInt32 value)
{
	Costile2::SetUInt(session, idx, value);
}

template <>
inline void PackSingle<float>(
	SInt32 session,
	SInt32 idx,
	float value)
{
	Costile2::SetFloat(session, idx, value);
}

template <>
inline void PackSingle<bool>(
	SInt32 session,
	SInt32 idx,
	bool value)
{
	Costile2::SetBool(session, idx, value);
}

template <>
inline void PackSingle<std::string>(
	SInt32 session,
	SInt32 idx,
	std::string value)
{
	Costile2::SetString(session, idx, value);
}

template <>
inline void PackSingle<const char *>(
	SInt32 session,
	SInt32 idx,
	const char *value)
{
	Costile2::SetString(session, idx, value);
}

template <>
inline void PackSingle<const cd::IValue &>(
	SInt32 session,
	SInt32 idx,
	const cd::IValue &value)
{
	Costile2::SetTESForm(session, idx, value.GetFormID());
}

/*template <>
static void PackSingle<BSScript::BSScriptObjectPtr>(
	SInt32 session,
	SInt32 idx,
	BSScript::BSScriptObjectPtr value)
{
	Costile2::SetTESForm(session, idx, value);
}*/

inline void CallVMFunc(const std::string &className, const std::string &funcName, BSScript::IStackCallbackFunctorPtr callback)
{
	const auto realFuncName = className + '_' + funcName;
	const auto realScriptName = "Costile2_" + className;

	auto vmState = g_skyrimVM->GetState();
	if (vmState)
	{
		std::thread([=] {
			try {
				if (CAPTURE_LOCKS != FALSE)
				{
					std::lock(vmState->classListLock,
						vmState->unk120,
						vmState->stackLock,
						vmState->unk4A50,
						vmState->unk4A68);
					std::lock_guard<BSSpinLock> lg1(vmState->classListLock, std::adopt_lock),
						lg2(vmState->unk120, std::adopt_lock),
						lg3(vmState->stackLock, std::adopt_lock),
						lg4(vmState->unk4A50, std::adopt_lock),
						lg5(vmState->unk4A68, std::adopt_lock);
				}
				static auto zeroArgs = BSScript::ZeroFunctionArguments();

				static const std::set<std::string> autoCalledFns = {
					"ObjectReference_PlaceAtMe",
					"ObjectReference_PlaceActorAtMe",
					"ObjectReference_SetPosition",
					"Debug_SendAnimationEvent",
					"Actor_KeepOffsetFromActor",
					"ObjectReference_TranslateTo",
					"Actor_StartCombat",
					"UILIB_1_ShowList",
					"UILIB_1_ShowListResult",
					"UILIB_1_ShowInput",
					"UILIB_1_ShowInputResult",
					"UILIB_1_SetDataInput"
				};
				if (autoCalledFns.find(realFuncName) == autoCalledFns.end())
					vmState->CallStaticFunction(realScriptName.data(), realFuncName.data(), &zeroArgs, BSScript::IStackCallbackFunctorPtr(callback));
			}
			catch (...) {
				ErrorHandling::SendError("ERROR:CostileInvoke %s.%s()", className.data(), funcName.data());
			}
		}).detach();
	}
}

extern UInt32 g_SDThreadId;


static std::mutex execImplMutex;

inline CostileArgument ExecImpl(bool isAsync,
	BSScript::IStackCallbackFunctorPtr callback,
	const std::string &className,
	const std::string &funcName, 
	const std::vector<CostileArgument> &arguments)
{
	std::lock_guard<std::mutex> l(execImplMutex);

	const auto realFuncName = className + '_' + funcName;
	auto s = Costile2::CreateSession(realFuncName);
	for (size_t i = 0; i != arguments.size(); ++i)
	{
		using T = CostileArgument::Type;
		switch (arguments[i].type)
		{
		case T::Int:
			PackSingle(s, i, atoi(arguments[i].data.data()));
			break;
		case T::Bool:
			PackSingle(s, i, arguments[i].data == "true" || arguments[i].data == "1");
			break;
		case T::Float:
			PackSingle(s, i, (float)atof(arguments[i].data.data()));
			break;
		case T::String:
			PackSingle(s, i, arguments[i].data);
			break;
		case T::Object:
			std::istringstream idStream(arguments[i].data);
			uint32_t formID;
			idStream >> formID;
			PackSingle(s, i, (cd::Value<TESForm>)LookupFormByID(formID));
			break;
		}
	}
	Costile2::SetSessionCallback(s, BSScript::IStackCallbackFunctorPtr(callback));

	CallVMFunc(className, funcName, callback);

	if (!isAsync)
	{
		while (!GetCallState()->gotResult)
		{
			if (GetCurrentThreadId() == g_SDThreadId)
				sd::Wait(0);
		}
		GetCallState()->gotResult = false;
		return CostileArgument(Any(GetCallState()->result));
	}

	return CostileArgument();
}

template <class ResultT, class T1, class ... TN>
inline ResultT ExecImpl(
	bool isAsync,
	BSScript::IStackCallbackFunctorPtr callback,
	const std::string &className,
	const std::string &funcName,
	T1 first,
	TN ... nth)
{
	std::lock_guard<std::mutex> l(execImplMutex);

	const auto realFuncName = className + '_' + funcName;
	auto s = Costile2::CreateSession(realFuncName);
	Pack(s, 0, first, nth ...);
	Costile2::SetSessionCallback(s, BSScript::IStackCallbackFunctorPtr(callback));

	CallVMFunc(className, funcName, callback);

	if (!isAsync)
	{
		while (!GetCallState()->gotResult)
		{
			if (GetCurrentThreadId() == g_SDThreadId)
				sd::Wait(0);
		}
		GetCallState()->gotResult = false;
		return ResultT(Any(GetCallState()->result));
	}

	return ResultT();
}

class CostileInvoke2
{
public:
	template <class ResultT>
	class Invoke
	{
	public:
		template <class T1, class ... TN>
		static ResultT Exec(
			bool isAsync,
			std::function<void(ResultT)> callback, // Async calls only
			const std::string &className,
			const std::string &funcName,
			T1 first,
			TN ... nth)
		{
			if(IsTraceCDCalls())
			{
				const clock_t invokeStart = clock();
				callback = [=](ResultT result) {
					const clock_t invokeEnd = clock();
					if (callback != nullptr)
						callback(result);
					const clock_t callbackEnd = clock();
					std::stringstream ss;
					ss << className << '.' << funcName << "() ";
					ss << "invoked in " << invokeEnd - invokeStart << ' ';
					if (callback != nullptr)
						ss << "callback exec in " << callbackEnd - invokeEnd << ' ';
					ci::Chat::AddMessage(StringToWstring(ss.str()));
				};
			}
			const auto res = ExecImpl<ResultT, T1, TN ...>(isAsync, new CostileCallback<ResultT>(callback, isAsync), className, funcName, first, nth ...);
			return res;
		}
	};

	template <>
	class Invoke<void>
	{
	public:
		template <class T1, class ... TN>
		static void Exec(
			bool isAsync,
			std::function<void()> callback, // Async calls only
			const std::string &className,
			const std::string &funcName,
			T1 first,
			TN ... nth)
		{
			if(IsTraceCDCalls())
			{
				const clock_t invokeStart = clock();
				callback = [=] {
					const clock_t invokeEnd = clock();
					if (callback != nullptr)
						callback();
					const clock_t callbackEnd = clock();
					std::stringstream ss;
					ss << className << '.' << funcName << "() ";
					ss << "invoked in " << invokeEnd - invokeStart << ' ';
					if (callback != nullptr)
						ss << "callback exec in " << callbackEnd - invokeEnd << ' ';
					ci::Chat::AddMessage(StringToWstring(ss.str()));
				};
			}
			return ExecImpl<void, T1, TN ...>(isAsync, new CostileCallbackVoid(callback, isAsync), className, funcName, first, nth ...);
		}
	};
};