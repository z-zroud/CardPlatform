#pragma once
#include <string>

using namespace std;

/**************************************************************
* CFileDlgΪ���ļ��Ի����࣬�����ļ����ļ��жԻ���Ĳ���
***************************************************************/
class CFileDlg
{
public:
    string OpenFileDlg();
    void SaveFileDlg(const string &filePath);
    string OpenFolderDlg();
    static void GetFiles(string path, vector<string>& files);
};