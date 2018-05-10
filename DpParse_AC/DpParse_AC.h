#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSEAC_EXPORTS
#define ACDPPARSE_API __declspec(dllexport)
#else
#define ACDPPARSE_API __declspec(dllimport)
#endif


extern "C" ACDPPARSE_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

class ACDpParse : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count);
};

