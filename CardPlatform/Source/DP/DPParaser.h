#pragma once
#include <string>
#include <map>
#include <vector>
#include "Util\IniParaser.h"
#include "Util\Tool.h"
#include "Util\TLVParaser.h"
#include "Interface\IDPParser.h"
using namespace std;



/***************************************************************
* ����TLVԪ��
****************************************************************/
struct TLVItem
{
	TLVItem(string tag, string len, string value, bool isNeedConvert = false)
	{
		if (isNeedConvert)
		{			
			strTag = Tool::StrToHex(tag.c_str(),tag.size());
			strLen = Tool::StrToHex(len.c_str(),len.size());
			const char* pLen = strLen.c_str();
			int nLen = Tool::HexStrToInt(strLen.c_str(), strLen.size());
			strValue = Tool::StrToHex(value.c_str(), nLen);
		}
		else {
			strTag = tag;
			strLen = len;
			strValue = value;
		}
	}
	string	strTag;
	string  strLen;
	string	strValue;
};

////////////////���ڽ���DP�ļ���ʽ//////////////////////////
//DGI ��������,ÿ��DGI�������TLV�ṹ
typedef struct tagDGI
{
	void Clear() { vecItem.clear(); }
	string DGIName;
	vector<TLVItem> vecItem;
}DGI;

//ÿһ�ſ�Ƭ�ĸ��˻����ݽṹ
struct OneCardPersoData
{
    int nCardSequence;
    vector<DGI> vecDpData;
    string strAccount;	//��Ƭ�˺���Ϊ������ļ�����
};
//////////////////////////////////////////////////////////
//////////////////���ڱ��浽CPS�ļ���//////////////////////
//CPS ���˻�����ṹ
typedef struct tagDTL
{
	string DGI;
	string Tag;
	string Value;
}DTL;

//ÿһ��CPS�ļ������ݽṹ
typedef struct tagCPSDP
{
	string fileName;
	vector<DTL> data;
}CPSDP;
/////////////////////////////////////////////////////////

class DPParser : public IDPParser
{
public:
	virtual int Read(const string& filePath);
	virtual void Save(const string& filePath);
	virtual IniParser GetConfig(const string& filePath);

protected:
	vector<DGI> m_DpData;
};

