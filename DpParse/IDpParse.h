#pragma once

#include <vector>
#include <string>
#include <fstream>
using namespace std;

/****************************************************************
* �����漰������Ԫ�ص�˳���ϵ�������ñ�׼���ֵ���������Dict��Ҫ����
* DP�����У��洢ÿ��DGI�����tag=value���ϣ������в�����
*****************************************************************/
class Dict
{
public:
    /***************************************************************
    * ���ֵ����������Ԫ��
    ****************************************************************/
	void AddItem(string key, string value){ m_vecItems.push_back(pair<string, string>(key, value)); }

    /***************************************************************
    *�滻key��ָ����ֵvalue
    ****************************************************************/
    void ReplaceValue(string existedKey, string value);

    /***************************************************************
    *����key
    ****************************************************************/
    void ReplaceKey(string oldKey, string newKey);

    /***************************************************************
    * ɾ��keyָ����Ԫ��
    ****************************************************************/
    void DeleteItem(string key);

    /***************************************************************
    *�ж�key�Ƿ����
    ****************************************************************/
    bool IsExisted(string key);

    /***************************************************************
    * ����ֵ��е�Ԫ��
    ****************************************************************/
    void Clear();

    /***************************************************************
    *��ȡ����Ԫ��
    ****************************************************************/
	vector<pair<string, string>> GetItems() { return m_vecItems; }

    /***************************************************************
    *��ȡkeyָ����Ԫ��
    ****************************************************************/
    string GetItem(string key);

    /***************************************************************
    *��������ֻ����һ����¼��������֪�������key,��ֱ�ӻ�ȡ��key�µ�ֵ
    *��������£�����û�취֪��key��ǰ���£���ʹ���������
    ****************************************************************/
    string GetItem();

private:
	vector<pair<string, string>> m_vecItems;
};

/***************************************************************
* ����ÿ��DGI�����ʽ
***************************************************************/
struct DGI
{
	string dgi;
	Dict value;
};

/**************************************************************
* �����׼CPS��ʽ,�κ���Ҫ����CPS��׼�ļ���DP���ݶ���Ҫת��Ϊ��
* �ṹ����ʽ
***************************************************************/
struct CPS
{
	string fileName;    //CPS�ļ�����
    string aid;         //AID ��� or ����
	vector<DGI> dgis;    //DGI���鼯��
    void AddDGI(DGI item);
};

bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);





struct DGIEncrypt
{
    string dgi;     
    string type;
    string key;
    bool isDelete80;
};

struct TagEncrypt
{
    string tag;
    string type;
    string key;
    int startPos;   //��ʾ���ܺ��ȡ���ݵ���ʼλ��
    int len;    //��ȡ���ݵĳ���
    bool isDelete80;
    bool isWholeDecrypted;
};

struct TagMerge     //��old tag�ϲ��� new tag��
{
    string srcDgi;
    string srcTag;
    string dstDgi;
    string dstTag;
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
    string srcTag;
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

struct TagInsert
{
    string dgi;
    string tag;
    string insertedValue;
    int pos;
};

struct AddKCV
{
    string srcDgi;
    string dstDgi;
    string keyType;
};

/************************************************************
//����DGIת���Ļ�������DP��������øýӿ�ʵ��DGI��ת��
*************************************************************/
class IRule
{
public:
	virtual bool SetRuleCfg(const char* szRuleConfig);
	virtual void HandleRule(CPS& cps);
protected:
	void HandleDGIMap(CPS& cps);           //����DGIӳ���ϵ
	void HandleDGIExchange(CPS& cps);      //����DGI������ϵ
	void HandleDGIDecrypt(CPS& cps);       //����DGI����
    void HandleTagDecrypt(CPS& cps);       //����TAG����
    void HandleDGIDelete(CPS& cps);        //����ɾ��DGI
    void HandleTagMergeTagFromOtherTag(CPS& cps);         //�����tag�ϲ�
    void HandleDGIAddTagFromOtherDGI(CPS& cps);        //����DGI���Tag
    void HandleDGIAddFixedTagValue(CPS& cps);   //����DGI��ӹ̶�ֵ��Tag
	void HandleTagDelete(CPS& cps);        //����ɾ��DGIĳ��tag
    void HandleTagInsertValue(CPS& cps);   //����Tag���ֵ������
    void AddKcv(CPS& cps);                 //����KCV
    void AddTagPrefix(CPS& cps);      //���tag�����ȵ�key=value �е�value����
    void SpliteEF02(CPS& cps);             //����EFO2 ��������ר�� �洢8201,8202...IC��˽Կ
    void AddPseAndPPSE(CPS& cps);      //����PSE��PPSE DGI

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
    vector<TagEncrypt>          m_vecTagEncrypt;
	vector<AddTagFromDGI>       m_vecAddTagFromDGI;
	vector<AddFixedTagValue>    m_vecFixedTagAdd;
    vector<DGIDeleteTag>        m_vecTagDelete;
    vector<TagMerge>            m_vecTagMerge;
    vector<TagInsert>           m_vecTagInsert;
    vector<AddKCV>              m_vecAddKcv;
    vector<string>              m_vecAddTagAndTemplate;
    bool                        m_hasEF02;
    bool                        m_addPse;
    string                      m_aid;
};

/******************************************************************
* DP���ݽ����ӿڣ�DP����Ҫʵ��HandleDp�ӿ�
*******************************************************************/
struct IDpParse
{
	/**************************************************************
	* ÿһ���̳�IDpParse���඼��Ҫʵ����ν���DP�ļ��ӿ�
	***************************************************************/
	virtual bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count) = 0;
	
	/**************************************************************
	* ��ȡ�ļ���С
	***************************************************************/
	virtual long GetFileSize(ifstream& dpFile);

    virtual string GetLine(ifstream& dpFile);

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
	virtual void HandleRule(const char* szRuleConfig, CPS& cpsItem);

    /***************************************************************
    * ��cpsItem����ȡaccount��Ϊ�ļ�����
    ****************************************************************/
    virtual string GetAccount(CPS& cpsItem);
    virtual string GetAccount2(string magstripData);

	/***************************************************************
	* ����CPS���ݵ������ļ������ļ�Ĭ��ΪDP���ݵ��¼�Ŀ¼�ļ�
	****************************************************************/
	virtual void Save(CPS cpsItem);



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

