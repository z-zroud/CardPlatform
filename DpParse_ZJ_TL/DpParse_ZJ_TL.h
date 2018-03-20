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
	virtual bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

protected:
	void ReadDGIName(ifstream &dpFile);
    string GetDpMark(ifstream& dpFile);
    string GetDGIStartMark(ifstream& dpFile);
    int GetDGIDataLen(ifstream& dpFile);
    void ParsePSE(ifstream& dpFile, CPS_ITEM& cpsItem, string dgiName);
private:
	vector<string>              m_vecDGI;
	string						m_currentAccount;
};

extern "C" DPPARSE_ZJ_TL_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);