#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSESZSM_EXPORTS
#define DPPARSESZSM_API __declspec(dllexport)
#else
#define DPPARSESZSM_API __declspec(dllimport)
#endif


extern "C" DPPARSESZSM_API bool HandleDp(const char* szFileName, const char* ruleFile);

class SZSMDpParser : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile);

    string GetAccount2(string magstripData);
protected:
    void HandleData(const string data, Dict& dgiItem);
};