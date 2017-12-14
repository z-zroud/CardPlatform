#pragma once

#ifdef DATAPARSE_EXPORTS
#define DATAPARSE_EXPORTS_API __declspec(dllexport)
#else
#define DATAPARSE_EXPORTS_API __declspec(dllimport)
#endif



typedef struct TLV
{
	unsigned char*	tag;			//��ǩ
	unsigned char*	length;			//����
	unsigned char*	value;			//����
	unsigned int	tagSize;		//��ǩռ���ֽ�  һ�������ֽ�
	unsigned int	lenSize;		//����ռ�õ��ֽ�
	bool			isTemplate;		//�Ƿ�Ϊ���Ͻṹ��Ҳ�������tag����template
	TLV*			subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
	unsigned int	subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
} *PTLV;

typedef struct TL
{
	unsigned char* tag;		//��ǩ
	unsigned char* len;		//����
}*PTL;

//Ӧ���ļ���λ��(AFL)
typedef struct AFL
{
	int		sfi;				//���ļ���ʶ��
	int		recordNumber;		//��¼��
	bool	bSigStaticData;		//�Ƿ�Ϊǩ��������(������Щ������Ҫ���뵽ǩ����)
}*PAFL;

/**********************************************************************
* ����DOL���ݽṹ����TL������value��TLV
* ������ buffer ��������TL����
* ������ pTls ����֮���TL����
* ������ count ����TL�ĸ���
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseTL(char* buffer, PTL pTls, unsigned int& count);

/**********************************************************************
* ����TLV���ݽṹ
* ������ buffer ��������TLV����
* ������ pTlvs ����֮���TLV����
* ������ count ����TLV�ĸ���
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseTLV(char* buffer, PTLV pTlvs, unsigned int& count);

/**********************************************************************
* ����AFL�ṹ�����������ȡ��¼����
* ������ buffer ��������TLV����
* ������ pTlvs ����֮���TLV����
* ������ count ����TLV�ĸ���
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseAFL(char* buffer, PAFL pAfls, unsigned int& count);