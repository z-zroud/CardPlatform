// UtilTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Util\Tool.h"
#include "..\Util\SockServer.h"
#include "..\Util\NamedPipe.h"
#include "..\Util\SHA1.h"
#include "..\Util\Des0.h"
#include "..\\PCSC\PCSC.h"
#include <vector>
#include <string>
#include <bitset>
using namespace std;

/*******************************************************************
* Func: 奇偶校验
*******************************************************************/
string EvenOddCheck(string input)
{
    string result;
    if (input.length() % 2 != 0)
    {
        return "";
    }
    for (unsigned int i = 0; i < input.length(); i += 2)
    {
        int num = stoi(input.substr(i, 2), 0, 16);
        bitset<8> b(num);
        if (b.count() % 2 == 0 && b[0] == 0)
        {//前7位是偶数，且最后一位是0
            b.set(0);
        }
        else if (b.count() % 2 == 0 && b[0] == 1)
        {//前7位是奇数，且最后一位是1
            b.reset(0);
        }

        unsigned long res = b.to_ulong();
        char szResult[3] = { 0 };
        sprintf_s(szResult, 3, "%02X", res);
        result += szResult;
    }

    return result;
}

void Encrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen)
{
    int dataLen = strlen(input);
    int count = dataLen / 16;

    string leftKey = string(key).substr(0, 16);
    string rightKey = string(key).substr(16);
    string result;
    char output1[17] = { 0 };
    string data = input;
    for (int i = 0; i < count; i++)
    {
        string block = data.substr(i * 16, 16);
        str_xor(output1, (char*)block.c_str(), 16);
        Des(output1, (char*)leftKey.c_str(), output1);
        _Des(output1, (char*)rightKey.c_str(), output1);
        Des(output1, (char*)leftKey.c_str(), output1);
        result += output1;
    }
    strncpy_s(output, outputLen, result.c_str(), result.length());
}

void Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen)
{
    int dataLen = strlen(input);
    int count = dataLen / 16;

    string leftKey = string(key).substr(0, 16);
    string rightKey = string(key).substr(16);
    string result;
    char output1[17] = { 0 };
    string data = input;
    for (int i = count - 1; i >= 0; i--)
    {
        _Des(output1, (char*)leftKey.c_str(), (char*)data.substr(i * 16, 16).c_str());
        Des(output1, (char*)rightKey.c_str(), output1);
        _Des(output1, (char*)leftKey.c_str(), output1);
        if (i != 0)
        {
            char temp[17] = { 0 };
            strncpy_s(temp, 17, data.substr((i - 1) * 16, 16).c_str(), 16);
            str_xor(output1, temp, 16);
        }
        result = output1 + result;
    }
    int len = result.length();
    strncpy_s(output, outputLen, result.c_str(), result.length());
}

