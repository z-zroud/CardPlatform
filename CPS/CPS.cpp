// CPS.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ICPS.h"
#include "IniConfig.h"
#include "Des0.h"
#include "../ApduCmd/IApdu.h"

string g_kmc;
string g_isd;
int g_divMethod = 0;
int g_secureLevel = 0;

struct InstallParam
{
	string packageAid;
	string appletAid;
	string instanceAid;
	string privilege;
	string installParam;
	string token;
};

void GetInstallParam(IniConfig cfg, string appType, InstallParam& param)
{
	param.packageAid = cfg.GetValue(appType, "PackageAid");
	param.appletAid = cfg.GetValue(appType, "AppletAid");
	param.instanceAid = cfg.GetValue(appType, "InstanceAid");
	param.privilege = cfg.GetValue(appType, "Privilege");
	param.installParam = cfg.GetValue(appType, "InstallParam");
	param.token = cfg.GetValue(appType, "Token");
}

int InstallApp(InstallParam& param)
{
	return InstallAppCmd(param.packageAid.c_str(), param.appletAid.c_str(), param.instanceAid.c_str(), param.privilege.c_str(), param.installParam.c_str(), param.token.c_str());
}
/******************************************************
* 生成统一CPS文件,便于个人化
* 参数说明： szDllName 用于处理DP文件的Dll名称
* 参数说明： szFileName DP文件路径
* 返回值： DP文件处理成功返回true, 否则返回false
*******************************************************/
bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szFileName)
{
	HINSTANCE hInst = NULL;

	hInst = LoadLibrary(szDllName);
	if (hInst == NULL)
	{
		return false;
	}
	PHandleDpCallback pHandleDpFunc = (PHandleDpCallback)GetProcAddress(hInst, szFuncName);
	if (pHandleDpFunc == NULL)
	{
		return false;
	}
	return pHandleDpFunc(szFileName);
}



void SetPersonlizationConfig(const char* isd,const char* kmc, int divMethod, int secureLevel)
{
	g_isd = isd;
	g_kmc = kmc;
	g_divMethod = divMethod;
	g_secureLevel = secureLevel;
}

string DecryptDGIData(string encSessionKey, string dgi, string data)
{
	if (dgi == "8201" ||
		dgi == "8202" ||
		dgi == "8203" ||
		dgi == "8204" ||
		dgi == "8205")
	{
		if (data.length() % 16 == 0)
		{
			data += "8000000000000000";
		}
		else {
			data += "80";
			int remaindZero = data.length() % 16;
			data.append(remaindZero, '0');
		}
	}
	char decryptData[2048] = { 0 };
	Des3_ECB(decryptData, (char*)encSessionKey.c_str(), (char*)data.c_str(), data.length());

	return decryptData;
}

bool IsNeedDecrypt(string dgi)
{
	vector<string> vecDGIs = { "8000","8201","8202","8203" ,"8204","8205","8020" };
	for (auto item : vecDGIs) {
		if (dgi == item)
			return true;
	}

	return false;
}

