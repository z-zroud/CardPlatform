// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#ifdef _DEBUG
#pragma comment(lib,"..//lib//Debug//ApduCmd.lib")
#pragma comment(lib,"..//lib//Debug//PCSC.lib")
#else
#pragma comment(lib,"..//lib//Release//ApduCmd.lib")
#pragma comment(lib,"..//lib//Release//PCSC.lib")
#endif

// TODO: reference additional headers your program requires here
