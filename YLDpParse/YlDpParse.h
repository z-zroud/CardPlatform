#pragma once
#include <map>
#include <vector>
#include "IDpParse.h"
using namespace std;



class YLDpParser : public IDpParse
{
public:
	YLDpParser();
	bool HandleDp(const char* szFileName);

private:
	bool			IsValueOnlyDGI(unsigned short sDGINO);
	bool			IsEncryptDGI(string tag);
	void			ReadDGIName(ifstream &dpFile, streamoff offset);						//读取DGI名称
	char			ReadDGIStartTag(ifstream &dpFile, streamoff offset);					//读取起始标签
	unsigned short	ReadFollowedDataLength(ifstream &dpFile, streamoff offset);			//读取数据长度
	int				ReadCardSequence(ifstream &dpFile, streamoff offset);				//读取卡片序列号
	int				ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset);	//读取卡片个人化数据总长度
	unsigned short	ReadDGISequence(ifstream &dpFile, streamoff offset);					//读取卡片DGI分组序号
	char			ReadRecordTemplate(ifstream &dpFile, streamoff offset);				//读取记录模板
	void			ParseTLV(char* buffer, int nBufferLen, Dict& tlvs);		//解析TLV结构
	bool			CheckFolderExist(const string &strPath);
	void			FilterDpData();
	string			DecrptData(string tag, string value);

private:

	vector<string>              m_vecDGI;
	string						m_currentAccount;
	const int                   m_reserved = 8596;  //银联保留的文件头大小
	vector<string>              m_encryptTag;       //加密的tag
	vector<unsigned short>      m_valueOnlyDGI;     //仅还有value的tag
	map<string, string>         m_exchangeDGI;      //需要交换的tag
	string                      m_key;              //解密密钥
};

