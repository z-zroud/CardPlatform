#pragma once
#include <ws2tcpip.h>
class SockServer
{
public:
    static bool InitSock();
    static void ReleaseLibrary();

public:
    int CreateServer(const char* ip, unsigned short port);
    void Recive();
protected:
    SOCKET m_serverSock;
};
