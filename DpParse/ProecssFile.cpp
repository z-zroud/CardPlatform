#include "stdafx.h"
#include "ProcessFile.h"
#include "../Tool/tool.h"

ProcessFile::ProcessFile(string fileName)
{
    m_file.open(fileName, ios::in | ios::binary);
}

string ProcessFile::ReadLine()
{
    char* data = new char[100 * 1024];
    memset(data, 0, 100 * 1024);
    m_file.getline(data, 100 * 1024);

    string oneLineData(data);
    int indexR = oneLineData.find_last_of('\r');
    int indexN = oneLineData.find_last_of('\n');
    if (indexN != string::npos)
        oneLineData = oneLineData.substr(0, indexN);
    if (indexR != string::npos)
        oneLineData = oneLineData.substr(0, indexR);
    delete data;

    return oneLineData;
}

string ProcessFile::Read(int &offset, int len)
{
    m_file.seekg(offset, ios::beg);
    char* buf = new char[len + 1];
    memset(buf, 0, len + 1);
    m_file.read(buf, len);
    offset = m_file.tellg();
    string buffer(buf);

    delete buf;
    return buffer;
}

string ProcessFile::ReadBinary(int &offset, int len)
{
    m_file.seekg(offset, ios::beg);
    char* buf = new char[len + 1];
    memset(buf, 0, len + 1);
    m_file.read(buf, len);
    offset = m_file.tellg();
    int bcdLen = len * 2 + 1;
    char* bcd = new char[bcdLen];
    memset(bcd, 0, bcdLen);
    StrToBcd(buf, len, bcd, bcdLen);
    string buffer(bcd);

    delete buf;
    delete bcd;

    return buffer;
}

int ProcessFile::ReadInt(int &offset)
{
    string value =  ReadBinary(offset, 2);
    return HexStrToInt(value.c_str());
}

void ProcessFile::Save(string fileName, vector<string> lines)
{
    int pos = fileName.find_last_of('\\');
    string folder = fileName.substr(0, pos);
    WIN32_FIND_DATA  wfd;
    bool exsited = false;
    HANDLE hFind = FindFirstFile(folder.c_str(), &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        exsited = true;
    }
    FindClose(hFind);


    if (!exsited)
    {
        CreateDirectory(folder.c_str(), NULL);
    }
    ofstream outputFile(fileName);
    if (!outputFile)
        return;

    for (auto line : lines)
    {
        outputFile << line << endl;
    }

    outputFile.clear();
    outputFile.close();
}