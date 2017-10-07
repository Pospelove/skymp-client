#pragma once
#include <memory>

namespace cd
{
	namespace Script
	{
		class Context
		{
		public:
			Context();
			~Context();

			void Example()
			{
				Eval("");
			}

			void Eval(std::string scriptString);
		private:
			struct Impl;
			std::shared_ptr<Impl> pimpl;
		};
	}
}