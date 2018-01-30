// UtilTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Util\Tool.h"
#include "..\Util\SockServer.h"

int main()
{
    //Test BcdToStr
    char* bcd = "50525950463030303031";
    char str[32] = { 0 };
    Tool::BcdToStr(bcd, str, 32);

	//Test IntToStr
	int value = 12;
	char hexStr[5] = { 0 };
	Tool::IntToStr(value, hexStr, Two_byte);

	value = 160;
	Tool::IntToStr(value, hexStr, Four_byte);


	//Test SubStr
	char* src = "Hello! Welcome to C++ world.";
	char dst[32] = { 0 };
	Tool::SubStr(src, 7, 7, dst);

    //Test socket server
    //SockServer server;
    //server.InitSock();
    //server.CreateServer("127.0.0.1", 8888);
    //server.Recive();
    //server.ReleaseLibrary();
    return 0;
}

