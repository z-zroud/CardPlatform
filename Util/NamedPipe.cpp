#include "stdafx.h"
#include "NamedPipe.h"
using namespace std;

bool NamedPipe::Open(string name, PIPE_MODE mode)
{
    string pipeName = "\\\\.\\pipe\\" + name;
    if (mode == PIPE_MODE::PIPE_WRITE)
    {
        m_pipeHandle = CreateFile(pipeName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }
    else {
        m_pipeHandle = CreateFile(pipeName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    }
    if (m_pipeHandle == NULL || m_pipeHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    return true;
}

void NamedPipe::CloseNamedPipe()
{
    if (m_pipeHandle)
    {
        CloseHandle(m_pipeHandle);
        m_pipeHandle = NULL;
    }
}

void NamedPipe::SendPipeMessage(char* szMessage, int len)
{
    DWORD cbWritten = 0;
    WriteFile(m_pipeHandle, szMessage, len, &cbWritten, NULL);
}

void NamedPipe::SendPipeMessage(char* szFormatString, ...)
{
    char szMessage[2048] = { 0 };

    va_list ap;
    va_start(ap, szFormatString);
    _vsnprintf_s(szMessage, 2048, szFormatString, ap);
    va_end(ap);
    DWORD cbWritten = 0;
    WriteFile(m_pipeHandle, szMessage, strlen(szMessage), &cbWritten, NULL);
}