#pragma once
/*************************************************
* 用于设置和获取终端数据
**************************************************/
#include <string>
#include "Util\IniParaser.h"
using namespace std;
//case 0x5F2A:	return "0156";		//交易货币代码, 默认RMB


//case 0x9F02:	return "000000010000";	//授权金额，默认100元
//case 0x9F03:	return "000000000000";	//其他金额	
//case 0x9F09:	return "0030";		//终端应用版本号
//case 0x9F1A:	return "0156";		//终端国家代码，默认中国
//case 0x9F1B:	return "000000000000";	//终端最低限额
//case 0x9F37:	return "11223344";	//随机数
//case 0x9F42:	return "0156";		//应用货币代码，默认中国
//case 0x9F4E:	return "6368696E61756E696F6E7061792E616263643132";		//商户名称
//case 0x9F66:    return "76C00000";  //终端交易属性
//case 0x9C:		return "00";			//交易类型

//case 0x95:		return GetTVR();	//终端验证结果TVR
//case 0x9A:		return GenTransDate();	//交易日期

//case 0x9F21:	return GenTransTime();			//交易时间

//case 0x9F7A:	return "00";		//电子现金交易指示器
//case 0xDF60:    return "00";    //终端不支持扩展交易
//case 0xDF69:    return "01";        //SM2算法支持指示器，默认采用DES算法
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

	static INIParser m_parse;
	static bool bLoaded;
};