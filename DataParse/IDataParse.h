#pragma once

#ifdef DATAPARSE_EXPORTS
#define DATAPARSE_EXPORTS_API __declspec(dllexport)
#else
#define DATAPARSE_EXPORTS_API __declspec(dllimport)
#endif

/*****************************************************
* �����TLV�ṹ���������Ķ�������
******************************************************/
typedef struct TLV
{
    bool            isTemplate;     //�Ƿ���ģ��
    int             level;          //�㼶
    char*           tag;            //��ǩ
    unsigned int    length;         //����
    char*           value;          //ֵ
}*PTLV;

typedef struct TL
{
	unsigned char* tag;		//��ǩ
	unsigned int len;		//����
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
extern "C" DATAPARSE_EXPORTS_API void ParseTL(char* bcdBuffer, PTL pTls, unsigned int& count);

/**********************************************************************
* ����TLV���ݽṹ
* ������ buffer ��������TLV����
* ������ pTlvs ����֮���TLV����
* ������ count ����TLV�ĸ���
* ���أ� true ˵�������ɹ���false TLV��ʽ����ȷ
**********************************************************************/
//extern "C" DATAPARSE_EXPORTS_API bool ParseTLV(char* bcdBuffer, PTLV pTlvs, unsigned int& count);


/*********************************************************************
* ����TLV���ݽṹ
*
* ���أ� true ˵�������ɹ���false TLV��ʽ����ȷ
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API bool ParseTLV(char* bcdBuffer, PTLV pTlv, unsigned int& count);



/**********************************************************************
* ����AFL�ṹ�����������ȡ��¼����
* ������ buffer ��������TLV����
* ������ pTlvs ����֮���TLV����
* ������ count ����TLV�ĸ���
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseAFL(char* bcdBuffer, PAFL pAfls, unsigned int& count);