#pragma once


/**************************************************************
* TLV������ʽ(����DP�����ļ��Ľ���)
***************************************************************/
typedef struct ASCII_TLV
{
    unsigned char* tag;			//��ǩ
    unsigned char* length;		//����
    unsigned char* value;		//����
    unsigned int tagSize;		//��ǩռ���ֽ�  һ�������ֽ�
    unsigned int lengthSize;	//����ռ�õ��ֽ�
    bool         isTemplate;    //�Ƿ�Ϊģ��
    ASCII_TLV* subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
    unsigned int subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
}*PASCII_TLV;


typedef struct BCD_TLV
{
    unsigned char*	tag;			//��ǩ
    unsigned char*	length;			//����
    unsigned char*	value;			//����
    unsigned int	tagSize;		//��ǩռ���ֽ�  һ�������ֽ�
    unsigned int	lenSize;		//����ռ�õ��ֽ�
    bool			isTemplate;		//�Ƿ�Ϊ���Ͻṹ��Ҳ�������tag����template
    BCD_TLV*			subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
    unsigned int	subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
} *PBCD_TLV;

bool ParseBcdTLV(char* buffer, PBCD_TLV pTlvs, unsigned int& count);

void ParseAsciiTLV(unsigned char *buffer, unsigned int bufferLength, PASCII_TLV PTlvEntity, unsigned int& entitySize);

bool IsAsciiTlvStruct(unsigned char* buffer, unsigned int bufferLength);
bool IsBcdTlvStruct(char* buffer, unsigned int bufferLength);