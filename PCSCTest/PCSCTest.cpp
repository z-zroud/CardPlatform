// PCSCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PCSC/PCSC.h"
#include "../CPS/ICPS.h"
#include "../ApduCmd/IApdu.h"
#include "../DataParse/IDataParse.h"
#include <Windows.h>




#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_utils.hpp"
#include "rapidxml\rapidxml_print.hpp"

#include <string>
#include <iostream>
using namespace std;
using namespace rapidxml;

int main()
{
	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", readers[i]);
	}
	OpenReader(readers[1]);
	WarmReset();
	ColdReset();

	char atr[128] = { 0 };
	GetATR(atr, sizeof(atr));
	printf("%s\n", atr);

	GetCardStatus();
	GetTransProtocol();

	char resp[1024] = { 0 };

    int sw = SelectAppCmd("A000000333010101", resp);
    if (sw == 0x9000)
    {
        memset(resp, 0, 1024);
        sw = ReadRecordCmd(2, 1, resp);
        if (sw == 0x9000)
        {
            TLV tlvs[20] = { 0 };
            unsigned int count = 20;
            if (ParseTLV(resp, tlvs, count))
            {
                for (int i = 0; i < count; i++)
                {
                    if (tlvs[i].tag == "5A")
                    {

                    }
                }
            }
        }
    }

	//file<> fDoc("F:\\CardPlatform\\Debug\\test.xml");
	//xml_document<> doc;
	//doc.parse<0>(fDoc.data());

	//string text;
	//rapidxml::print(back_inserter(text), doc, 0);
	//cout << text << endl;

	//xml_node<>* root = doc.first_node();
	//
	//xml_node<>* firstChildNode = root->first_node();
	//auto pNode = firstChildNode;
	//while (pNode)
	//{
	//	cout << "node name: " << pNode->name() << endl;
	//	for (auto attr = pNode->first_attribute(); attr != NULL; attr = attr->next_attribute())
	//	{
	//		cout << attr->name() << "=" << attr->value() << endl;
	//	}
	//	pNode = pNode->next_sibling();
	//}

	GenCpsFile("YLDpParse.dll", "HandleDp", "F:\\CardPlatform\\Debug\\yinlian.DP",NULL);
    return 0;
}

