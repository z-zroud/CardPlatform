#pragma once
/****************************************************
* �ö�̬���ṩһϵ�е��ַ���������֮���ת���ӿڣ�
* ASCII��BCD��֮���ת����
*****************************************************/

#ifdef TOOL_EXPORTS
#define TOOL_API __declspec(dllexport)
#else
#define TOOL_API __declspec(dllimport)
#endif

extern "C"
{
    /*************************************************
    * ���ܣ���inputת��Ϊʮ�����Ƶ��ַ�����ʽ����input
    * ��0~FF֮�䣬outputΪ2�ֽڡ�FF~FFFF֮����������
    **************************************************/
    void TOOL_API IntToHexStr(int input, char* output, int outputLen);

    int TOOL_API HexStrToInt(const char* hexStr);

    /*************************************************
    * �ַ�����BCD��֮��Ļ���ת��
    **************************************************/
    void TOOL_API StrToBcd(const char* str, int strLen, char* bcd, int bcdLen);

    void TOOL_API BcdToStr(const char* bcd, char* str, int strLen);

    /*************************************************
    * ���ܣ���ȡstr��ָ��λ�õ�ָ�����ȵ����ַ���
    **************************************************/
    void TOOL_API GetSubString(const char* str, int start, int len, char* subStr);

    /*************************************************
    * ���ܣ���startλ�ý�ȡ�ַ����Ұ벿��
    **************************************************/
    void TOOL_API GetStringRightPart(const char* str, int start, char* subStr);

    /*************************************************
    * ���ܣ�ɾ���ַ��������еĿո�
    **************************************************/
    void TOOL_API Trim(const char* intput, char* output, int outputLen);

    /*************************************************
    * ���ܣ��ж�hexStr�Ƿ�ȫ��1-9,a-z,A-Z�ַ�
    **************************************************/
    bool TOOL_API IsHexStr(const char* hexStr);

    /**************************************************
    * ����: Base64�ַ���תBCD��
    ***************************************************/
    void TOOL_API Base64ToBcd(const char* base64, char* bcd, int bcdLen);
}