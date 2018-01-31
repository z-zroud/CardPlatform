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


extern "C" HTDPPARSE_API bool HandleDp(const char* szFileName, const char* ruleFile);

class HTDpParser : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile);

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

    vector<string>              m_vecDGI;
    string						m_currentAccount;
    vector<string>              m_encryptTag;       //加密的tag
    vector<unsigned short>      m_valueOnlyDGI;     //仅还有value的tag
    map<string, string>         m_exchangeDGI;      //需要交换的tag
    string                      m_key;              //解密密钥
};


