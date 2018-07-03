#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSEYLMC_EXPORTS
#define YLMCDPPARSE_API __declspec(dllexport)
#else
#define YLMCDPPARSE_API __declspec(dllimport)
#endif


extern "C" YLMCDPPARSE_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

class YLMCDpParser : public IDpParse
{
public:
    YLMCDpParser();
    bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count);

private:
    void			ReadDGIName(ifstream &dpFile);                          //读取DGI名称
    char			ReadDGIStartTag(ifstream &dpFile);                      //读取起始标签
    int	            GetFollowedDataLen(ifstream &dpFile);                   //读取数据长度
    int				GetOneCardDpDataLen(ifstream &dpFile);	                //读取卡片个人化数据总长度
                                                                            //void			ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs);   //解析TLV结构
    int				ParsePSE(ifstream &dpFile, DGI &dgiItem);          //分析PSE PPSE
private:

    vector<string>              m_vecDGI;
    string						m_currentAccount;
    const int                   m_reserved = 8596;  //银联保留的文件头大小
    vector<string>              m_encryptTag;       //加密的tag
    vector<unsigned short>      m_valueOnlyDGI;     //仅还有value的tag
    map<string, string>         m_exchangeDGI;      //需要交换的tag
    string                      m_key;              //解密密钥
};

