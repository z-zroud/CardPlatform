// PbocTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include "..\PCSC\PCSC.h"
#include "..\ApduCmd\IApdu.h"
#include "..\DataParse\IDataParse.h"
#include "..\Util\Tool.h"
#include "..\\Authencation\IGenKey.h"

using namespace std;

#define SW_9000 0x9000

vector<pair<string, string>> g_Tags;	//保存所有的卡片Tag值
string g_sigStaticData;					//保存需要签名的数据

/****************************************
* 保存卡片中的Tag值，供后续使用
*****************************************/
void SetTags(string tag, string value)
{
	bool bExist = false;
	for (auto& item : g_Tags)
	{
		if (item.first == tag) {
			item.second = value;
			bExist = true;
			break;
		}
	}
	if (!bExist) {
		g_Tags.push_back(pair<string, string>(tag, value));
	}
}

/*********************************************
* 保存卡片中的Tag值，供后续使用,Tag从TLV结构中取
**********************************************/
void SetTags(PTLV tlvs, int num)
{
	for (int i = 0; i < num; i++)
	{
        SetTags((char*)tlvs[i].tag, (char*)tlvs[i].value);
	}
}

/*********************************************
* 从保存的tag容器中获取指定的tag值
**********************************************/
string GetTag(string tag)
{
	for (auto item : g_Tags)
	{
		if (item.first == tag)
			return item.second;
	}
	return "";
}

