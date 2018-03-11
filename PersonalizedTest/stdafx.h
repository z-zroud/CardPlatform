// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib,"..//lib//Debug//CPS.lib")
#pragma comment(lib,"..//lib//Debug//PCSC.lib")
#pragma comment(lib,"..//lib//Debug//DpParse.lib")
#else
#pragma comment(lib,"..//lib//Release//CPS.lib")
#pragma comment(lib,"..//lib//Release//PCSC.lib")
#pragma comment(lib,"..//lib//Release//DpParse.lib")
#endif

// TODO: reference additional headers your program requires here
