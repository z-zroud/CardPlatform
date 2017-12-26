#pragma once
#include <string>

enum Hex_Len
{
	Two_byte = 2,
	Four_byte = 4
};


using namespace std;
//#ifdef __cplusplus
//extern "C"
//{
//#endif
namespace Tool 
{
	//typedef unsigned char byte;
	void BcdToAsc(unsigned char *asc, unsigned char *bcd, long bcd_len);	//Ascii ת BCD
	long AscToBin(char *ptr, int len);	//Ascii ת ������
	void AscToBcd(char *bcd, char *asc, long asc_len);	//BCDתAscii
	void StrToHex(unsigned char *pDest, unsigned char *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);
    string Trim(string str);

	/**********************************************************
	* ���ܣ�
	***********************************************************/
	void GetBcdDataLen(const char* bcdLen, char* output, int len);
	void IncreaseStep(const char* strLen, int step, char* output, int len);

	/***********************************************************
	* ���ܣ� ��ȡ���ַ���
	* ������ src Դ�ַ���
	* ������ start ��ȡԴ�ַ�������ʼλ��
	* ������ len ��ȡ�ĳ���
	* ������ dst ������ַ���
	************************************************************/
	bool SubStr(const char* src, int start, int len, char* dst);

	/*********************************************************
	* ���ܣ� ������ת��Ϊ�ַ�����ʽ
	* ������ value ��������
	* ������ hexStr �����ʮ�������ַ�����ʽ
	* ������ ָ������ĳ��� Two_byte ���� Four_byte
	**********************************************************/
	void IntToStr(int value, char* hexStr, Hex_Len hexLen);
}

//#ifdef __cplusplus
//}
//#endif

