#pragma once
#include <string>
#include <map>
#include <vector>
#include "Util\IniParaser.h"
#include "Util\Converter.h"
#include "Util\TLVParaser.h"
#include "Interface\IDPParser.h"
using namespace std;



/***************************************************************
* 构造TLV元素
****************************************************************/
struct TLVItem
{
	TLVItem(string tag, string len, string value, bool isNeedConvert = false)
	{
		if (isNeedConvert)
		{			
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

////////////////用于解析DP文件格式//////////////////////////
//DGI 分组数据,每个DGI包含多个TLV结构
typedef struct tagDGI
{
	void Clear() { vecItem.clear(); }
	string DGIName;
	vector<TLVItem> vecItem;
}DGI;

//每一张卡片的个人化数据结构
struct OneCardPersoData
{
    int nCardSequence;
    vector<DGI> vecDpData;
    string strAccount;	//卡片账号作为输出的文件名称
};
//////////////////////////////////////////////////////////
//////////////////用于保存到CPS文件中//////////////////////
//CPS 个人化分组结构
typedef struct tagDTL
{
	string DGI;
	string Tag;
	string Value;
}DTL;

//每一个CPS文件的数据结构
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

