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
	const int                   m_reserved = 8596;  //�����������ļ�ͷ��С
	vector<string>              m_encryptTag;       //���ܵ�tag
	vector<unsigned short>      m_valueOnlyDGI;     //������value��tag
	string                      m_key;              //������Կ
};

extern "C" DPPARSE_ZJ_TL_API bool HandleDp(const char* szFileName, const char* ruleFile);