#pragma once

#include <vector>
#include <string>
#include <fstream>
using namespace std;

/****************************************************************
* 由于涉及到插入元素的顺序关系，不能用标准的字典容器。此Dict主要用于
* DP分组中，存储每个DGI分组的tag=value集合，并进行操作。
*****************************************************************/
class Dict
{
public:
    /***************************************************************
    * 向字典容器中添加元素
    ****************************************************************/
	void AddItem(string key, string value){ m_vecItems.push_back(pair<string, string>(key, value)); }

    /***************************************************************
    *替换key所指定的值value
    ****************************************************************/
    void ReplaceValue(string existedKey, string value);

    /***************************************************************
    *更新key
    ****************************************************************/
    void ReplaceKey(string oldKey, string newKey);

    /***************************************************************
    * 删除key指定的元素
    ****************************************************************/
    void DeleteItem(string key);

    /***************************************************************
    *判断key是否存在
    ****************************************************************/
    bool IsExisted(string key);

    /***************************************************************
    * 清空字典中的元素
    ****************************************************************/
    void Clear();

    /***************************************************************
    *获取所有元素
    ****************************************************************/
	vector<pair<string, string>> GetItems() { return m_vecItems; }

    /***************************************************************
    *获取key指定的元素
    ****************************************************************/
    string GetItem(string key);

    /***************************************************************
    *若集合中只包含一条记录，则无需知道具体的key,可直接获取该key下的值
    *特殊情况下，我们没办法知道key的前提下，请使用这个方法
    ****************************************************************/
    string GetItem();

private:
	vector<pair<string, string>> m_vecItems;
};

/***************************************************************
* 定义每个DGI分组格式
***************************************************************/
struct DGI
{
	string dgi;
	Dict value;
};

/**************************************************************
* 定义标准CPS格式,任何需要生成CPS标准文件的DP数据都需要转换为此
* 结构体形式
***************************************************************/
struct CPS
{
	string fileName;    //CPS文件名称
    string aid;         //AID 借记 or 贷记
	vector<DGI> dgis;    //DGI分组集合
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
    int startPos;   //表示解密后截取数据的起始位置
    int len;    //截取数据的长度
    bool isDelete80;
    bool isWholeDecrypted;
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
//定义DGI转换的基本规则，DP解析库调用该接口实现DGI的转换
*************************************************************/
class IRule
{
public:
	virtual bool SetRuleCfg(const char* szRuleConfig);
	virtual void HandleRule(CPS& cps);
protected:
	void HandleDGIMap(CPS& cps);           //处理DGI映射关系
	void HandleDGIExchange(CPS& cps);      //处理DGI交换关系
	void HandleDGIDecrypt(CPS& cps);       //处理DGI解密
    void HandleTagDecrypt(CPS& cps);       //处理TAG解密
    void HandleDGIDelete(CPS& cps);        //处理删除DGI
    void HandleTagMergeTagFromOtherTag(CPS& cps);         //将多个tag合并
    void HandleDGIAddTagFromOtherDGI(CPS& cps);        //处理DGI添加Tag
    void HandleDGIAddFixedTagValue(CPS& cps);   //处理DGI添加固定值的Tag
	void HandleTagDelete(CPS& cps);        //处理删除DGI某个tag
    void HandleTagInsertValue(CPS& cps);   //处理Tag添加值的问题
    void AddKcv(CPS& cps);                 //增加KCV
    void AddTagPrefix(CPS& cps);      //添加tag及长度到key=value 中的value部分
    void SpliteEF02(CPS& cps);             //解析EFO2 神舟数码专用 存储8201,8202...IC卡私钥
    void AddPseAndPPSE(CPS& cps);      //增加PSE和PPSE DGI

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
    vector<TagInsert>           m_vecTagInsert;
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
	virtual void HandleRule(const char* szRuleConfig, CPS& cpsItem);

    /***************************************************************
    * 从cpsItem中提取account作为文件名称
    ****************************************************************/
    virtual string GetAccount(CPS& cpsItem);
    virtual string GetAccount2(string magstripData);

	/***************************************************************
	* 保存CPS数据到本地文件，该文件默认为DP数据的下级目录文件
	****************************************************************/
	virtual void Save(CPS cpsItem);



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

