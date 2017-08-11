#pragma once
#include <vector>
#include <map>
#include <string>
#include "Util\IniParaser.h"
using namespace std;

class CYLDPConfig
{
public:

    bool Read(const string &filePath);
    string GetEncryptData();
    string GetValueData();
    string GetExchangeData();
    string GetDecryptKey();
private:
    vector<string>      m_vecEncryptData;
    vector<string>      m_vecValueData;
    map<string,string>  m_mapExchangeData;
    string              m_sDecryptKey;

    INIParser           m_paraser;
};