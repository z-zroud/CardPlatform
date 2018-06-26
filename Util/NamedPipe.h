#pragma once
#include <Windows.h>
#include <string>

enum PIPE_MODE
{
    PIPE_WRITE,
    PIPE_READ
};

class NamedPipe 
{
public:
    bool Open(std::string name, PIPE_MODE mode);
    void SendPipeMessage(char* szFormatString, ...);
    std::string RecvPipeMessage();
    void CloseNamedPipe();

private:
    HANDLE m_pipeHandle;
};
