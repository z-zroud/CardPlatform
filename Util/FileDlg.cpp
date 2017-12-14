#include "StdAfx.h"
#include "FileDlg.h"
#include <cstdio>
#include <io.h>

/***********************************************************
* 获取指定路径下所有文件，包括子目录。
************************************************************/
void CFileDlg::GetFiles(string path, vector<string>& files)
{
    //文件句柄  
    long   hFile = 0;
    //文件信息  
    struct _finddata_t fileinfo;
    string p = path + _T("\\*");
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,迭代之  
            //如果不是,加入列表  
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                files.push_back(fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

/***************************************************
* 打开文件对话框，并返回选定的文件名
****************************************************/
string CFileDlg::OpenFileDlg()
{
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
    ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
    ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
    ofn.lpstrFilter = _T("Describe Files (*.*)|*.*|All Files (*.*)|*.*||");//设置过滤  
    ofn.nFilterIndex = 1;//过滤器索引  
    ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
    ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
    ofn.lpstrInitialDir = NULL;//初始目录为默认  
    ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
    if (GetOpenFileName(&ofn))
    {
        return strFilename;
    }
    
    return _T("");
}

/***********************************************
* 打开文件夹对话框，并返回指定的文件夹名称
************************************************/
string CFileDlg::OpenFolderDlg()
{
    TCHAR szBuffer[MAX_PATH] = { 0 };
    BROWSEINFO bi = { 0 };
    bi.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
    bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区  
    bi.lpszTitle = TEXT("选择一个文件夹");//标题  
    bi.ulFlags = BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    if (SHGetPathFromIDList(idl, szBuffer)) 
    {
        return szBuffer;
    }
    
    return _T("");
}

/****************************************************
* 打开保存文件对话框
*****************************************************/
void CFileDlg::SaveFileDlg(const string &filePath)
{
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
    ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
    ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
    ofn.lpstrFilter = _T("Describe Files (*.*)|*.*|All Files (*.*)|*.*||");//设置过滤  
    ofn.nFilterIndex = 1;//过滤器索引  
    ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
    ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
    ofn.lpstrInitialDir = NULL;//初始目录为默认  
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//目录必须存在，覆盖文件前发出警告  
    ofn.lpstrTitle = TEXT("保存到");//使用系统默认标题留空即可  
    ofn.lpstrDefExt = TEXT("cpp");//默认追加的扩展名  
    if (GetSaveFileName(&ofn))
    {
        MessageBox(NULL, strFilename, TEXT("保存到"), 0);
    }
    else {
        MessageBox(NULL, TEXT("请输入一个文件名"), NULL, MB_ICONERROR);
    }
}
