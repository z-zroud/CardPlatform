#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSEHT_EXPORTS
#define HTDPPARSE_API __declspec(dllexport)
#else
#define HTDPPARSE_API __declspec(dllimport)
#endif


extern "C" HTDPPARSE_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

class HTDpParser : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count);

protected:
    int     GetCardSequence();
    string  GetCardMark();
    char    GetCardType();
    int     GetCardDataLen();
    int     ParseEmbossData();
    int     ParseMagstripData(const char* szFileName);
    int     ParseCardData(const char* szFileName, const char* szRuleFile);
private:
    ifstream    m_DpFile;
    string      m_Magstrip1;
    string      m_Magstrip2;
    string      m_magstrip3;
};


