// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#if _DEBUG
#pragma comment(lib,"..\\lib\\Util_d.lib")
#else
#pragma comment(lib,"..\\lib\\Util.lib")
#endif


// TODO: reference additional headers your program requires here
