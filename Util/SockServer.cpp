#include "stdafx.h"
#include "SockServer.h"
#include <stdio.h>
#include <vector>

using namespace std;

bool SockServer::InitSock()
{
    WSADATA WSAData = { 0 };
    return 0 == WSAStartup(MAKEWORD(2,2),&WSAData);
}

void SockServer::ReleaseLibrary()
{
    WSACleanup();
}

void HandleData(SOCKET& client, pReciveCallBack pReciveFunc)
{
    vector<char> theVector;
    char buffer;
    int rVal;

    while (true)
    {
        rVal = recv(client, &buffer, 1, 0);
        if (rVal == SOCKET_ERROR)
        {
            int errorVal = WSAGetLastError();

            if (errorVal == WSAENOTCONN)
            {
                //socketError("Socket not connected!");
            }
            //socketError("Failed recv()");
            WSACleanup();
        }

        if (buffer == '\n')
        {
            char *data = new char[theVector.size() + 1];
            memset(data, 0, theVector.size() + 1);

            for (int i = 0; i<theVector.size(); i += 1)
            {
                data[i] = theVector[i];
            }

            //cout << data << endl;
            //return data;
        }
        else
        {
            theVector.push_back(buffer);
        }
    }
}

void SockServer::Recive(pReciveCallBack pReciveFunc)
{
    while (true)
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(SOCKADDR_IN);
        memset(&clientAddr, 0, addrLen);
        SOCKET clientSock = accept(m_serverSock, (SOCKADDR*)&clientAddr, &addrLen);
        char clientIp[32] = { 0 };
        inet_ntop(AF_INET, (void*)&clientAddr.sin_addr, clientIp, 32);
        //printf("Accepted client:%s:%d\n", clientIp, ntohs(clientAddr.sin_port));

        HandleData(clientSock, pReciveFunc);
    }  
}


int SockServer::CreateServer(const char* ip, unsigned short port)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return 1;
    }
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET,ip, (PVOID)&serverAddr.sin_addr.s_addr);

    int ret = bind(sock, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));
    if (INVALID_SOCKET == ret) {
        closesocket(sock);
        return 2;
    }
    ret = listen(sock, 5);
    if (INVALID_SOCKET == ret) {
        closesocket(sock);
        return 3;
    }
    m_serverSock = sock;
    return 0;
}

