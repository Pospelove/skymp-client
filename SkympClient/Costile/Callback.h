#pragma once
#include "Costile2.h"
#include "Value.h"
#include "CostileInvoke.h"

namespace cd
{
	class Callback
	{
	public:
		Callback()
		{
		}

		Callback(std::function<void(void)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f();
				}
				catch (...)
				{
				}
			};
		}

		Callback(std::function<void(SInt32)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f(var.data.i);
				}
				catch (...)
				{
				}
			};
		}

		Callback(std::function<void(UInt32)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f(var.data.u);
				}
				catch (...)
				{
				}
			};
		}

		Callback(std::function<void(bool)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f(var.data.b);
				}
				catch (...)
				{
				}
			};
		}

		Callback(std::function<void(float)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f(var.data.f);
				}
				catch (...)
				{
				}
			};
		}

		Callback(std::function<void(std::string)> f)
		{
			realCallback = [=](Var var) {
				try
				{
					f(var.data.str);
				}
				catch (...)
				{
				}
			};
		}

		template <typename T>
		Callback(std::function<void(Value<T>)> f)
		{
			realCallback = [=](Var var) {
#ifdef GetObject
#undef GetObject
#endif
				Value<T> result;
				const auto scriptObject = var.GetObject();
				if (scriptObject)
				{
					const VMHandle handle = scriptObject->GetHandle();
					result.impl = std::shared_ptr<Value<T>::Impl>(new Value<T>::Impl(handle));
				}
				try
				{
					f(result);
				}
				catch (...)
				{
				}
			};
		}

		void operator()(Var var)
		{
			try
			{
				realCallback(var);
			}
			catch (...)
			{
			}
		}

	private:
		std::function<void(Var)> realCallback;
	};
}