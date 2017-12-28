#pragma once
#include "../DpParse/IDpParse.h"

#ifdef DPPARSE_ZJ_TL_EXPORTS
#define DPPARSE_ZJ_TL_API __declspec(dllexport)
#else
#define DPPARSE_ZJ_TL_API __declspec(dllimport)
#endif


class ZJTLDpParse : public IDpParse
{
public:
	virtual bool HandleDp(const char* szFileName, const char* ruleFile);

protected:
	void ReadDGIName(ifstream &dpFile);
	void ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs);
private:
	vector<string>              m_vecDGI;
	string						m_currentAccount;
	const int                   m_reserved = 8596;  //银联保留的文件头大小
	vector<string>              m_encryptTag;       //加密的tag
	vector<unsigned short>      m_valueOnlyDGI;     //仅还有value的tag
	string                      m_key;              //解密密钥
};

extern "C" DPPARSE_ZJ_TL_API bool HandleDp(const char* szFileName, const char* ruleFile);