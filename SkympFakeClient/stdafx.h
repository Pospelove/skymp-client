#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <ctype.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
 
#include <RakPeer.h>
#include <MessageIdentifiers.h>

#include <CoreInterface\CoreInterface.h>
#include <ClientLogic\MessageID.h>
#include <ClientLogic\Serialization.h>