#pragma once
#include <map>
#include <vector>
#include <string>
#include "../DpParse//IDpParse.h"
using namespace std;



class YLDpParser : public IDpParse
{
public:
	YLDpParser();
	bool HandleDp(const char* szFileName);

private:
	void			ReadDGIName(ifstream &dpFile);						//��ȡDGI����
	char			ReadDGIStartTag(ifstream &dpFile);					//��ȡ��ʼ��ǩ
	unsigned short	GetFollowedDataLen(ifstream &dpFile);			//��ȡ���ݳ���
	int				GetOneCardDpDataLen(ifstream &dpFile);	//��ȡ��Ƭ���˻������ܳ���
	string			GetDgiSeq(ifstream &dpFile);					//��ȡ��ƬDGI�������
	char			ReadRecordTemplate(ifstream &dpFile, streamoff offset);				//��ȡ��¼ģ��
	void			ParseTLVEx(char* buffer, int nBufferLen, Dict& tlvs);		//����TLV�ṹ
	string			DecrptData(string tag, string value);
	int				ParsePSE(ifstream &dpFile, DGI_ITEM &dgiItem);	//����PSE PPSE
private:

	vector<string>              m_vecDGI;
	string						m_currentAccount;
	const int                   m_reserved = 8596;  //�����������ļ�ͷ��С
	vector<string>              m_encryptTag;       //���ܵ�tag
	vector<unsigned short>      m_valueOnlyDGI;     //������value��tag
	map<string, string>         m_exchangeDGI;      //��Ҫ������tag
	string                      m_key;              //������Կ
};

