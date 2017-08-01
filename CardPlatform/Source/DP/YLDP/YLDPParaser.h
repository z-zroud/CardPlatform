#pragma once
#include "../DPParaser.h"

struct YLPersonlizedDpData
{
	int nCardSequence;
	vector<DGI> vecDpData;
	string strAccount;	//��Ƭ�˺���Ϊ������ļ�����
};

class YLDpParser : public DPParser
{
public:
	YLDpParser();
	int Read(const string& filePath);
	void Save(const string &strPath = "YLDP");

private:
	bool			IsNeedConvertDGI(unsigned short sDGINO);
	bool			IsNeedDecrpt(string tag);
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
    string          GetTagValue(string Tag);
    void            SetTagValue(string tag, string value);

private:
	vector<unsigned short> m_ValueOnlyDGI;
	vector<string> m_encryptTag;
	vector<string> m_DGIName;
	const int m_reserved = 8596;
	YLPersonlizedDpData m_CurrentDpData;
	vector<YLPersonlizedDpData> m_YLDpData;	//����һ�������ļ��������ſ�Ƭ����
	vector<DP>	m_vecDP;	//m_YLDpData���˺�����ݸ�ʽ
};

