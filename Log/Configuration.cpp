#include "stdafx.h"
#include "Configuration.h"

const int kMaxLog = 32;    //�洢�����ͬʱд���ٸ���־�ļ�
int kCurrentLogCount = 0;   //��¼��ǰʹ�õ���־�ļ�����

//���һ����¼���ڱ�����Ϣ��������ʵ����;ʹ��
LogState  kLogStatePool[kMaxLog + 1] = { 0 };
int kBaseAddr = 0x10000000;
#define IncreaseAddr(x)     ++x

bool HasExisted(const char* path)
{
    for (int i = 0; i < kMaxLog; i++)
    {
        if (kLogStatePool[i].path)
        {
            if (strcmp(kLogStatePool[i].path, path) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

LogState& GetExistedLog(log_t handle)
{
    for (int i = 0; i < kMaxLog; i++)
    {
        if (kLogStatePool[i].handle == handle)
        {
            return kLogStatePool[i];
        }
    }
    return kLogStatePool[kMaxLog];
}

LogState& GetExistedLog(const char* path)
{    
    for (int i = 0; i < kMaxLog; i++)
    {
        if (strcmp(kLogStatePool[i].path, path) == 0)
        {
            return kLogStatePool[i];
        }
    }
    return kLogStatePool[kMaxLog];
}


log_t AddNewLog(const char* path)
{
    if (HasExisted(path))
    {
        LogState state = GetExistedLog(path);
        state.handle;
    }
    if (kCurrentLogCount == kMaxLog)
    {
        return 0;
    }
    for (int i = 0; i < kMaxLog; i++)
    {
        if (kLogStatePool[i].handle == 0)
        {
            fopen_s(&kLogStatePool[i].file, path, "a+");
            if (!kLogStatePool[i].file)
                return 0;
            kLogStatePool[i].handle = IncreaseAddr(kBaseAddr);
            kLogStatePool[i].path = new char[128];
            memset(kLogStatePool[i].path, 0, 128);
            strncpy_s(kLogStatePool[i].path, 127, path, strlen(path));
            fclose(kLogStatePool[i].file);
            return kLogStatePool[i].handle;
        }
    }
    return 0;
}