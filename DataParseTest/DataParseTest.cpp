// DataParseTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../DataParse/IDataParse.h"
#include "../Util/ParseTLV.h"
#include <cstring>
int main()
{


	//Test ParseTLV
	char* tlvBuffer = "6F688408A000000333010101A55C500E556E696F6E5061792044656269748701019F381BDF60019F66049F02069F03069F1A0295055F2A029A039C019F37045F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0E9F4D020B0ADF4D020C0ADF6101421234";
    char* tlvBuffer2 = "70B39381B03C7B01E12BDCAA437640A4DB515AD8B02A7199F3B2DA854008756070D6CAFD88A33CD75982CBFF5D664B2F7B0AF3C3886CCA6DC31599294B70A33770B7736E20C0DF089C85AF3FE7509D41DA93A247CCDB112F496C51AAC102E304E9A3026AFEA6522AA63A25A9E36312F3A74DF3583E0F29E84A6F22F2C352B0195DB175BA221066A43709B0600BB396A364A0A737FD5BA93E5371C67D3C4AB5BB5429DC995D113FE5D6D754B80B6E69773B1530ADAF";
	
	unsigned int tlvCount = 32;
    TLV pTlvs[32];
    for (int i = 0; i < 1000; i++) {
        memset(pTlvs, 0, sizeof(TLV) * 32);
        IsBcdTlvStruct(tlvBuffer, strlen(tlvBuffer));
        ParseTLV(tlvBuffer2, pTlvs, tlvCount);
    }

    //Test ParseTL
    char* tlBuffer = "9F7A019F02065F2A02DF6901";
    TL pTls[32];
    unsigned int count = 32;
    ParseTL(tlBuffer, pTls, count);

	//Test Parse AFL
	AFL pAfls[12] = { 0 };
	char* aflBuffer = "1801020018040701200505003001020040010100";
	unsigned int aflCount = 12;
	ParseAFL(aflBuffer, pAfls, aflCount);

    getchar();
    return 0;
}

