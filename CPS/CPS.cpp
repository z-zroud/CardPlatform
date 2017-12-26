// CPS.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ICPS.h"
#include "IniConfig.h"
#include "Des0.h"
#include "../ApduCmd/IApdu.h"
#include "../Util/Tool.h"

/****************************************/
string		g_kmc;
string		g_isd;
int			g_divMethod = 0;
int			g_secureLevel = 0;
IniConfig   g_cfgFile;
/****************************************/

struct InstallParam
{
	string packageAid;
	string appletAid;
	string instanceAid;
	string privilege;
	string installParam;
	string token;
};

/***********************************************************************
* �������ļ��л�ȡ��װ����
************************************************************************/
bool GetInstallParam(IniConfig cfg, string appType, InstallParam& param)
{
	param.packageAid	= cfg.GetValue(appType, "PackageAid");
	param.appletAid		= cfg.GetValue(appType, "AppletAid");
	param.instanceAid	= cfg.GetValue(appType, "InstanceAid");
	param.privilege		= cfg.GetValue(appType, "Privilege");
	param.installParam	= cfg.GetValue(appType, "InstallParam");
	param.token			= cfg.GetValue(appType, "Token");

    if (param.token.empty()) {
        param.token == "00";
    }

    return param.packageAid.empty() ||
        param.appletAid.empty() ||
        param.instanceAid.empty() ||
        param.privilege.empty() ||
        param.installParam.empty();
}

/************************************************************
* ��װӦ�ã���װ�ɹ�����0x9000
*************************************************************/
int InstallApp(IniConfig cfg, string appType)
{
    InstallParam param;
    if (!GetInstallParam(cfg, appType, param)) {
        return 0x6982;
    }

	return InstallAppCmd(param.packageAid.c_str(), param.appletAid.c_str(), param.instanceAid.c_str(), param.privilege.c_str(), param.installParam.c_str(), param.token.c_str());
}

/******************************************************
* ����ͳһCPS�ļ�,���ڸ��˻�
* ����˵���� szDllName ���ڴ���DP�ļ���Dll����
* ����˵���� szFileName DP�ļ�·��
* ����ֵ�� DP�ļ�����ɹ�����true, ���򷵻�false
*******************************************************/
bool GenCpsFile(const char* szDllName, const char* szFuncName, const char* szDpFile, const char* szRuleFile)
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
	return pHandleDpFunc(szDpFile, szRuleFile);
}


void SetPersonlizationConfig(const char* isd,const char* kmc, int divMethod, int secureLevel)
{
	g_isd = isd;
	g_kmc = kmc;
	g_divMethod = divMethod;
	g_secureLevel = secureLevel;
}

bool ExistedInList(string value, vector<pair<string, string>> collection)
{
    for (auto item : collection)
    {
        if (item.second == value) {
            return true;
        }
    }
    return false;
}

