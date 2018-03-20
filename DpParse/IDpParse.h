#pragma once

#include <vector>
#include <string>
#include <fstream>
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
    void ReplaceItem(string key, string value);
    void ReplaceKey(string oldKey, string newKey);
    void DeleteItem(string key);
    bool TagExisted(string tag);
	vector<pair<string, string>> GetItems() { return m_vecItems; }
    string GetItem(string tag);
    string GetItem();   //返回DGI中唯一的一个tag的值，不需要考虑是那条tag
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
    string aid;
	vector<DGI_ITEM> items;
    void AddDgiItem(DGI_ITEM item);
};

bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);



/************************************************************
//定义DGI转换的基本规则，DP解析库调用该接口实现DGI的转换
*************************************************************/

struct DGIEncrypt
{
    string dgi;     
    string type;
    string key;
};

struct TagEncrypt
{
    string tag;
    string type;
    string key;
    int startPos;   //表示解密后截取数据的起始位置
    int len;    //截取数据的长度
};

struct TagMerge     //将old tag合并到 new tag中
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

struct AddKCV
{
    string srcDgi;
    string dstDgi;
    string keyType;
};


class IRule
{
public:
	virtual bool SetRuleCfg(const char* szRuleConfig);
	virtual void HandleRule(CPS_ITEM& cpsItem);
protected:
	void HandleDGIMap(CPS_ITEM& cpsItem);           //处理DGI映射关系
	void HandleDGIExchange(CPS_ITEM& cpsItem);      //处理DGI交换关系
	void HandleDGIDecrypt(CPS_ITEM& cpsItem);       //处理DGI解密
    void HandleTagDecrypt(CPS_ITEM& cpsItem);       //处理TAG解密
    void HandleDGIDelete(CPS_ITEM& cpsItem);        //处理删除DGI
    void HandleTagsMerge(CPS_ITEM& cpsItem);         //将多个tag合并
    void HandleDGIAddTag(CPS_ITEM& cpsItem);        //处理DGI添加Tag
    void HandleDGIAddFixedTag(CPS_ITEM& cpsItem);   //处理DGI添加固定值的Tag
	void HandleTagDelete(CPS_ITEM& cpsItem);        //处理删除DGI某个tag
    void AddKcv(CPS_ITEM& cpsItem);                 //增加KCV
    void AddTagToValue(CPS_ITEM& cpsItem);      //将tag及template到数据部分
    void SpliteEF02(CPS_ITEM& cpsItem);             //解析EFO2 神舟数码专用 存储8201,8202...IC卡私钥
    void AddPseAndPPSE(CPS_ITEM& cpsItem);      //增加PSE和PPSE DGI

    /***************************************************************
    * 对DP分组数据进行解密
    * 参数：tk 数据传输密钥
    * 参数：encryptData 密文
    * 返回 明文
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
    vector<AddKCV>              m_vecAddKcv;
    vector<string>              m_vecAddTagAndTemplate;
    bool                        m_hasEF02;
    bool                        m_addPse;
    string                      m_aid;
};

/******************************************************************
* DP数据解析接口，DP库需要实现HandleDp接口
*******************************************************************/
struct IDpParse
{
	/**************************************************************
	* 每一个继承IDpParse的类都需要实现如何解析DP文件接口
	***************************************************************/
	virtual bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count) = 0;
	
	/**************************************************************
	* 获取文件大小
	***************************************************************/
	virtual long GetFileSize(ifstream& dpFile);

    virtual string GetLine(ifstream& dpFile);

	/**************************************************************
	* 打开文件
	***************************************************************/
	virtual bool OpenDpFile(const char* fileName, ifstream& dpFile);

	/**************************************************************
	* 获取DP文件中，从当前流指针位置到bufferLen长的数据，返回流指针位置
	***************************************************************/
	virtual streampos GetBuffer(ifstream &dpFile, char* buffer, int bufferLen);
	virtual streampos GetBCDBuffer(ifstream &dpFile, string& buffer, int len);
    virtual streampos GetBCDBufferLittleEnd(ifstream &dpFile, string &buffer, int len);

	/***************************************************************
	* 获取的指定的数据长度的buffer转换成整数类型
	* 参数：dpFile DP文件句柄
	* 参数：dataLen 指定长度的buffer转换成的整数值
	* 参数：len 指定buffer的长度
	****************************************************************/
	virtual streampos GetLenTypeBuffer(ifstream &dpFile, int &dataLen, int len);
    virtual streampos GetLenTypeBufferLittleEnd(ifstream &dpFile, int &dataLen, int len);

	/***************************************************************
	* 处理DP文件生成CPS规则
	****************************************************************/
	virtual void HandleRule(const char* szRuleConfig, CPS_ITEM& cpsItem);

    /***************************************************************
    * 从cpsItem中提取account作为文件名称
    ****************************************************************/
    virtual string GetAccount(CPS_ITEM& cpsItem);
    virtual string GetAccount2(string magstripData);

	/***************************************************************
	* 保存CPS数据到本地文件，该文件默认为DP数据的下级目录文件
	****************************************************************/
	virtual void Save(CPS_ITEM cpsItem);



    /****************************************************************
    * 解析TLV数据结构
    * buffer中不应该包含70模板
    *****************************************************************/
    void ParseTLV(char* buffer, unsigned int bufferLen, Dict& tlvs, bool littleEnd = true);

	/****************************************************************
	* 判断某段数据是否为TLV结构
	*****************************************************************/
	bool IsTlvStruct(char* buffer, unsigned int bufferLength, bool littleEnd = true);
};

