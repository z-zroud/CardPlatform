// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#pragma comment(lib,"..\\lib\\Des0.lib")
#pragma comment(lib,"..\\lib\\PCSC.lib")

#if _DEBUG
#pragma comment(lib,"..\\lib\\Util_d.lib")
#else
#pragma comment(lib,"..\\lib\\Util.lib")
#endif


// TODO: reference additional headers your program requires here
