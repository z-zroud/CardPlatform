// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib,"..//lib//Debug//PythonInterface.lib")
#pragma comment(lib,"..//lib//Debug//PCSC.lib")
#else
#pragma comment(lib,"..//lib//Release//PythonInterface.lib")
#pragma comment(lib,"..//lib//Release//PCSC.lib")
#endif
// TODO: 在此处引用程序需要的其他头文件
