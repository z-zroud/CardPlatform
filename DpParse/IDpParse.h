#pragma once

#include <vector>
#include <string>
#include <fstream>
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
    void ReplaceItem(string key, string value);
    void ReplaceKey(string oldKey, string newKey);
    void DeleteItem(string key);
	vector<pair<string, string>> GetItems() { return m_vecItems; }
    string GetItem(string tag);
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

bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);



/************************************************************
//����DGIת���Ļ�������DP��������øýӿ�ʵ��DGI��ת��
*************************************************************/

struct DGIEncrypt
{
    string dgi;     
    string type;
    string key;
};

struct DGIMap
{
    string srcDgi;
    string dstDgi;
};

struct DGIExchange
{
    string srcDgi;
    string exchangedDgi;
};

struct AddTagFromDGI
{
    string srcDgi;
    string dstDgi;
    string dstTag;
};

struct AddFixedTagValue
{
    string srcDgi;
    string tag;
    string tagValue;
};

struct DGIDeleteTag
{
    string srcDgi;
    string tag;
};




class IRule
{
public:
	virtual bool SetRuleCfg(const char* szRuleConfig);
	virtual void HandleRule(CPS_ITEM& cpsItem);
protected:
	void HandleDGIMap(CPS_ITEM& cpsItem);           //����DGIӳ���ϵ
	void HandleDGIExchange(CPS_ITEM& cpsItem);      //����DGI������ϵ
	void HandleDGIDecrypt(CPS_ITEM& cpsItem);       //����DGI����
    void HandleDGIDelete(CPS_ITEM& cpsItem);        //����ɾ��DGI
    void HandleDGIAddTag(CPS_ITEM& cpsItem);        //����DGI���Tag
    void HandleDGIAddFixedTag(CPS_ITEM& cpsItem);   //����DGI��ӹ̶�ֵ��Tag
	void HandleTagDelete(CPS_ITEM& cpsItem);        //����ɾ��DGIĳ��tag

    /***************************************************************
    * ��DP�������ݽ��н���
    * ������tk ���ݴ�����Կ
    * ������encryptData ����
    * ���� ����
    *****************************************************************/
    string DesDecryptDGI(string tk, string encryptData);
    string SMDecryptDGI(string tk, string encryptData1);

private:


	vector<DGIMap>              m_vecDGIMap;
	vector<string>              m_vecDGIDelete;
	vector<DGIExchange>         m_vecDGIExchange;
	vector<DGIEncrypt>          m_vecDGIEncrypt;
	vector<AddTagFromDGI>       m_vecAddTagFromDGI;
	vector<AddFixedTagValue>    m_vecFixedTagAdd;
    vector<DGIDeleteTag>        m_vecTagDelete;
};

/******************************************************************
* DP���ݽ����ӿڣ�DP����Ҫʵ��HandleDp�ӿ�
*******************************************************************/
struct IDpParse
{
	/**************************************************************
	* ÿһ���̳�IDpParse���඼��Ҫʵ����ν���DP�ļ��ӿ�
	***************************************************************/
	virtual bool HandleDp(const char* szFileName, const char* ruleFile) = 0;
	
	/**************************************************************
	* ��ȡ�ļ���С
	***************************************************************/
	virtual long GetFileSize(ifstream& dpFile);

	/**************************************************************
	* ���ļ�
	***************************************************************/
	virtual bool OpenDpFile(const char* fileName, ifstream& dpFile);

	/**************************************************************
	* ��ȡDP�ļ��У��ӵ�ǰ��ָ��λ�õ�bufferLen�������ݣ�������ָ��λ��
	***************************************************************/
	virtual streampos GetBuffer(ifstream &dpFile, char* buffer, int bufferLen);
	virtual streampos GetBCDBuffer(ifstream &dpFile, string& buffer, int len);
    virtual streampos GetBCDBufferLittleEnd(ifstream &dpFile, string &buffer, int len);

	/***************************************************************
	* ��ȡ��ָ�������ݳ��ȵ�bufferת������������
	* ������dpFile DP�ļ����
	* ������dataLen ָ�����ȵ�bufferת���ɵ�����ֵ
	* ������len ָ��buffer�ĳ���
	****************************************************************/
	virtual streampos GetLenTypeBuffer(ifstream &dpFile, int &dataLen, int len);
    virtual streampos GetLenTypeBufferLittleEnd(ifstream &dpFile, int &dataLen, int len);

	/***************************************************************
	* ����DP�ļ�����CPS����
	****************************************************************/
	virtual void HandleRule(const char* szRuleConfig, CPS_ITEM& cpsItem);

    /***************************************************************
    * ��cpsItem����ȡaccount��Ϊ�ļ�����
    ****************************************************************/
    virtual string GetAccount(CPS_ITEM& cpsItem);

	/***************************************************************
	* ����CPS���ݵ������ļ������ļ�Ĭ��ΪDP���ݵ��¼�Ŀ¼�ļ�
	****************************************************************/
	virtual void Save(CPS_ITEM cpsItem);



    /****************************************************************
    * ����TLV���ݽṹ
    * buffer�в�Ӧ�ð���70ģ��
    *****************************************************************/
    void ParseTLV(char* buffer, unsigned int bufferLen, Dict& tlvs, bool littleEnd = true);

	/****************************************************************
	* �ж�ĳ�������Ƿ�ΪTLV�ṹ
	*****************************************************************/
	bool IsTlvStruct(char* buffer, unsigned int bufferLength, bool littleEnd = true);
};

