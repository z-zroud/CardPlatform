// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\Debug\\Util.lib")
#pragma comment(lib,"..\\lib\\Debug\\PCSC.lib")
#pragma comment(lib,"..\\lib\\Debug\\Authencation.lib")
#pragma comment(lib,"..\\lib\\Debug\\Des0.lib")
#else
#pragma comment(lib,"..\\lib\\Release\\Util.lib")
#pragma comment(lib,"..\\lib\\Release\\PCSC.lib")
#pragma comment(lib,"..\\lib\\Release\\Authencation.lib")
#pragma comment(lib,"..\\lib\\Release\\Des0.lib")
#endif



// TODO: reference additional headers your program requires here
