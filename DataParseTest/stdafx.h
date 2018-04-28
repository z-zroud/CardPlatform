// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\Debug\\DataParse.lib")
#pragma comment(lib,"..\\lib\\Debug\\Util.lib")
#else
#pragma comment(lib,"..\\lib\\Release\\DataParse.lib")
#endif

// TODO: reference additional headers your program requires here
