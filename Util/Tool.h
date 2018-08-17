#pragma once
#include <string>
#include <vector>

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
    string BcdToStr(const char* bcd);
	void BcdToAsc(unsigned char *asc, unsigned char *bcd, long bcd_len);	//Ascii ת BCD
	long AscToBin(char *ptr, int len);	//Ascii ת ������
	void AscToBcd(char *bcd, char *asc, long asc_len);	//BCDתAscii
	void StrToHex(unsigned char *pDest, unsigned char *pSrc, int nLen);
    void DeleteSpace(const char* src, char* dest, int len);
    string Trim(string str);
    int ctoi(unsigned char c);
    string StrToBcd(const char* str, int len);
    string base64_decode(string const& encoded_string, int& len);
	/**********************************************************
	* ���ܣ�
	***********************************************************/
	void GetBcdDataLen(const char* bcdLen, char* output, int len);
	void IncreaseStep(const char* strLen, int step, char* output, int len);

    string GetBcdStrLen(string data);

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

    //��ʮ�������ַ���ת��Ϊ����
    int HexStrToInt(string str);

    /**********************************************************
    * ���ܣ���BCD��ת��Ϊstring�ַ���
    ************************************************************/
    void BcdToStr(const char* bcd, char* str, int strLen);

    /***********************************************************
    * ��ȡ�ļ�����Ŀ¼·��
    ************************************************************/
    string GetDirectory(const char* filePath);

    /***********************************************************
    * ���ܣ����ַ���str��sep�ָ�����ַ������洢��subStrs������
    ************************************************************/
    void SplitStr(const string& str, string sep, vector<string>& subStrs);

    bool IsHexStr(string str);
}

//#ifdef __cplusplus
//}
//#endif

