#include "stdafx.h"
#include "GenKey.h"

GENKEY_API IGenKey* GetGenKeyInstance()
{
	return new KeyGenerator;
}

GENKEY_API void DeleteGenKeyInstance(IGenKey* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}		
}