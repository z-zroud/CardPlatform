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
    string GetItem();
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


/**************************************************************
* TLV������ʽ(����DP�����ļ��Ľ���)
***************************************************************/
typedef struct TLVEntity
{
	unsigned char* tag;			//��ǩ
	unsigned char* length;		//����
	unsigned char* value;		//����
	unsigned int tagSize;		//��ǩռ���ֽ�  һ�������ֽ�
	unsigned int lengthSize;	//����ռ�õ��ֽ�
	TLVEntity* subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
	unsigned int subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
}*PTLVEntity;




int ctoi(unsigned char c);
bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);



/************************************************************
//����DGIת���Ļ�������DP��������øýӿ�ʵ��DGI��ת��
*************************************************************/
class IRule
{
public:
	virtual void SetRule(const string& ruleConfig);
	virtual void HandleRule(CPS_ITEM& cpsItem);
protected:
	void HandleDGIMap(CPS_ITEM& cpsItem);           //����DGIӳ���ϵ
	void HandleDGIExchange(CPS_ITEM& cpsItem);      //����DGI������ϵ
	void HandleDGIEncrypt(CPS_ITEM& cpsItem);       //����DGI����
	void HandleDGIDelete(CPS_ITEM& cpsItem);        //����ɾ��DGI
	void HandleTagDelete(CPS_ITEM& cpsItem);        //����ɾ��DGIĳ��tag
private:

    string  m_tk;
    vector<string>                  m_vecDGIpadding80;
	vector<pair<string, string>>    m_vecDGIMap;       // src --> dst
	vector<string>                  m_vecDGIDelete;
	vector<pair<string, string>>    m_vecDGIExchange;  // DGI1 <--> DGI2
	vector<string>                  m_vecDGIEncrypt;
	vector<pair<string, string>>    m_vecTagDelete;    // DGI --> tag
	vector<pair<string, string>>    m_vecTagAdd;       // DGI --> tag
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
	virtual void HandleRule(string ruleConfig, CPS_ITEM& cpsItem);

	/***************************************************************
	* ����CPS���ݵ������ļ������ļ�Ĭ��ΪDP���ݵ��¼�Ŀ¼�ļ�
	****************************************************************/
	virtual void Save(CPS_ITEM cpsItem);

	/***************************************************************
	* ��DP�������ݽ��н���
	* ������tk ���ݴ�����Կ
	* ������encryptData ����
	* ���� ����
	*****************************************************************/
	virtual string DecryptDGI(string tk, string encryptData, bool padding80);

	/****************************************************************
	* ����TLV���ݽṹ
	*****************************************************************/
	void ParseTLV(unsigned char* buffer, unsigned int bufferLength, PTLVEntity PTlvEntity, unsigned int& entityNum);

	/****************************************************************
	* �ж�ĳ�������Ƿ�ΪTLV�ṹ
	*****************************************************************/
	bool IsTlvStruct(unsigned char* buffer, unsigned int bufferLength);
};

