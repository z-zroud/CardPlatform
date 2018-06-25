#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;

#ifdef DPPARSEGEER_EXPORTS
#define DPPARSEGEER_API __declspec(dllexport)
#else
#define DPPARSEGEER_API __declspec(dllimport)
#endif


extern "C" DPPARSEGEER_API bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count);

class GEERDpParser : public IDpParse
{
public:
    bool HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count);
};