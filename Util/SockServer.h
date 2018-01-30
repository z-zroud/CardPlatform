#pragma once
#include <ws2tcpip.h>

/***********************************************************
* 定义消息处理回调函数
************************************************************/
typedef void(*pReciveCallBack)(char*);

class SockServer
{
public:
    static bool InitSock();
    static void ReleaseLibrary();

public:
    int CreateServer(const char* ip, unsigned short port);
    void Recive(pReciveCallBack pReciveFunc);

private:
    void HandleData(SOCKET& client, pReciveCallBack pReciveFunc);
protected:
    SOCKET m_serverSock;
};
