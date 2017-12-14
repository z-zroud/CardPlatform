#pragma once

/**********************************************************
* 定义该dll对外接口
***********************************************************/
#define DP_PARSE_EXPORTS


#ifdef DP_PARSE_EXPORTS
#define DP_PARSE_API __declspec(dllexport)
#else
#define DP_PARSE_API __declspec(dllimport)
#endif

#include <vector>
#include <string>

using namespace std;

/****************************************************************
* 由于涉及到插入元素的顺序关系，不能用标准的字典容器
*****************************************************************/
class Dict
{
public:
	void InsertItem(string key, string value)
	{
		m_vecItems.push_back(pair<string, string>(key, value));
	}
	vector<pair<string, string>> GetItems() { return m_vecItems; }

private:
	vector<pair<string, string>> m_vecItems;
};

/***************************************************************
* 定义每个DGI分组格式
***************************************************************/
struct DGI_ITEM
{
	string dgi;
	Dict value;
};

/**************************************************************
* 定义标准CPS格式
***************************************************************/
struct CPS_ITEM
{
	string fileName;
	vector<DGI_ITEM> items;
};

typedef struct TLVEntity
{
	unsigned char* Tag;			//标签
	unsigned char* Length;		//长度
	unsigned char* Value;		//数据
	unsigned int TagSize;		//标签占用字节  一般两个字节
	unsigned int LengthSize;	//数据占用的字节
	TLVEntity* subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
	unsigned int subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
}TLV, *PTLV;




int ctoi(unsigned char c);
bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);


//定义DGI转换的基本规则，若需要可扩展该接口
class IRule
{
public:
	virtual void ParseRule(const string& ruleConfig);
	virtual void HandleRule(CPS_ITEM& cpsItem);
protected:
	void HandleDGIMap();
	void HandleDGIExchange();
	void HandleDGIDecrypt();
	void HandleDGIDelete();
	void HandleTagDelete();
private:
	CPS_ITEM m_cpsItem;

	vector<pair<string, string>> m_vecDGIMap;
	vector<string> m_vecDGIDelete;
	vector<pair<string, string>> m_vecDGIExchange;
	vector<string> m_vecDGIDecrypt;
	vector<pair<string, string>> m_vecTagDelete;
	vector<pair<string, string>> m_vecTagAdd;
};


struct IDpParse
{
	virtual bool HandleDp(const char* szFileName) = 0;
	virtual void Save(CPS_ITEM cpsItem);
	virtual void HandleRule(IRule ruleObj, string ruleConfig, CPS_ITEM& cpsItem);

	
	void TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PTLV PTlvEntity, unsigned int& entityNum);

	bool TLVParseAndFindError(PTLV PTlvEntity, unsigned int entitySize, unsigned char* buffer, unsigned int& bufferLength);
	bool IsTlvStruct(unsigned char* buffer, unsigned int bufferLength);
};

extern "C" DP_PARSE_API bool HandleDp(const char* szFileName);
extern "C" DP_PARSE_API void SetRuleConfig(const char* szConfigFile);