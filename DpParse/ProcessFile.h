#pragma once
#include <vector>
#include <string>
#include <fstream>
using namespace std;

/*******************************************************
* 此类专门用于处理DP数据文件，并不需要实现写文件操作，另外
* 该类处理文本文件、二进制文件，其中有些函数仅用于文本文件
* 请小心使用
********************************************************/
class ProcessFile
{
public:
    ProcessFile(string fileName);
    /**********************************************
    * 对文本文件，按行读取文件
    ***********************************************/
    string  ReadLine();
    string  Read(int &offset, int len);
    string  ReadBinary(int &offset, int len);
    int     ReadInt(int &offset);
    void    Save(string fileName, vector<string> lines);
private:
    ifstream m_file;
};