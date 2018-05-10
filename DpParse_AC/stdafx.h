// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\Debug\\DpParse.lib")
#pragma comment(lib,"..\\lib\\Debug\\Util.lib")
#pragma comment(lib,"..\\lib\\Debug\\Des0.lib")
#else
#pragma comment(lib,"..\\lib\\Release\\DpParse.lib")
#pragma comment(lib,"..\\lib\\Release\\Util.lib")
#pragma comment(lib,"..\\lib\\Release\\Des0.lib")
#endif

// TODO: 在此处引用程序需要的其他头文件