string DecryptDGIData(string encSessionKey, string dgi, string data)
{
    char decryptData[2048] = { 0 };
    if (g_cfgFile.IsOpened())
    {
        vector<pair<string, string>> padding80DGIs;
        g_cfgFile.GetValue("Padding80", padding80DGIs);
        if (ExistedInList(dgi, padding80DGIs)) {
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
        Des3_ECB(decryptData, (char*)encSessionKey.c_str(), (char*)data.c_str(), data.length());
    }
	return decryptData;
}

bool IsNeedDecrypt(string dgi)
{
    if (g_cfgFile.IsOpened())
    {
        vector<pair<string, string>> encryptDGIs;
        g_cfgFile.GetValue("EncryptDGI", encryptDGIs);
        if (ExistedInList(dgi, encryptDGIs)) {
            return true;
        }
    }
	return false;
}

int DoPersonlizationWithOrderdDGI(const char* szCpsFile, const char* installCfgFile)
{
	//IniConfig cpsFile;
	//IniConfig cfgFile;

 //   g_cfgFile = installCfgFile;
	//if (!cfgFile.Read(g_cfgFile))
	//{
	//	return 1;
	//}
	////��ȡ���������Ϣ
	//if (!cpsFile.Read(szCpsFile))
	//{
	//	return 2;
	//}

	//char resp[RESP_LEN] = { 0 };
	//int sw = SelectAppCmd(g_isd.c_str(), resp);
	//if (sw != 0x9000) {
	//	return 3;
	//}
	//if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
	//	return 4;
	//}
	//
	////���˻�PSE
	//sw = SelectAppCmd(pseParam.instanceAid.c_str(), resp);
	//if (sw != 0x9000) {
	//	return sw;
	//}
	//if (!OpenSecureChannel(g_kmc.c_str(),g_divMethod,g_secureLevel)) {
	//	return 3;
	//}
	//vector<pair<string, string>> pseDGIs;
	//cpsFile.GetValue("PSE", pseDGIs);
	//int dgiSize = pseDGIs.size();
	//if (dgiSize == 1) {
	//	StoreDataCmd(pseDGIs[0].second.c_str(), STORE_DATA_END, true);
	//}
	//else {
	//	StoreDataCmd(pseDGIs[0].second.c_str(), STORE_DATA_PLANT, true);
	//	for (int i = 1; i < dgiSize - 1; i++) {
	//		StoreDataCmd(pseDGIs[i].second.c_str(), STORE_DATA_PLANT, false);
	//	}
	//	StoreDataCmd(pseDGIs[dgiSize - 1].second.c_str(), STORE_DATA_END, false);
	//}
	//
	////���˻�PPSE
	//sw = SelectAppCmd(ppseParam.instanceAid.c_str(), resp);
	//if (sw != 0x9000) {
	//	return sw;
	//}
	//if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
	//	return 3;
	//}

	//vector<pair<string, string>> ppseDGIs;
	//cpsFile.GetValue("PPSE", ppseDGIs);
	//dgiSize = ppseDGIs.size();
	//if (dgiSize == 1) {
	//	StoreDataCmd(ppseDGIs[0].second.c_str(), STORE_DATA_END, true);
	//}
	//else {
	//	StoreDataCmd(ppseDGIs[0].second.c_str(), STORE_DATA_PLANT, true);
	//	for (int i = 1; i < dgiSize - 1; i++) {
	//		StoreDataCmd(ppseDGIs[i].second.c_str(), STORE_DATA_PLANT, false);
	//	}
	//	StoreDataCmd(ppseDGIs[dgiSize - 1].second.c_str(), STORE_DATA_END, false);
	//}
	////���˻�APP
	//
	//sw = SelectAppCmd(appParam.instanceAid.c_str(), resp);
	//if (sw != 0x9000) {
	//	return sw;
	//}
	//if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
	//	return 3;
	//}
	//char encSessionKey[33] = { 0 };
	//GetScureChannelSessionEncKey(encSessionKey);
	//vector<pair<string, string>> appDGIs;
	//cpsFile.GetValue("APP", appDGIs);
	//string firstDGI = appDGIs.begin()->first;
	//string lastDGI = appDGIs.rbegin()->first;
	//for (auto item : appDGIs) {
	//	//�ж��Ƿ�Ϊ�������ͣ���ת��
	//	int storeType = STORE_DATA_PLANT;
	//	bool bReset = false;
	//	string data = item.second;
	//	if (IsNeedDecrypt(item.first)) {
	//		data = DecryptDGIData(encSessionKey, item.first, data);
	//		storeType = STORE_DATA_ENCRYPT;
	//	}
	//	if (item.first == firstDGI) {
	//		bReset = true;
	//	}
	//	if (item.first == lastDGI) {
	//		storeType = STORE_DATA_END;
	//	}
	//	if (data.length() / 2 < 0xFC) {
	//		StoreDataCmd(data.c_str(), storeType, bReset);
	//	}
	//	else {
	//		string data1 = data.substr(0, 0xDD * 2);
	//		StoreDataCmd(data.c_str(), storeType, bReset);
	//		int remaindDataLen = data.length() - 0xDD * 2;
	//		while (remaindDataLen > 0) {
	//			string storeData = data.substr(0xDD * 2, 0xDD * 2);
	//			StoreDataCmd(data.c_str(), storeType, bReset);
	//		}
	//	}
	//	
	//}
	//
	//
	return true;
}

bool PersonlizePSE(IniConfig cpsFile, string app)
{
    if (cpsFile.IsOpened())
    {
        vector<pair<string, string>> personlizeDGIs;
        cpsFile.GetValue(app, personlizeDGIs);
        if (personlizeDGIs.size() == 1) {   //���DGI����Ϊ1�����
            if (0x9000 != StoreDataCmd(personlizeDGIs[0].second.c_str(), STORE_DATA_END, true)) {
                return false;
            }
        }
        else {  //DGI��������1�����
            auto firstDGI = personlizeDGIs.begin();
            auto lastDGI = personlizeDGIs.rbegin();
            for (auto DGI : personlizeDGIs) {
                if (*firstDGI == DGI) {
                    if (0x9000 != StoreDataCmd(DGI.second.c_str(), STORE_DATA_PLANT, true)) {
                        return false;
                    }
                }
                else if (*lastDGI == DGI) {
                    if (0x9000 != StoreDataCmd(DGI.second.c_str(), STORE_DATA_END, false)) {
                        return false;
                    }
                }
                else {
                    if (0x9000 != StoreDataCmd(DGI.second.c_str(), STORE_DATA_PLANT, false)) {
                        return false;
                    }
                }
            }
        }

    }
    return true;
}

/********************************************************
* ���˻�PBOC��CPS��PSE��PPSE������ŵ�CPS����������
*********************************************************/
bool PersonlizePBOC(IniConfig cpsFile)
{
    if (cpsFile.IsOpened())
    {
        vector<pair<string, string>> personlizeDGIs;
        auto cpsNodes = cpsFile.GetAllNodes();

        if (cpsNodes.size() < 3) {
            return false;
        }
        int pbocDGIs = cpsNodes.size() - 2;

        for (int i = 0; i < pbocDGIs; i++)
        {
            int dataType = STORE_DATA_PLANT;
            bool bReset = false;
            string data;
            string dataContainedDGI;
            for (auto item : cpsNodes[i].second.m_collection) {
                data += item.second;    //ƴ��DGI����������Tag��ֵ
            }
            if (IsNeedDecrypt(cpsNodes[i].first)) { //�����Ƿ���Ҫ����
                char encSessionKey[33];
                GetScureChannelSessionAuthKey(encSessionKey);
                data = DecryptDGIData(encSessionKey, cpsNodes[i].first, data);
                dataType = STORE_DATA_ENCRYPT;
            }
            if (i == 0) {   //��һ������
                bReset = true;
            }
            else if (i == pbocDGIs - 1) {   //���������
                dataType = STORE_DATA_END;
            }

            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(data.c_str(), dataLen, 5);
            if (data.length() > 0xFC * 2) { //���ݹ�������ֶδ���              
                string firstDataBlock = data.substr(0, 0xFC * 2);
                dataContainedDGI = cpsNodes[i].first + dataLen + firstDataBlock;    //DGI + dataLen + data
                if (0x9000 != StoreDataCmd(dataContainedDGI.c_str(), dataType, bReset)) {
                    return false;
                }
                int count = 0xFC * 2;
                while (count < data.length()) {
                    string dataBlock = data.substr(count, 0xFC);
                    if (0x9000 != StoreDataCmd(dataBlock.c_str(), dataType, bReset)) {
                        return false;
                    }
                    count += dataBlock.length();
                }
            }
            else {
                dataContainedDGI = cpsNodes[i].first + dataLen + data;    //DGI + dataLen + data
                if (0x9000 != StoreDataCmd(dataContainedDGI.c_str(), dataType, bReset)) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}
/******************************************************
* ͨ��CPS�ļ�����ɿ�Ƭ���˻�
*******************************************************/
bool DoPersonlization(const char* szCpsFile, const char* iniConfigFile)
{
	IniConfig cpsFile;
    IniConfig cfgFile;

	//��ȡ���������Ϣ
	if (!cpsFile.Read(szCpsFile)){
		return false;
	}
	if (!cfgFile.Read(iniConfigFile)){
		return false;
	}
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
		return false;
	}
	//��װӦ��
    vector<pair<string, string>> InstallApps;
    cfgFile.GetValue("InstallApp", InstallApps);
    for (auto app : InstallApps) {
        if (0x9000 != InstallApp(cfgFile, app.second)) {
            return false;
        }
    }
    //Ӧ�ø��˻�
    for (auto app : InstallApps) {
        char resp[RESP_LEN] = { 0 };
        string aid = cfgFile.GetValue(app.second, "InstanceAid");
        if (0x9000 != SelectAppCmd(aid.c_str(), resp)) {
            return false;
        }
        if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
            return false;
        }
        if (app.second == "PSE" || app.second == "PPSE") {
            if (!PersonlizePSE(cpsFile,app.second)) {
                return false;
            }
        }
        else {
            if (!PersonlizePBOC(cpsFile)) {
                return false;
            }
        }
    }

    return true;
}

