// PCSCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PCSC/PCSC.h"
#include <Windows.h>
#ifdef _DEBUG
#pragma comment(lib,"../Debug/PCSC.lib")
#else
#pragma comment(lib,"../Release/PCSC.lib")
#endif

#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_utils.hpp"
#include "rapidxml\rapidxml_print.hpp"

#include <string>
#include <iostream>
using namespace std;
using namespace rapidxml;

int main()
{
	//int count = 0;
	//char* readers[2] = { 0 };
	//GetReaders(readers, count);
	//for (int i = 0; i < count; i++)
	//{
	//	printf("%s\n", readers[i]);
	//}
	//OpenReader(readers[0]);
	//WarmReset();
	//ColdReset();

	//char atr[33] = { 0 };
	//GetATR(atr, sizeof(atr));
	//printf("%s\n", atr);

	//GetCardStatus();
	//GetTransProtocol();

	//char output[256] = { 0 };
	//SendApdu("00A40400 08 A000000003000000", output, sizeof(output));
	//printf("%s\n", output);

	file<> fDoc("F:\\CardPlatform\\Debug\\test.xml");
	xml_document<> doc;
	doc.parse<0>(fDoc.data());

	string text;
	rapidxml::print(back_inserter(text), doc, 0);
	cout << text << endl;

	xml_node<>* root = doc.first_node();
	
	xml_node<>* firstChildNode = root->first_node();
	auto pNode = firstChildNode;
	while (pNode)
	{
		cout << "node name: " << pNode->name() << endl;
		for (auto attr = pNode->first_attribute(); attr != NULL; attr = attr->next_attribute())
		{
			cout << attr->name() << "=" << attr->value() << endl;
		}
		pNode = pNode->next_sibling();
	}
    return 0;
}

