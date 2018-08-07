#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSEGOLDPAC_EXPORTS
#define GOLDPAC_API __declspec(dllexport)
#else
#define GOLDPAC_API __declspec(dllimport)
#endif


extern "C" GOLDPAC_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

class GoldpacDpParser : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count);
};
