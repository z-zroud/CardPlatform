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
	void			ReadDGIName(ifstream &dpFile, streamoff offset);						//��ȡDGI����
	void			DealPSEDataCompleted(ifstream &dpFile, streamoff offset);			//����PSE����
	char			ReadDGIStartTag(ifstream &dpFile, streamoff offset);					//��ȡ��ʼ��ǩ
	unsigned short	ReadFollowedDataLength(ifstream &dpFile, streamoff offset);			//��ȡ���ݳ���
	int				ReadCardSequence(ifstream &dpFile, streamoff offset);				//��ȡ��Ƭ���к�
	int				ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset);	//��ȡ��Ƭ���˻������ܳ���
	unsigned short	ReadDGISequence(ifstream &dpFile, streamoff offset);					//��ȡ��ƬDGI�������
	char			ReadRecordTemplate(ifstream &dpFile, streamoff offset);				//��ȡ��¼ģ��
	void			ParseTLV(char* buffer, int nBufferLen, DGI &YLDGI);		//����TLV�ṹ
	bool			CheckFolderExist(const string &strPath);
	void			FilterDpData();
	string			DecrptData(string tag, string value);
	void			ClearCurrentDPData();
    string          GetTagValue(string Tag, OneCardPersoData &oneCardDpData);
    void            SetTagValue(string tag, string value, OneCardPersoData &oneCardDpData);

private:
	
	
	vector<string>              m_dGIName;	
    OneCardPersoData            m_oneCardDpData;
	vector<OneCardPersoData>    m_manyCardDpData;	//����һ�������ļ��������ſ�Ƭ����
	vector<CPSDP>               m_vecCPSDP;	//CPS DP���ݽṹ
    const int                   m_reserved = 8596;  //�����������ļ�ͷ��С
    vector<string>              m_encryptTag;       //���ܵ�tag
    vector<unsigned short>      m_valueOnlyDGI;     //������value��tag
    map<string, string>         m_exchangeDGI;      //��Ҫ������tag
    string                      m_key;              //������Կ
};

