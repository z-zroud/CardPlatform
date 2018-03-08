// PCSCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PCSC/PCSC.h"
#include "../CPS/ICPS.h"
#include <Windows.h>

#pragma comment(lib,"../bin/PCSC.lib")
#pragma comment(lib,"../bin/CPS.lib")


#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_utils.hpp"
#include "rapidxml\rapidxml_print.hpp"

#include <string>
#include <iostream>
using namespace std;
using namespace rapidxml;

int main()
{
    char reader[1024] = { 0 };
    int size = 1024;
    GetReadersEx(reader, size);
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

	char output[1024] = { 0 };
	//int sw = SendApdu("00A40400 08 A000000003000000", output, sizeof(output));
	int sw = SendApdu("00A40000 02 3F00", output, sizeof(output));
	printf("sw: %04X\n%s\n", sw, output);

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

