#include "StdAfx.h"
#include "YLDPConfig.h"
#include "Util\StringParaser.h"

#define ROOT            _T("YINLIAN")
#define ENCRYPT_DATA   _T("ENCRYPT_DATA")
#define DECRYPT_KEY    _T("DECRYPT_KEY")
#define VALUE_DATA      _T("VALUE_DATA")
#define EXCHANGE_DATA   _T("EXCHANGE_DATA")

bool CYLDPConfig::Read(const string &filePath)
{
    if (INI_OK != m_paraser.Read(filePath))
    {
        return false;
    }

    //获取配置信息
    string encryptData  = m_paraser.GetValue(ROOT, ENCRYPT_DATA);
    m_sDecryptKey       = m_paraser.GetValue(ROOT, DECRYPT_KEY);
    string valueData    = m_paraser.GetValue(ROOT, VALUE_DATA);
    string exchangeData = m_paraser.GetValue(ROOT, EXCHANGE_DATA);

    //解析数据
    Tool::Stringparser::SplitString(encryptData, m_vecEncryptData, _T(","));
    Tool::Stringparser::SplitString(valueData, m_vecValueData, _T(","));

    vector<string> vecExchangeData;
    Tool::Stringparser::SplitString(exchangeData, vecExchangeData, _T("("), _T(")"));

    return true;
}

string CYLDPConfig::GetEncryptData()
{
    return m_paraser.GetValue(ROOT, ENCRYPT_DATA);
}

string CYLDPConfig::GetValueData()
{
    return m_paraser.GetValue(ROOT, VALUE_DATA);
}

string CYLDPConfig::GetExchangeData()
{
    return m_paraser.GetValue(ROOT, EXCHANGE_DATA);
}

string CYLDPConfig::GetDecryptKey()
{
    return m_sDecryptKey;
}