#pragma once

#ifndef CI_INCLUDED
#define CI_INCLUDED

#include <cstdint>
#include <cstddef>

#include "..\Skyrim\include\Skyrim\NetImmerse\NiPoint3.h"

#include "ciGUI.h"
#include "ciOther.h"
#include "ciTypes.h"
#include "ciObject.h"
#include "ciItemType.h"
#include "ciMagicEffect.h"
#include "ciSpell.h"
#include "ciIActor.h"
#include "ciLocalPlayer.h"
#include "ciRemotePlayer.h"
#include "ciMain.h"

#endif

std::string WstringToString(const std::wstring &wstring);
std::wstring StringToWstring(const std::string &string);