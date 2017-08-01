#include "stdafx.h"
#include "Interface\CardBase.h"

string CardStatusToString(CARD_STATUS status)
{
	switch (status)
	{
	case UNKNOWN:		return _T("UNKNOWN");
	case ABSENT:		return _T("ABSENT");
	case PRESENT:		return _T("PRESENT");
	case SWALLOWED:		return _T("SWALLOWED");
	case POWERED:		return _T("POWERED");
	case NEGOTIABLE:	return _T("NEGOTIABLE");
	case SPECIFIC:		return _T("SPECIFIC");
	}

	return _T("Error");
}

string CTPToString(CARD_TRANSMISSION_PROTOCOL CTP)
{
	switch (CTP)
	{
	case UNDEFINED:		return _T("UNDEFINED");
	case T0:			return _T("T0");
	case T1:			return _T("T1");
	case RAW:			return _T("RAW");
	}

	return _T("Error");
}

string DivToString(DIV_METHOD_FLAG flag)
{
	switch (flag)
	{
	case NO_DIV:		return _T("NO DIV");
	case DIV_CPG202:	return _T("DIV CPG202");
	case DIV_CPG212:	return _T("DIV CPG212");
	}

	return _T("Error");
}

string SecureLevelToString(SECURE_LEVEL level)
{
	switch (level)
	{
	case SL_NO_SECURE:	return _T("NO SECURE");
	case SL_MAC:		return _T("SECURE MAC");
	case SL_MAC_ENC:	return _T("SECURE MAC ENC");
	}

	return _T("Error");
}
