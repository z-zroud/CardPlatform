#pragma once
#include <Windows.h>

/****************************** 访问加密机动态库接口*******************************/
//制卡文件解密
typedef DWORD(WINAPI *Func_dp_file_decrypt)(char* in, char* out);

//卡认证信息
typedef DWORD(WINAPI *Func_card_verify)(char* in, char* out);

//生成密文KMC
typedef DWORD(WINAPI *Func_generate_en_kmc)(char* in, char* out);

//转加密
typedef DWORD(WINAPI *Func_change_encryption)(char* in, char* out);

//生产MAC
typedef DWORD(WINAPI *Func_append_record_cal_mac)(char* in, char* out);


/****************************** 读写器动态库***************************************/

//Des0.dll接口
/*************************************************************************/
typedef void(__cdecl *PFunc_Des3_ECB)(char *, char *, char *, unsigned char);
typedef void( __cdecl *PFunc_str_xor)(char *, char *, int );
typedef void( __cdecl *PFunc_Des)(char *, char *, char *);
/*******************************访问动态库接口结束***********************************/

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

