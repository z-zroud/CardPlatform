#pragma once
#include "../DPParaser.h"



class YLDpParser : public DPParser
{
public:
	YLDpParser();
	int Read(const string& filePath);
	void Save(const string &strPath = "YLDP");

    void SetOnlyValueOfDGI(vector<unsigned short> vecValueDGI);    
    void SetEncryptTag(vector<string> vecEncryptDGI);
    void SetDecryptKey(const string &key);
    void SetExchangeDGI(map<string, string> mapDGI);

private:
	bool			IsValueOnlyDGI(unsigned short sDGINO);
	bool			IsEncryptDGI(string tag);
	void			ReadDGIName(ifstream &dpFile, streamoff offset);						//读取DGI名称
	void			DealPSEDataCompleted(ifstream &dpFile, streamoff offset);			//处理PSE数据
	char			ReadDGIStartTag(ifstream &dpFile, streamoff offset);					//读取起始标签
	unsigned short	ReadFollowedDataLength(ifstream &dpFile, streamoff offset);			//读取数据长度
	int				ReadCardSequence(ifstream &dpFile, streamoff offset);				//读取卡片序列号
	int				ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset);	//读取卡片个人化数据总长度
	unsigned short	ReadDGISequence(ifstream &dpFile, streamoff offset);					//读取卡片DGI分组序号
	char			ReadRecordTemplate(ifstream &dpFile, streamoff offset);				//读取记录模板
	void			ParseTLV(char* buffer, int nBufferLen, DGI &YLDGI);		//解析TLV结构
	bool			CheckFolderExist(const string &strPath);
	void			FilterDpData();
	string			DecrptData(string tag, string value);
	void			ClearCurrentDPData();
    string          GetTagValue(string Tag, OneCardPersoData &oneCardDpData);
    void            SetTagValue(string tag, string value, OneCardPersoData &oneCardDpData);

private:
	
	
	vector<string>              m_dGIName;	
    OneCardPersoData            m_oneCardDpData;
	vector<OneCardPersoData>    m_manyCardDpData;	//银联一个数据文件包含多张卡片数据
	vector<CPSDP>               m_vecCPSDP;	//CPS DP数据结构
    const int                   m_reserved = 8596;  //银联保留的文件头大小
    vector<string>              m_encryptTag;       //加密的tag
    vector<unsigned short>      m_valueOnlyDGI;     //仅还有value的tag
    map<string, string>         m_exchangeDGI;      //需要交换的tag
    string                      m_key;              //解密密钥
};

