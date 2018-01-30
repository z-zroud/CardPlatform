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
	vector<pair<string, string>> GetItems() { return m_vecItems; }
    string GetItem(string tag);
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

bool  CheckFolderExist(const string &strPath);
string StrToHex(const char* strBin, int len, bool bIsUpper = true);
string DeleteSpace(string s);



/************************************************************
//定义DGI转换的基本规则，DP解析库调用该接口实现DGI的转换
*************************************************************/
class IRule
{
public:
	virtual void SetRule(const string& ruleConfig);
	virtual void HandleRule(CPS_ITEM& cpsItem);
protected:
	void HandleDGIMap(CPS_ITEM& cpsItem);           //处理DGI映射关系
	void HandleDGIExchange(CPS_ITEM& cpsItem);      //处理DGI交换关系
	void HandleDGIEncrypt(CPS_ITEM& cpsItem);       //处理DGI加密
	void HandleDGIDelete(CPS_ITEM& cpsItem);        //处理删除DGI
	void HandleTagDelete(CPS_ITEM& cpsItem);        //处理删除DGI某个tag
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
* DP数据解析接口，DP库需要实现HandleDp接口
*******************************************************************/
struct IDpParse
{
	/**************************************************************
	* 每一个继承IDpParse的类都需要实现如何解析DP文件接口
	***************************************************************/
	virtual bool HandleDp(const char* szFileName, const char* ruleFile) = 0;
	
	/**************************************************************
	* 获取文件大小
	***************************************************************/
	virtual long GetFileSize(ifstream& dpFile);

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
	virtual void HandleRule(string ruleConfig, CPS_ITEM& cpsItem);

    /***************************************************************
    * 从cpsItem中提取account作为文件名称
    ****************************************************************/
    virtual string GetAccount(CPS_ITEM& cpsItem);

	/***************************************************************
	* 保存CPS数据到本地文件，该文件默认为DP数据的下级目录文件
	****************************************************************/
	virtual void Save(CPS_ITEM cpsItem);

	/***************************************************************
	* 对DP分组数据进行解密
	* 参数：tk 数据传输密钥
	* 参数：encryptData 密文
	* 返回 明文
	*****************************************************************/
	virtual string DecryptDGI(string tk, string encryptData);

    virtual string SMDecryptDGI(string tk, string encryptData1);

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

