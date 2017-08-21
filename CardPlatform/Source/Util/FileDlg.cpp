#include "StdAfx.h"
#include "FileDlg.h"
#include <cstdio>
#include <io.h>

void CFileDlg::GetFiles(string path, vector<string>& files)
{
    //�ļ����  
    long   hFile = 0;
    //�ļ���Ϣ  
    struct _finddata_t fileinfo;
    string p = path + _T("\\*");
    if ((hFile = _findfirst(p.c_str(), &fileinfo)) != -1)
    {
        do
        {
            //�����Ŀ¼,����֮  
            //�������,�����б�  
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

string CFileDlg::OpenFileDlg()
{
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  
    ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
    ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
    ofn.lpstrFilter = _T("Describe Files (*.*)|*.*|All Files (*.*)|*.*||");//���ù���  
    ofn.nFilterIndex = 1;//����������  
    ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
    ofn.nMaxFile = sizeof(strFilename);//����������  
    ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��  
    ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���  
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  
    if (GetOpenFileName(&ofn))
    {
        return strFilename;
    }
    
    return _T("");
}

string CFileDlg::OpenFolderDlg()
{
    TCHAR szBuffer[MAX_PATH] = { 0 };
    BROWSEINFO bi = { 0 };
    bi.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
    bi.pszDisplayName = szBuffer;//�����ļ��еĻ�����  
    bi.lpszTitle = TEXT("ѡ��һ���ļ���");//����  
    bi.ulFlags = BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    if (SHGetPathFromIDList(idl, szBuffer)) 
    {
        return szBuffer;
    }
    
    return _T("");
}

void CFileDlg::SaveFileDlg(const string &filePath)
{
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  
    ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
    ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
    ofn.lpstrFilter = _T("Describe Files (*.*)|*.*|All Files (*.*)|*.*||");//���ù���  
    ofn.nFilterIndex = 1;//����������  
    ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
    ofn.nMaxFile = sizeof(strFilename);//����������  
    ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��  
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//Ŀ¼������ڣ������ļ�ǰ��������  
    ofn.lpstrTitle = TEXT("���浽");//ʹ��ϵͳĬ�ϱ������ռ���  
    ofn.lpstrDefExt = TEXT("cpp");//Ĭ��׷�ӵ���չ��  
    if (GetSaveFileName(&ofn))
    {
        MessageBox(NULL, strFilename, TEXT("���浽"), 0);
    }
    else {
        MessageBox(NULL, TEXT("������һ���ļ���"), NULL, MB_ICONERROR);
    }
}
