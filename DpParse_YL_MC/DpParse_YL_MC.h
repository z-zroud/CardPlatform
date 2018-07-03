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
    void			ReadDGIName(ifstream &dpFile);                          //��ȡDGI����
    char			ReadDGIStartTag(ifstream &dpFile);                      //��ȡ��ʼ��ǩ
    int	            GetFollowedDataLen(ifstream &dpFile);                   //��ȡ���ݳ���
    int				GetOneCardDpDataLen(ifstream &dpFile);	                //��ȡ��Ƭ���˻������ܳ���
                                                                            //void			ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs);   //����TLV�ṹ
    int				ParsePSE(ifstream &dpFile, DGI &dgiItem);          //����PSE PPSE
private:

    vector<string>              m_vecDGI;
    string						m_currentAccount;
    const int                   m_reserved = 8596;  //�����������ļ�ͷ��С
    vector<string>              m_encryptTag;       //���ܵ�tag
    vector<unsigned short>      m_valueOnlyDGI;     //������value��tag
    map<string, string>         m_exchangeDGI;      //��Ҫ������tag
    string                      m_key;              //������Կ
};