int DoPersonlizationWithOrderdDGI(const char* szCpsFile, const char* iniConfigFile)
{
	IniConfig cpsFile;

	IniConfig cfgFile;
	if (!cfgFile.Read(iniConfigFile))
	{
		return 1;
	}
	//获取相关配置信息
	if (!cpsFile.Read(szCpsFile))
	{
		return 2;
	}

	char resp[RESP_LEN] = { 0 };
	int sw = SelectAppCmd(g_isd.c_str(), resp);
	if (sw != 0x9000) {
		return false;
	}
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
		return 3;
	}
	//安装PSE
	InstallParam pseParam;
	GetInstallParam(cfgFile, "PSE", pseParam);
	sw = InstallApp(pseParam);
	if (sw != 0x9000) {
		return sw;
	}
	//安装PPSE
	InstallParam ppseParam;
	GetInstallParam(cfgFile, "PPSE", ppseParam);
	sw = InstallApp(ppseParam);
	if (sw != 0x9000) {
		return sw;
	}
	//安装金融APP
	InstallParam appParam;
	GetInstallParam(cfgFile, "APP", appParam);
	sw = InstallApp(appParam);
	if (sw != 0x9000) {
		return sw;
	}
	//个人化PSE
	sw = SelectAppCmd(pseParam.instanceAid.c_str(), resp);
	if (sw != 0x9000) {
		return sw;
	}
	if (!OpenSecureChannel(g_kmc.c_str(),g_divMethod,g_secureLevel)) {
		return 3;
	}
	vector<pair<string, string>> pseDGIs;
	cpsFile.GetValue("PSE", pseDGIs);
	int dgiSize = pseDGIs.size();
	if (dgiSize == 1) {
		StoreDataCmd(pseDGIs[0].second.c_str(), STORE_DATA_END, true);
	}
	else {
		StoreDataCmd(pseDGIs[0].second.c_str(), STORE_DATA_PLANT, true);
		for (int i = 1; i < dgiSize - 1; i++) {
			StoreDataCmd(pseDGIs[i].second.c_str(), STORE_DATA_PLANT, false);
		}
		StoreDataCmd(pseDGIs[dgiSize - 1].second.c_str(), STORE_DATA_END, false);
	}
	
	//个人化PPSE
	sw = SelectAppCmd(ppseParam.instanceAid.c_str(), resp);
	if (sw != 0x9000) {
		return sw;
	}
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
		return 3;
	}

	vector<pair<string, string>> ppseDGIs;
	cpsFile.GetValue("PPSE", ppseDGIs);
	dgiSize = ppseDGIs.size();
	if (dgiSize == 1) {
		StoreDataCmd(ppseDGIs[0].second.c_str(), STORE_DATA_END, true);
	}
	else {
		StoreDataCmd(ppseDGIs[0].second.c_str(), STORE_DATA_PLANT, true);
		for (int i = 1; i < dgiSize - 1; i++) {
			StoreDataCmd(ppseDGIs[i].second.c_str(), STORE_DATA_PLANT, false);
		}
		StoreDataCmd(ppseDGIs[dgiSize - 1].second.c_str(), STORE_DATA_END, false);
	}
	//个人化APP
	
	sw = SelectAppCmd(appParam.instanceAid.c_str(), resp);
	if (sw != 0x9000) {
		return sw;
	}
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
		return 3;
	}
	char encSessionKey[33] = { 0 };
	GetScureChannelSessionEncKey(encSessionKey);
	vector<pair<string, string>> appDGIs;
	cpsFile.GetValue("APP", appDGIs);
	string firstDGI = appDGIs.begin()->first;
	string lastDGI = appDGIs.rbegin()->first;
	for (auto item : appDGIs) {
		//判断是否为加密类型，并转换
		int storeType = STORE_DATA_PLANT;
		bool bReset = false;
		string data = item.second;
		if (IsNeedDecrypt(item.first)) {
			data = DecryptDGIData(encSessionKey, item.first, data);
			storeType = STORE_DATA_ENCRYPT;
		}
		if (item.first == firstDGI) {
			bReset = true;
		}
		if (item.first == lastDGI) {
			storeType = STORE_DATA_END;
		}
		if (data.length() / 2 < 0xFC) {
			StoreDataCmd(data.c_str(), storeType, bReset);
		}
		else {
			string data1 = data.substr(0, 0xDD * 2);
			StoreDataCmd(data.c_str(), storeType, bReset);
			int remaindDataLen = data.length() - 0xDD * 2;
			while (remaindDataLen > 0) {
				string storeData = data.substr(0xDD * 2, 0xDD * 2);
				StoreDataCmd(data.c_str(), storeType, bReset);
			}
		}
		
	}
	
	
	return true;
}

/******************************************************
* 通过CPS文件，完成卡片个人化
*******************************************************/
bool DoPersonlization(const char* szCpsFile, const char* iniConfigFile)
{
	IniConfig cpsFile;
	IniConfig cfgFile;

	//获取相关配置信息
	if (!cpsFile.Read(szCpsFile))
	{
		return false;
	}
	if (!cfgFile.Read(iniConfigFile))
	{
		return false;
	}
	OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel);
	auto DGIs = cpsFile.GetAllNodes();

	//构造TLV DGI分组
	for (auto dgiNode : DGIs)
	{
		string dgi = dgiNode.first;
		string dgiValue;
		//构造每一个TAG的TLV结构
		for (auto tv : dgiNode.second.m_Dict)
		{
			int len = tv.second.length() / 2;
			char szLen[5] = { 0 };
			if (len <= 0xFF)
			{
				sprintf_s(szLen, "%02X", len);
			}
			else {
				sprintf_s(szLen, "82%04X", len);
			}
			dgiValue += tv.first + szLen + tv.second;	//TLV
		}
		//构造完每一个TAG的TLV结构之后，再构造DGI的TLV结构
		int dgiLen = dgiValue.length() / 2;
		char szDgiLen[5] = { 0 };
		if (dgiLen <= 0xFF) {
			sprintf_s(szDgiLen, "%02X", dgiLen);
		}
		else {
			sprintf_s(szDgiLen, "82%04X", dgiLen);
		}
		string data = dgi + szDgiLen + dgiValue;	//整个DGI分组的TLV构造完成

		//发送Store Data命令，如果data域长度大于0xFF,则需要发送多次store data命令
		if (data.length() / 2 > 0xFF)
		{

		}
		else {
			//StoreDataCmd(data,)
		}
	}
	//		if (type == STORE_DATA_ENCRYPT)
	//		{   //处理特殊数据
	//			string encKey = m_pPCSC->GetSessionEncKey(); //m_pPCSC->GetEncKey();
	//			char szEncryptData[1024] = { 0 };
	//			if (v.first != "8000")
	//			{
	//				v.second += "8000000000000000";
	//			}
	//			Des3_ECB(szEncryptData, (char*)encKey.c_str(), (char*)v.second.c_str(), v.second.length());
	//			v.second = szEncryptData;
	//		}
	//		APDU_RESPONSE response;
	//		//存储个人化数据
	//		if (!pAPDU->StoreDataCmd(v.first, v.second, type, false, response))
	//		{
	//			return;     // 个人化PBOC失败
	//		}
	//	}
	//}
	//个人化任务成功
return false;
}

