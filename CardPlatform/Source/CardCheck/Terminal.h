#pragma once
/*************************************************
* �������úͻ�ȡ�ն�����
**************************************************/
#include <string>
#include "Util\IniParaser.h"
using namespace std;
//case 0x5F2A:	return "0156";		//���׻��Ҵ���, Ĭ��RMB


//case 0x9F02:	return "000000010000";	//��Ȩ��Ĭ��100Ԫ
//case 0x9F03:	return "000000000000";	//�������	
//case 0x9F09:	return "0030";		//�ն�Ӧ�ð汾��
//case 0x9F1A:	return "0156";		//�ն˹��Ҵ��룬Ĭ���й�
//case 0x9F1B:	return "000000000000";	//�ն�����޶�
//case 0x9F37:	return "11223344";	//�����
//case 0x9F42:	return "0156";		//Ӧ�û��Ҵ��룬Ĭ���й�
//case 0x9F4E:	return "6368696E61756E696F6E7061792E616263643132";		//�̻�����
//case 0x9F66:    return "76C00000";  //�ն˽�������
//case 0x9C:		return "00";			//��������

//case 0x95:		return GetTVR();	//�ն���֤���TVR
//case 0x9A:		return GenTransDate();	//��������

//case 0x9F21:	return GenTransTime();			//����ʱ��

//case 0x9F7A:	return "00";		//�����ֽ���ָʾ��
//case 0xDF60:    return "00";    //�ն˲�֧����չ����
//case 0xDF69:    return "01";        //SM2�㷨֧��ָʾ����Ĭ�ϲ���DES�㷨
//
class CTerminal
{
public:
	static void LoadData(const string &filePath);
	static void SetTerminalData(const string &tag, const string &value);
	static string GetTerminalData(const string &tag);
	static string Get5F2A();
	static string Get95();
	static string Get9A();
	static string Get9C();
	static string Get9F02();
	static string Get9F03();
	static string Get9F09();
	static string Get9F1A();
	static string Get9F1B();
	static string Get9F21();
	static string Get9F37();
	static string Get9F42();
	static string Get9F4E();
	static string Get9F66();
	static string Get9F7A();
	static string GetDF60();
	static string GetDF69();

	static void Set5F2A(string value);
	static void Set95(string value);
	//static void Set9A(string value);
	static void Set9C(string value);
	static void Set9F02(string value);
	static void Set9F03(string value);
	static void Set9F09(string value);
	static void Set9F1A(string value);
	static void Set9F1B(string value);
	//static void Set9F21(string value);
	static void Set9F37(string value);
	static void Set9F42(string value);
	static void Set9F4E(string value);
	static void Set9F66(string value);
	static void Set9F7A(string value);
	static void SetDF60(string value);
	static void SetDF69(string value);

private:
    static void InitTerminalTags();
private:
	static string m_5F2A;
	static string m_95;
	static string m_9A;
	static string m_9C;
	static string m_9F02;
	static string m_9F03;
	static string m_9F09;
	static string m_9F1A;
	static string m_9F1B;
	static string m_9F21;
	static string m_9F37;
	static string m_9F42;
	static string m_9F4E;
	static string m_9F66;
	static string m_9F7A;
	static string m_DF60;
	static string m_DF69;

	static IniParser m_parse;
	static bool bLoaded;
};