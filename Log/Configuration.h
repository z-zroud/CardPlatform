#pragma once
#include <stdio.h>
#include "Log.h"
/*********************************************************
* ��ͷ�ļ���Ҫ����ά����־���״̬��Ϣ�ṹ������Ϣ������
* �û�����
**********************************************************/

//һ���洢��־״̬��Ϣ�Ľṹ��
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


