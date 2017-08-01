#pragma once
#include <string>
#include <map>
#include <vector>
#include "Util\IniParaser.h"
#include "Util\Converter.h"
#include "Util\TLVParaser.h"
#include "Interface\IDPParser.h"
using namespace std;



//单个TLV元素
struct TLVItem
{
	//TLVItem(string tag, int len, string value)
	//{
	//	strTag = Tool::Converter::StrToHex(tag);
	//	nLen = len;
	//	strValue = Tool::Converter::StrToHex(value);		
	//}

	TLVItem(string tag, string len, string value, bool isNeedConvert = false)
	{
		if (isNeedConvert)
		{
			//int *pLen = &len.c_;//stoi(len, nullptr, 16);
			//const char* pLen = len.c_str();
			
			strTag = Tool::Converter::StrToHex(tag.c_str(),tag.size());
			strLen = Tool::Converter::StrToHex(len.c_str(),len.size());
			const char* pLen = strLen.c_str();
			int nLen = Tool::Converter::HexStrToInt(strLen.c_str(), strLen.size());
			strValue = Tool::Converter::StrToHex(value.c_str(), nLen);
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

//DGI 分组数据
typedef struct tagDGI
{
	void Clear() { vecItem.clear(); }
	string DGIName;
	vector<TLVItem> vecItem;
}DGI;

typedef struct tagDTL
{
	string DGI;
	string Tag;
	string Value;
}DTL;

typedef struct tagDP
{
	string fileName;
	vector<DTL> data;
}DP;

class DPParser : public IDPParser
{
public:
	virtual int Read(const string& filePath);
	virtual void Save(const string& filePath);
	virtual INIParser GetConfig(const string& filePath);

protected:
	vector<DGI> m_DpData;
};

