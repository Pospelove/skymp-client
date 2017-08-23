#pragma once
#include "SKSE/GameForms.h"

namespace cd
{
	class IValue
	{
	public:
		virtual VMHandle GetVMHandle() const = 0;
		virtual RefHandle GetFormID() const = 0;
		virtual ~IValue() {};
	};

	template <typename T>
	class Value : public IValue
	{
	public:
		Value()
		{
		}

		Value(T *rawPointer)
		{
			if (rawPointer)
 				this->formID = rawPointer->formID;
		}

		explicit Value(RefHandle formID)
		{
			this->formID = formID;
		}

		virtual ~Value()
		{
		}

		template <typename T2>
		operator Value<T2>() const
		{
			static T2 *test = (T *)0x1;

			Value<T2> result(this->formID);
			return result;
		}

		operator T*() const
		{
			return (T*)(this->formID ? LookupFormByID(this->formID) : 0);
		}

		virtual VMHandle GetVMHandle() const override
		{
			return NULL;
		}

		virtual RefHandle GetFormID() const override
		{
			return this->formID;
		}

	private:
		RefHandle formID = 0;
	};

	/*template <typename T>
	class Value : public IValue
	{
	public:
		Value()
		{
		}

		Value(T *rawPointer)
		{
			if (rawPointer)
				this->Init(rawPointer);
		}

		Value(UInt32 formID)
		{
			auto rawPointer = LookupFormByID(formID);
			if (rawPointer && rawPointer->formType == T::kTypeID)
				this->Init(rawPointer);
		}

		virtual ~Value()
		{
		}

		virtual VMHandle GetVMHandle() const override
		{
			return impl ? impl->handle : 0;
		}

		template <typename T2>
		operator Value<T2>() const
		{
			static T2 *test = (T *)0x1;

			Value<T2> result;
			result.impl = *(decltype(&result.impl))&this->impl;
			return result;
		}

		operator T*() const
		{
			if (!impl)
				return nullptr;
			auto vmState = g_skyrimVM->GetState();
			auto policy = vmState->GetHandlePolicy();
			BSSpinLockGuard l(policy->m_lock);
			return (T *)policy->Resolve<TESForm>(impl->handle);
		}

	public:

		void Init(T *rawPointer)
		{
			auto vmState = g_skyrimVM->GetState();
			auto policy = vmState->GetHandlePolicy();
			BSSpinLockGuard l(policy->m_lock);
			const auto handle = policy->Create((UInt32)rawPointer->formType, rawPointer);
			impl = std::shared_ptr<Impl>(new Impl(handle));
		}


		class Impl
		{
		public:

			Impl(VMHandle handle) :
				handle(handle)
			{
			}

			~Impl()
			{
				if (this->handle)
				{
					auto vmState = g_skyrimVM->GetState();
					auto policy = vmState->GetHandlePolicy();
					BSSpinLockGuard l(policy->m_lock);
					policy->Release(this->handle);
				}
			}

			const VMHandle handle;
		};

		std::shared_ptr<Impl> impl = nullptr;
	};*/

}