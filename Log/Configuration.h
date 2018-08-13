#pragma once
#include <stdio.h>
#include "Log.h"
/*********************************************************
* 该头文件主要用来维护日志类的状态信息结构，此信息并不对
* 用户开放
**********************************************************/

//一个存储日志状态信息的结构体
struct LogState
{
    log_t       handle;
    FILE*       file;
    char*       path;
    int         outputType;
};



LogState& GetExistedLog(const char* path);
LogState& GetExistedLog(log_t handle);
log_t AddNewLog(const char* path);


