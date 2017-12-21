#pragma once
#include "../DpParse/IDpParse.h"

#ifdef DPPARSE_DXT_EXPORTS
#define DPPARSE_DXT_API __declspec(dllexport)
#else
#define DPPARSE_DXT_API __declspec(dllimport)
#endif


class DXTDpParse : public IDpParse
{
public:
	virtual bool HandleDp(const char* szFileName);
};

extern "C" DPPARSE_DXT_API bool HandleDp(const char* szFileName);