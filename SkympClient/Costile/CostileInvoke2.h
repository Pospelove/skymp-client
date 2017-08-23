#pragma once
#include "Costile2.h"
#include "Value.h"
#include "Skyrim\BSScript\BSScriptIFunctionArguments.h"

#include <memory>
#include <thread>

struct Any
{
public:
	Any(BSScript::BSScriptVariable var)
		: var(var)
	{
	}

	explicit operator void()
	{
		return void();
	}

	explicit operator SInt32()
	{
		return var.data.i;
	}

	explicit operator UInt32()
	{
		return var.data.u;
	}

	explicit operator float()
	{
		return var.data.f;
	}

	explicit operator std::string()
	{
		return var.data.str;
	}

	explicit operator bool()
	{
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

	const BSScript::BSScriptVariable var;
};

struct CallState
{
	BSScript::BSScriptVariable result = {};
	bool gotResult = false;
};

CallState *GetCallState()
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
static void Pack(
	SInt32 session,
	SInt32 idx,
	T1 first,
	TN ... nth)
{
	PackSingle(session, idx, first);
	Pack(session, idx + 1, nth ...);
}

static void Pack(SInt32 session, SInt32 idx)
{
	Costile2::FinalisePacking(session);
}

template <class T>
static void PackSingle(
	SInt32 session,
	SInt32 idx,
	T value)
{
	PackSingle<const cd::IValue &>(session, idx, value); //redirect
}

template <>
static void PackSingle<SInt32>(
	SInt32 session,
	SInt32 idx,
	SInt32 value)
{
	Costile2::SetInt(session, idx, value);
}

template <>
static void PackSingle<UInt32>(
	SInt32 session,
	SInt32 idx,
	UInt32 value)
{
	Costile2::SetUInt(session, idx, value);
}

template <>
static void PackSingle<float>(
	SInt32 session,
	SInt32 idx,
	float value)
{
	Costile2::SetFloat(session, idx, value);
}

template <>
static void PackSingle<bool>(
	SInt32 session,
	SInt32 idx,
	bool value)
{
	Costile2::SetBool(session, idx, value);
}

template <>
static void PackSingle<std::string>(
	SInt32 session,
	SInt32 idx,
	std::string value)
{
	Costile2::SetString(session, idx, value);
}

template <>
static void PackSingle<const char *>(
	SInt32 session,
	SInt32 idx,
	const char *value)
{
	Costile2::SetString(session, idx, value);
}

template <>
static void PackSingle<const cd::IValue &>(
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

extern UInt32 g_SDThreadId;

template <class ResultT, class T1, class ... TN>
ResultT ExecImpl(
	bool isAsync,
	BSScript::IStackCallbackFunctorPtr callback,
	const std::string &className,
	const std::string &funcName,
	T1 first,
	TN ... nth)
{
	static std::mutex mutex;
	std::lock_guard<std::mutex> l(mutex);

	const auto realFuncName = className + '_' + funcName;
	const auto realScriptName = "Costile2_" + className;

	auto s = Costile2::CreateSession(realFuncName);
	Pack(s, 0, first, nth ...);
	Costile2::SetSessionCallback(s, BSScript::IStackCallbackFunctorPtr(callback));

	auto vmState = g_skyrimVM->GetState();
	if (vmState)
	{
		static UInt32 numThreads = 0;
		try
		{
			static std::recursive_mutex numThreadsMutex;
			std::lock_guard<std::recursive_mutex> l_(numThreadsMutex);
			++numThreads;
			//std::thread([=] {
				/*BSSpinLockGuard l[] = {
					vmState->classListLock,
					vmState->unk120,
					vmState->stackLock,
					vmState->unk4A50,
					vmState->unk4A68
				};*/

				std::lock(vmState->classListLock,
					vmState->unk120,
					vmState->stackLock,
					vmState->unk4A50,
					vmState->unk4A68);
				std::lock_guard<BSSpinLock> lg1(vmState->classListLock, std::adopt_lock);
				std::lock_guard<BSSpinLock> lg2(vmState->unk120, std::adopt_lock);
				std::lock_guard<BSSpinLock> lg3(vmState->stackLock, std::adopt_lock);
				std::lock_guard<BSSpinLock> lg4(vmState->unk4A50, std::adopt_lock);
				std::lock_guard<BSSpinLock> lg5(vmState->unk4A68, std::adopt_lock);
				static auto zeroArgs = BSScript::ZeroFunctionArguments();

				if (realFuncName != "ObjectReference_PlaceAtMe"
					&& realFuncName != "ObjectReference_PlaceActorAtMe"
					&& realFuncName != "ObjectReference_SetPosition"
					&& realFuncName != "Debug_SendAnimationEvent"
					&& realFuncName != "Actor_KeepOffsetFromActor"
					&& realFuncName != "ObjectReference_TranslateTo"
					&& realFuncName != "Actor_StartCombat")
				{
					vmState->CallStaticFunction(realScriptName.data(), realFuncName.data(), &zeroArgs, BSScript::IStackCallbackFunctorPtr(callback));
				}
				//vmState->CallStaticFunction("Game", realFuncName.data(), &zeroArgs, BSScript::IStackCallbackFunctorPtr(callback));

				std::lock_guard<std::recursive_mutex> l1(numThreadsMutex);
				--numThreads;
			//}).detach();
		}
		catch (...)
		{
			sd::PrintNote("Failed to create thread %u", numThreads);
		}
	}

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
			return ExecImpl<ResultT, T1, TN ...>(isAsync, new CostileCallback<ResultT>(callback, isAsync), className, funcName, first, nth ...);
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
			return ExecImpl<void, T1, TN ...>(isAsync, new CostileCallbackVoid(callback, isAsync), className, funcName, first, nth ...);
		}
	};
};