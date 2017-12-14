#pragma once
#include <Windows.h>

/****************************** ���ʼ��ܻ���̬��ӿ�*******************************/
//�ƿ��ļ�����
typedef DWORD(WINAPI *Func_dp_file_decrypt)(char* in, char* out);

//����֤��Ϣ
typedef DWORD(WINAPI *Func_card_verify)(char* in, char* out);

//��������KMC
typedef DWORD(WINAPI *Func_generate_en_kmc)(char* in, char* out);

//ת����
typedef DWORD(WINAPI *Func_change_encryption)(char* in, char* out);

//����MAC
typedef DWORD(WINAPI *Func_append_record_cal_mac)(char* in, char* out);


/****************************** ��д����̬��***************************************/

//Des0.dll�ӿ�
/*************************************************************************/
typedef void(__cdecl *PFunc_Des3_ECB)(char *, char *, char *, unsigned char);
typedef void( __cdecl *PFunc_str_xor)(char *, char *, int );
typedef void( __cdecl *PFunc_Des)(char *, char *, char *);
/*******************************���ʶ�̬��ӿڽ���***********************************/

template <typename T>
T GetDllFunc(const char* funcName, const char* dllPath)
{
    static bool isLoaded = false;
	static HINSTANCE hInst = NULL;
    T ret = NULL;
    if (!isLoaded)
    {
		isLoaded = true;
		char szPath[256] = { 0 };
		//HMODULE hHandle = GetModuleHandle("IssueCard.dll");
		GetModuleFileName(NULL, szPath, 256);
		//GetCurrentDirectory(256, szPath);
		string path = szPath;
		int index = path.find_last_of('\\');
		path = path.substr(0, index + 1) + dllPath;
		hInst = LoadLibrary(path.c_str());
		if (hInst == NULL)
		{
			return ret;
		}
		
    }
	ret = (T)GetProcAddress(hInst, funcName);
    return ret;
}

