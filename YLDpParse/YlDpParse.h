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
	void			ReadDGIName(ifstream &dpFile, streamoff offset);						//��ȡDGI����
	char			ReadDGIStartTag(ifstream &dpFile, streamoff offset);					//��ȡ��ʼ��ǩ
	unsigned short	ReadFollowedDataLength(ifstream &dpFile, streamoff offset);			//��ȡ���ݳ���
	int				ReadCardSequence(ifstream &dpFile, streamoff offset);				//��ȡ��Ƭ���к�
	int				ReadCardPersonalizedTotelLen(ifstream &dpFile, streamoff offset);	//��ȡ��Ƭ���˻������ܳ���
	unsigned short	ReadDGISequence(ifstream &dpFile, streamoff offset);					//��ȡ��ƬDGI�������
	char			ReadRecordTemplate(ifstream &dpFile, streamoff offset);				//��ȡ��¼ģ��
	void			ParseTLV(char* buffer, int nBufferLen, Dict& tlvs);		//����TLV�ṹ
	bool			CheckFolderExist(const string &strPath);
	void			FilterDpData();
	string			DecrptData(string tag, string value);

private:

	vector<string>              m_vecDGI;
	string						m_currentAccount;
	const int                   m_reserved = 8596;  //�����������ļ�ͷ��С
	vector<string>              m_encryptTag;       //���ܵ�tag
	vector<unsigned short>      m_valueOnlyDGI;     //������value��tag
	map<string, string>         m_exchangeDGI;      //��Ҫ������tag
	string                      m_key;              //������Կ
};

