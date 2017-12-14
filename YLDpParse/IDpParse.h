#pragma once

/**********************************************************
* �����dll����ӿ�
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
* �����漰������Ԫ�ص�˳���ϵ�������ñ�׼���ֵ�����
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
* ����ÿ��DGI�����ʽ
***************************************************************/
struct DGI_ITEM
{
	string dgi;
	Dict value;
};

/**************************************************************
* �����׼CPS��ʽ
***************************************************************/
struct CPS_ITEM
{
	string fileName;
	vector<DGI_ITEM> items;
};

typedef struct TLVEntity
{
	unsigned char* Tag;			//��ǩ
	unsigned char* Length;		//����
	unsigned char* Value;		//����
	unsigned int TagSize;		//��ǩռ���ֽ�  һ�������ֽ�
	unsigned int LengthSize;	//����ռ�õ��ֽ�
	TLVEntity* subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
	unsigned int subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
}TLV, *PTLV;




int ctoi(unsigned char c);
bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);


//����DGIת���Ļ�����������Ҫ����չ�ýӿ�
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