#include "../stdafx.h"
#include "CostileInvoke2.h"
#include "CDScript.h"

namespace cd
{
	using namespace Script;
}

struct cd::Context::Impl
{
};

cd::Context::Context()
{
}

cd::Context::~Context()
{
}

void cd::Context::Eval(std::string scriptString)
{
	if (GetCurrentThreadId() != g_SDThreadId)
	{
		SET_TIMER(0, [=] {
			Eval(scriptString);
		});
		return;
	}
	scriptString.erase(std::remove_if(scriptString.begin(), scriptString.end(), ::isspace), scriptString.end());
	std::string className;
	std::string funcName;
	std::vector<CostileArgument> arguments;
	enum Mode
	{
		ReadClass,
		ReadFunc,
		ReadArg,
		End
	};
	Mode mode = ReadClass;
	bool isNewArg = false;
	for (size_t i = 0; i != scriptString.size(); i++)
	{
		if (mode == ReadClass)
		{
			if (scriptString[i] == '.')
			{
				mode = ReadFunc;
				//className.erase(std::remove_if(className.begin(), className.end(), [](char ch) {return ch == '.'; }), className.end());
			}
			else
				className += scriptString[i];
		}
		else if (mode == ReadFunc)
		{
			if (scriptString[i] == '(')
			{
				mode = ReadArg;
				isNewArg = true;
			}
			else
				funcName += scriptString[i];
		}
		else if (mode == ReadArg)
		{
			if (scriptString[i] == ')')
			{
				mode = End;
			}
			else if (scriptString[i] == ',')
			{
				isNewArg = true;
			}
			else
			{
				if (isNewArg)
				{
					arguments.push_back({});
					arguments.back().type = CostileArgument::String;
					isNewArg = false;
				}
				arguments.back().data += scriptString[i];
			}
		}
		else if (mode == End)
		{
		}
	}

	auto convertArg = [](CostileArgument &arg) {
		if (arg.data.find('\'') != arg.data.npos)
		{
			arg.data.erase(std::remove_if(arg.data.begin(), arg.data.end(), [](char ch) {return ch == '\'';}), arg.data.end());
			arg.type = arg.String;
			return;
		}
		if (!memcmp(arg.data.data(), "Form:", 5))
		{
			arg.data = { arg.data.begin() + 5, arg.data.end() };
			arg.type = arg.Object;
			return;
		}
		if (std::to_string(atoi(arg.data.data())) == arg.data)
		{
			arg.type = arg.Int;
			return;
		}
		if (arg.data.find('.') != arg.data.npos)
		{
			arg.type = arg.Float;
			return;
		}
	};

	for (auto it = arguments.begin(); it != arguments.end(); ++it)
		convertArg(*it);

	// Custom cdscript functions:
	if (className == "Game" && funcName == "SetRotationTo")
	{
		ErrorHandling::SendError("DBG:CDScript SetRotationTo called");
		const uint32_t formID = (uint32_t)atoll(arguments[0].data.data());
		SET_TIMER_LIGHT(0, [=] {
			auto ref = (TESObjectREFR *)LookupFormByID(formID);
			if (ref && ref->formType == FormType::Reference)
			{
				sd::SetAngle(g_thePlayer, 0, 0, sd::GetAngleZ(ref));
			}
		});
	}

	ExecImpl(true, new CostileCallbackVoid({},true), className, funcName, arguments);
}