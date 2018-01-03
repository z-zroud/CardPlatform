// DataParseTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../DataParse/IDataParse.h"

int main()
{
	//Test ParseTL
	char* tlBuffer = "9F7A019F02065F2A02DF6901";
	TL pTls[32];
	unsigned int count = 32;
	ParseTL(tlBuffer, pTls, count);

	//Test ParseTLV
	char* tlvBuffer = "6F27840E315041592E5359532E4444463031A5158801015F2D027A68BF0C0A9F4D020B0ADF4D020C0A";
    char* tlvBuffer2 = "6F274354840E315041592E5359532E4444463031A5158801015F2D027A68BF0C0A9F4D020B0ADF4D020C0A";
	TLV pTlvs[32];
	unsigned int tlvCount = 32;
	ParseTLV(tlvBuffer, pTlvs, tlvCount);

	//Test Parse AFL
	AFL pAfls[12] = { 0 };
	char* aflBuffer = "08010100100103011010100018010400";
	unsigned int aflCount = 12;
	ParseAFL(aflBuffer, pAfls, aflCount);
    return 0;
}

