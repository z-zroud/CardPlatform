// UtilTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Util\Tool.h"
#include "..\Util\SockServer.h"
#include "..\Util\NamedPipe.h"
#include <vector>
using namespace std;

int main()
{
    HANDLE hPipe1, hPipe2;
    BOOL Finished;
    char buf[] = "fuck\n";

    LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\LogNamedPipe4");

    DWORD cbWritten;
    DWORD dwBytesToWrite = (DWORD)strlen(buf);

    //Thread Init Data
    DWORD threadId;
    HANDLE hThread = NULL;

    BOOL Write_St = TRUE;

    Finished = FALSE;




    hPipe1 = CreateFile(lpszPipename1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    //hPipe2=CreateFile(lpszPipename2,	GENERIC_READ ,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);


    if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE))//||(hPipe2 == NULL || hPipe2 == INVALID_HANDLE_VALUE))
    {
        printf("Could not open the pipe  - (error %d)\n", GetLastError());

    }
    else
    {

        //hThread = CreateThread( NULL, 0, &NET_RvThr, NULL, 0, NULL);
        int fuck = 0;
        while (fuck < 50)
        {
            WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
            fuck++;
        }
                
        CloseHandle(hPipe1);
        Finished = TRUE;
    }

    static bool hasOpen = false;
    static NamedPipe* pipe = NULL;

    if (!hasOpen)
    {
        pipe = new NamedPipe();
        hasOpen = pipe->Open("LogNamedPipe4", PIPE_MODE::PIPE_WRITE);
    }
    int shit = 0;
    if (hasOpen && pipe)
    {
        while (shit < 50)
        {
            char xx[] = "shit\n";
            pipe->SendPipeMessage(xx, strlen(xx));
            shit++;
        }
            
    }
        

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
    return 0;
}

