#pragma once
#include "../CoreInterface/CoreInterface.h"

#include <set>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#include <RakPeer.h>

#include "Serialization.h"
#include "Config.h"
#include "MessageID.h"
#include "EncodeCyrillic.h"
#include "Tests.h"

#define MAX_NICKNAME							(24u)
#define MAX_PASSWORD							(32u)
#define ADD_PLAYER_ID_TO_NICKNAME_LABEL			FALSE