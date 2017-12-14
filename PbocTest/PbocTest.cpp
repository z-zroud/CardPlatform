// PbocTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include "..\PCSC\PCSC.h"
#include "..\ApduCmd\IApdu.h"
#include "..\DataParse\IDataParse.h"

using namespace std;

#define SW_9000 0x9000

vector<pair<string, string>> g_Tags;

/****************************************
* ���濨Ƭ�е�Tagֵ��������ʹ��
*****************************************/
void SetTags(string tag, string value)
{
	bool bExist = false;
	for (auto& item : g_Tags)
	{
		if (item.first == tag) {
			item.second = value;
			bExist = true;
			break;
		}
	}
	if (!bExist) {
		g_Tags.push_back(pair<string, string>(tag, value));
	}
}

/*********************************************
* �ӱ����tag�����л�ȡָ����tagֵ
**********************************************/
string GetTag(string tag)
{
	for (auto item : g_Tags)
	{
		if (item.first == tag)
			return item.second;
	}
	return "";
}

int main()
{
	/********************** �򿪶����� ********************************/
	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", readers[i]);
	}
	if (!OpenReader(readers[0]))
	{
		return 1;
	}

	char atr[128] = { 0 };
	GetATR(atr, sizeof(atr));
	printf("%s\n", atr);

	/************************* ѡ��Ӧ�� ****************************/
	int sw = 0;
	char selectAppResp[256] = { 0 };
	sw = SelectAppCmd("315041592E5359532E4444463031", selectAppResp);
	if (sw != SW_9000)
	{
		printf("ѡ��Ӧ��ʧ��!\n");
		return 2;
	}
	unsigned int tlvCount = 12;
	TLV tlvs[12];
	ParseTLV(selectAppResp, tlvs, tlvCount);
    return 0;
}

