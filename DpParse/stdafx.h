// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\Debug\\Des0.lib")
#pragma comment(lib,"..\\lib\\Debug\\Util.lib")
#pragma comment(lib,"..\\lib\\Debug\\Authencation.lib")
#pragma comment(lib,"..\\lib\\Debug\\DataParse.lib")
#pragma comment(lib,"..\\lib\\Debug\\Tool.lib")
#else
#pragma comment(lib,"..\\lib\\Release\\Des0.lib")
#pragma comment(lib,"..\\lib\\Release\\Util.lib")
#pragma comment(lib,"..\\lib\\Release\\Authencation.lib")
#pragma comment(lib,"..\\lib\\Release\\DataParse.lib")
#pragma comment(lib,"..\\lib\\Release\\Tool.lib")
#endif
// TODO: reference additional headers your program requires here
