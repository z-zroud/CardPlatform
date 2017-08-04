#pragma once
#include <string>
#include "Interface\CardBase.h"
using namespace std;

/*****************************************************************
* CModifyKmcÓÃÓÚÐÞ¸ÄKMC
******************************************************************/
class CModifyKmc
{
public:
    CModifyKmc(string readerName);
    bool StartModifyKMC(const string &aid, const string &oldKmc, const string &newKmc, DIV_METHOD_FLAG flag);

private:
    string m_readerName;
};