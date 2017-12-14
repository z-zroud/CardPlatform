#pragma once
#include <string>

using namespace std;

/**************************************************************
* CFileDlg为打开文件对话框类，用于文件及文件夹对话框的操作
***************************************************************/
class CFileDlg
{
public:
    string OpenFileDlg();
    void SaveFileDlg(const string &filePath);
    string OpenFolderDlg();
    static void GetFiles(string path, vector<string>& files);
};