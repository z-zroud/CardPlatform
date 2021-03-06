// ReadCardNo.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ReadCardNo.h"
#include "../PCSC/PCSC.h"
#include "../ApduCmd/IApdu.h"
#include "../DataParse/IDataParse.h"

int GetReadersEx(char** readers, int& count)
{
    return GetReaders(readers, count);
}

int OpenReaderEx(char* readerName)
{
    return OpenReader(readerName) ? 0 : -1;
}


int ReadCardNo(const char* aid, int sfi, int recordNo, char* cardNo, int cardNoLen)
{
    char resp[1024] = { 0 };

    int sw = SelectAppCmd(aid, resp);
    if (sw == 0x9000)
    {
        memset(resp, 0, 1024);
        sw = ReadRecordCmd(2, 1, resp);
        if (sw == 0x9000)
        {
            TLV tlvs[20] = { 0 };
            unsigned int count = 20;
            if (ParseTLV(resp, tlvs, count))
            {
                for (int i = 0; i < count; i++)
                {
                    if (strcmp(tlvs[i].tag,"5A") == 0)
                    {
                        strncpy_s(cardNo, cardNoLen, tlvs[i].value, strlen(tlvs[i].value));
                        return 0;
                    }
                }
            }
        }
    }
    return -1;
}