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
#pragma comment(lib,"..\\Debug\\Util.lib")
#pragma comment(lib,"..\\Debug\\PCSC.lib")
#pragma comment(lib,"..\\Debug\\GenKey.lib")
#elif
#pragma comment(lib,"..\Release\\Util.lib")
#pragma comment(lib,"..\Release\\PCSC.lib")
#pragma comment(lib,"..\\Release\\GenKey.lib")
#endif
#pragma comment(lib,"../lib/Des0.lib")
// TODO: reference additional headers your program requires here