int main()
{
    //HANDLE hPipe1, hPipe2;
    //BOOL Finished;
    //char buf[] = "fuck\n";

    //LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\LogNamedPipe4");

    //DWORD cbWritten;
    //DWORD dwBytesToWrite = (DWORD)strlen(buf);

    ////Thread Init Data
    //DWORD threadId;
    //HANDLE hThread = NULL;

    //BOOL Write_St = TRUE;

    //Finished = FALSE;




    //hPipe1 = CreateFile(lpszPipename1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    ////hPipe2=CreateFile(lpszPipename2,	GENERIC_READ ,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);


    //if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE))//||(hPipe2 == NULL || hPipe2 == INVALID_HANDLE_VALUE))
    //{
    //    printf("Could not open the pipe  - (error %d)\n", GetLastError());

    //}
    //else
    //{

    //    //hThread = CreateThread( NULL, 0, &NET_RvThr, NULL, 0, NULL);
    //    int fuck = 0;
    //    while (fuck < 50)
    //    {
    //        WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
    //        fuck++;
    //    }
    //            
    //    CloseHandle(hPipe1);
    //    Finished = TRUE;
    //}

    //static bool hasOpen = false;
    //static NamedPipe* pipe = NULL;

    //if (!hasOpen)
    //{
    //    pipe = new NamedPipe();
    //    hasOpen = pipe->Open("LogNamedPipe4", PIPE_MODE::PIPE_WRITE);
    //}
    //int shit = 0;
    //if (hasOpen && pipe)
    //{
    //    while (shit < 50)
    //    {
    //        char xx[] = "shit\n";
    //        pipe->SendPipeMessage(xx, strlen(xx));
    //        shit++;
    //    }
    //        
    //}
        

    //Test Base64 decode
 //   int len = 0;
 //   string testBase64 = Tool::base64_decode("fccMwd6wotmr/UhcqZL1kdvI0BqmcJwI6fBEF374ecrbAZwPgp9b2hukA+PwoXeWRhyeoQV4jq4G//Wp4/Axp3OLeRxdYMRJiNJGr2HHPZdeXQcE9v41vA==",len);
 //   string testBase641 = Tool::StrToBcd(testBase64.c_str(),len);
 //   char tmp[2048] = { 0 };
 //   Tool::AscToBcd(tmp, (char*)testBase64.c_str(), len * 2);
 //   //Test StrToBcd 6C696A6965
 //   char* a = "lijie";
 //   string test = Tool::StrToBcd(a,5);
 //   //Test GetDirectory
 //   const char* filePath = "a\\b\\c.txt";
 //   string dir = Tool::GetDirectory(filePath);

 //   //Test BcdToStr
 //   char* bcd = "50525950463030303031";
 //   char str[32] = { 0 };
 //   Tool::BcdToStr(bcd, str, 32);
 //   string asciiStr = Tool::BcdToStr(bcd);
	////Test IntToStr
	//int value = 12;
	//char hexStr[5] = { 0 };
	//Tool::IntToStr(value, hexStr, Two_byte);

	//value = 160;
	//Tool::IntToStr(value, hexStr, Four_byte);


	////Test SubStr
	//char* src = "Hello! Welcome to C++ world.";
	//char dst[32] = { 0 };
	//Tool::SubStr(src, 7, 7, dst);

 //   //Test SplitStr
 //   string strSplit = "9F56|1|00|9F13|2|0000|9F4D|2|0B0A|";
 //   vector<string> subStrs;
 //   Tool::SplitStr(strSplit, "|", subStrs);

    int count = 0;
    char* readers[2] = { 0 };
    GetReaders(readers, count);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", readers[i]);
    }
    OpenReader(readers[0]);


    char resp[1024] = { 0 };

    int sw = SendApdu2("00A4040C 07 A0000002471001");
    sw = SendApdu("00840000 08", resp, 1024);
    string rndIC = resp;

    string c1 = "00000001";
    string c2 = "00000002";

    CSHA1 sha1;
    string mrz = "4C383938393032433C333639303830363139343036323336";
    string seeds = sha1.GetBCDHash(mrz);
    seeds = seeds.substr(0, 32);
    string seedsc1 = seeds + c1;
    string seedsc2 = seeds + c2;
    string c1Hash = sha1.GetBCDHash(seedsc1);
    string c2Hash = sha1.GetBCDHash(seedsc2);
    string keyA = c1Hash.substr(0, 16);
    string keyB = c1Hash.substr(16, 16);
    keyA = EvenOddCheck(keyA);
    keyB = EvenOddCheck(keyB);
    string keyEnc = keyA + keyB;
    keyA = c2Hash.substr(0, 16);
    keyB = c2Hash.substr(16, 16);
    keyA = EvenOddCheck(keyA);
    keyB = EvenOddCheck(keyB);
    string keyMac = keyA + keyB;


    
    string rndIFD = "781723860C06C226";
    string keyIFD = "0B795240CB7049B01C19B33E32804F0B";
    string S = rndIFD + rndIC + keyIFD;

    char output[128] = { 0 };
    Encrypt_Des3_CBC(keyEnc.c_str(), S.c_str(), output, 128);
    char mac[64] = { 0 };
    DES_3DES_CBC_MAC(output, (char*)keyMac.c_str(), "0000000000000000", mac);

    string extData = string(output) + mac;
    string cmd = "00820000 28" + extData;
    sw = SendApdu(cmd.c_str(), resp,1028);
    /***************************************************/
    //string respData = "46B9342A41396CD7386BF5803104D7CEDC122B9132139BAF2EEDC94EE178534F2F2D235D074D7449";
    string respData = resp;
    memset(output, 0, 128);
    Decrypt_Des3_CBC(keyEnc.c_str(), respData.c_str(), output, 128);
    string aptRndIC = string(output).substr(0, 16);
    string aptRndIFD = string(output).substr(16, 16);
    string keyIC = string(output).substr(32,32);

    char keySeeds[33] = { 0 };
    strncpy_s(keySeeds, 33, keyIFD.c_str(), keyIFD.length());
    str_xor(keySeeds, (char*)keyIC.c_str(), 32);

    seeds = keySeeds;
    seedsc1 = seeds + c1;
    seedsc2 = seeds + c2;
    c1Hash = sha1.GetBCDHash(seedsc1);
    c2Hash = sha1.GetBCDHash(seedsc2);
    keyA = c1Hash.substr(0, 16);
    keyB = c1Hash.substr(16, 16);
    keyA = EvenOddCheck(keyA);
    keyB = EvenOddCheck(keyB);
    string keySEnc = keyA + keyB;
    keyA = c2Hash.substr(0, 16);
    keyB = c2Hash.substr(16, 16);
    keyA = EvenOddCheck(keyA);
    keyB = EvenOddCheck(keyB);
    string keySMac = keyA + keyB;
    string ssc = rndIC.substr(rndIC.length() - 8) + rndIFD.substr(rndIFD.length() - 8);
    sw = SendApdu2("00A4020C 02 011E");
    memset(resp, 0, 1024);
    //char resp1[1024] = { 0 };
    //int sw1 = SendApdu("00B08100 00", resp1, 1024);
    //sw1 = SendApdu("00B08100 20", resp1, 1024);
    return 0;
}

