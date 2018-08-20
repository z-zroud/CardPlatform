#pragma once
#include <vector>
#include <string>
#include <fstream>
using namespace std;

/*******************************************************
* ����ר�����ڴ���DP�����ļ���������Ҫʵ��д�ļ�����������
* ���ദ���ı��ļ����������ļ���������Щ�����������ı��ļ�
* ��С��ʹ��
********************************************************/
class ProcessFile
{
public:
    ProcessFile(string fileName);
    /**********************************************
    * ���ı��ļ������ж�ȡ�ļ�
    ***********************************************/
    string  ReadLine();
    string  Read(int &offset, int len);
    string  ReadBinary(int &offset, int len);
    int     ReadInt(int &offset);
    void    Save(string fileName, vector<string> lines);
private:
    ifstream m_file;
};