int main()
{
	/********************** 打开读卡器 ********************************/
	int count = 0;
	char* readers[2] = { 0 };
	GetReaders(readers, count);
	for (int i = 0; i < count; i++)
	{
		printf("%s\n", readers[i]);
	}
	if (!OpenReader(readers[0]))
	{
		return 1;
	}

	char atr[128] = { 0 };
	GetATR(atr, sizeof(atr));
	printf("%s\n", atr);

	/************************* 选择应用 ****************************/
	int sw = 0;
	char selectAppResp[1024] = { 0 };
	sw = SelectAppCmd("325041592E5359532E4444463031", selectAppResp);
	if (sw != SW_9000)
	{
		printf("选择应用失败!\n");
		return 2;
	}
	unsigned int tlvCount = 12;
	TLV tlvs[12];
	ParseTLV(selectAppResp, tlvs, tlvCount);
	SetTags(tlvs, tlvCount);

	char resp[1024] = { 0 };
	string aid = GetTag("4F");	
	sw = SelectAppCmd(aid.c_str(), resp);
	if (sw != SW_9000)
	{
		printf("选择应用失败!\n");
		return 2;
	}
	TLV tlv1[12];
	ParseTLV(resp, tlv1, tlvCount);
	SetTags(tlv1, tlvCount);

	/*************************** 应用初始化 *****************************/
	char* terminalData = "660000000000000001000000000000000156000000000001561712150002A943E700";
	memset(resp, 0, 1024);
	sw = GPOCmd(terminalData, resp);
	if (sw != SW_9000)
	{
		printf("GPO 失败!\n");
		return 2;
	}
	char aip[5] = { 0 };
	char afl[128] = { 0 };
	Tool::SubStr(resp, 4, 4, aip);
	Tool::SubStr(resp, 8, strlen(resp) - 8, afl);
	SetTags("82", aip);
	SetTags("94", afl);

	/************************** 读应用记录 *******************************/
	AFL pAfls[12] = { 0 };
	unsigned int aflCount = 12;
	ParseAFL(afl, pAfls, aflCount);
	for (int i = 0; i < aflCount; i++)
	{
		memset(resp, 0, 1024);
		ReadRecordCmd(pAfls[i].sfi, pAfls[i].recordNumber, resp);
		TLV tlv3[12];
		ParseTLV(resp, tlv3, tlvCount);
		SetTags(tlv3, tlvCount);

		if (pAfls[i].bSigStaticData)
		{
			g_sigStaticData += string(resp).substr(4); //除去模板及长度
		}
	}

	/************************* 脱机数据认证 *********************************/
	char caPublicKey[2048] = { 0 };
	string caIndex = GetTag("8F");
	GenCAPublicKey(caIndex.c_str(), "A000000333", caPublicKey);

	string issuerPublicCert = GetTag("90");	
	string ipkRemainder = GetTag("92");
	string issuerExponent = GetTag("9F32");
	char issuerPublicKey[2048] = { 0 };
	GenDesIssuerPublicKey(caPublicKey, issuerPublicCert.c_str(), ipkRemainder.c_str(), issuerExponent.c_str(), issuerPublicKey);

	string tag93 = GetTag("93");
	string tag82 = GetTag("82");
	DES_SDA(issuerPublicKey, issuerExponent.c_str(), tag93.c_str(), g_sigStaticData.c_str(), tag82.c_str());

	string iccPublicCert = GetTag("9F46");
	string iccRemainder = GetTag("9F48");
	string iccExponent = GetTag("9F47");
	char iccPublicKey[2048] = { 0 };
	printf("IPK: %s\n", issuerPublicKey);
	printf("ICC Cert: %s\n",iccPublicCert.c_str());
	GenDesICCPublicKey(issuerPublicKey, iccPublicCert.c_str(), iccRemainder.c_str(), g_sigStaticData.c_str(), iccExponent.c_str(), tag82.c_str(), iccPublicKey);

	string ddol = GetTag("9F49"); //动态数据认证数据对象列表DDOL
	TL pTls[32];
	unsigned int ddolCount = 32;
	ParseTL((char*)ddol.c_str(), pTls, ddolCount);

	string ddolData;
	//for (int i = 0; i < ddolCount; i++)	//一般ddol只是生成终端随机数传入卡片，这里简化步骤，省去ddolData的拼接过程
	//{
	//	ddolData += GetTerminalData(pTls[i]);	
	//}
	ddolData = "02A943E7";
	char tag9F4B[RESP_LEN] = { 0 };

	GenDynamicData(ddolData.c_str(), tag9F4B);
	string dynamicData;
	printf("ICC public key: %s\n", iccPublicKey);
	printf("tag9F4B: %s\n", tag9F4B);
	DES_DDA(iccPublicKey, iccExponent.c_str(), tag9F4B, ddolData.c_str());

	/*************************** 处理限制 ********************************/
	//主要检查点(由于这些检查不涉及tag数据的处理，在此省略对这些值的检查)
	//1. 应用生效日期检查
	//2. 应用失效日期检查
	//3. 应用版本号检查
	//4. AUC检查
	//5. 发卡行国家代码检查
	/**************************** 持卡人验证 *****************************/
	// 主要检查点(由于只涉及到Tag处理，不涉及Apdu指令等处理，省略处理这些操作)
	//1. X/Y金额检查
	//2. CVM 列表解析
	/**************************** 终端风险管理 ****************************/
	//由于也是涉及tag的检查，不涉及Apdu指令，省略处理
	//1. 终端异常文件检查
	//2. 商户强制联机交易
	//3. 最低限额检查
	//4. 频度检查
	//5. 新卡检查
	/*************************** 终端行为分析 *****************************/
	string cdol1 = GetTag("8C"); //动态数据认证数据对象列表DDOL
	TL cdol1TL[32];
	unsigned int cdol1Count = 32;
	ParseTL((char*)cdol1.c_str(), cdol1TL, cdol1Count);
	string cdol1Data;
	//for (int i = 0; i < cdol1Count; i++)	//这些值由终端根据交易检查结果指定，省略这些处理，填写固定值
	//{
	//	cdol1Data += GetTerminalData(cdol1TL[i]);	
	//}
	cdol1Data = "0000000001000000000000000156000004600001561712150002A943E7100429677579756875612D476F6C647061632032303132";
	memset(resp, 0, 1024);

	//终端行为分析完全由终端执行，不涉及卡片处理，也就是终端根据TVR结果决定GAC是发送AAC/ARQC/TC等，
	//由于不涉及APDU指令，这里默认采用ARQC行为。
	/**************************** 卡片行为分析 ***********************************/
	//卡片行为分析，也就是当终端发送GAC1命令后，卡片根据终端传入的CDOL1数据，进行相关检查
	//这里的检查由卡片内部(COS逻辑分析)完成，因此，外部检查程序只能得到卡片分析之后的结果
	//也就是GAC返回值
	sw = GACCmd(ARQC, cdol1Data.c_str(), resp);
	if (sw != SW_9000)
	{
		printf("GAC1 失败!\n");
		return 2;
	}

	char tag9F27[32] = { 0 };
	char tag9F36[32] = { 0 };
	char tag9F26[32] = { 0 };
	char tag9F10[128] = { 0 };

	Tool::SubStr(resp, 4, 2, tag9F27);
	Tool::SubStr(resp, 6, 4, tag9F36);
	Tool::SubStr(resp, 10, 16, tag9F26);
	Tool::SubStr(resp, 26, strlen(resp) - 26, tag9F10);

	SetTags("9F27", tag9F27);
	SetTags("9F36", tag9F36);
	SetTags("9F26", tag9F26);
	SetTags("9F10", tag9F10);

	/***************************** 发卡行认证 ***************************/
	char udkSessionKey[128] = { 0 };
	string atc = GetTag("9F36");
	char* udkAuthKey = "9EE66BE0F25DA4D308978CDF643BD00D";
	GenUdkSessionKey(udkAuthKey, atc.c_str(), udkSessionKey);

	char arpc[128] = { 0 };
	char* authCode = "3030";
	string ac = GetTag("9F26");	
	GenArpc(udkSessionKey, (char*)ac.c_str(), authCode, arpc);

	memset(resp, 0, 1024);
	sw = ExternalAuthencationCmd(arpc, authCode, resp);
	if (sw != SW_9000)
	{
		printf("外部认证 失败!\n");
		return 2;
	}
	/****************************** 交易结束 ******************************/
	//发送GAC2命令

	string cdol2 = GetTag("8D"); //动态数据认证数据对象列表DDOL
	TL cdol2TL[32];
	unsigned int cdol2Count = 32;
	ParseTL((char*)cdol2.c_str(), cdol2TL, cdol2Count);
	string cdol2Data;
	//for (int i = 0; i < cdol2Count; i++)	//这些值由终端根据交易检查结果指定，省略这些处理，填写固定值
	//{
	//	cdol2Data += GetTerminalData(cdol1TL[i]);	
	//}
	cdol2Data = "30300000000001000000000000000156000004600001561712150002A943E7100429677579756875612D476F6C647061632032303132";
	memset(resp, 0, 1024);
	//缺省 接受
	sw = GACCmd(TC, cdol2Data.c_str(), resp);
	if (sw != SW_9000)
	{
		printf("GAC2 失败!\n");
		return 2;
	}

	Tool::SubStr(resp, 4, 2, tag9F27);
	Tool::SubStr(resp, 6, 4, tag9F36);
	//Tool::SubStr(resp, 10, 16, tag9F26);
	Tool::SubStr(resp, 26, strlen(resp) - 26, tag9F10);

	SetTags("9F27", tag9F27);
	SetTags("9F36", tag9F36);
	//SetTags("9F26", tag9F26);
	SetTags("9F10", tag9F10);

	/***************************** 发卡行脚本处理 ****************************/
	//圈存500
	char mac[32] = { 0 };
	char udkMacKey[] = "C1D9DC08546E2FD5D6709D49800DB910";
	char udkMacSessionKey[33] = { 0 };
	GenUdkSessionKey(udkMacKey, atc.c_str(), udkMacSessionKey);

	string data = "04DA9F790A" + GetTag("9F36") + GetTag("9F26") + "000000050000";
	//GenIssuerScriptMac(udkMacSessionKey, data.c_str(), mac);
	//sw = PutDataCmd("9F79", "000000050000", mac);
	//if (sw != SW_9000)
	//{
	//	printf("圈存 失败!\n");
	//	return 2;
	//}
    return 0;
}

