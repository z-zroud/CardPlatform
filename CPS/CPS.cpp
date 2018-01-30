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

    return !param.packageAid.empty() &&
        !param.appletAid.empty() &&
        !param.instanceAid.empty() &&
        !param.privilege.empty() &&
        !param.installParam.empty();
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

string EncryptDGIData(string encSessionKey, string dgi, string data)
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

bool IsNeedEncrypt(string dgi)
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
	return 0;
}

bool PersonlizePSE(IniConfig cpsFile, string app)
{
    if (cpsFile.IsOpened())
    {
        vector<pair<string, string>> personlizeDGIs;
        cpsFile.GetValue(app, personlizeDGIs);
        if (personlizeDGIs.size() == 1) {   //���DGI����Ϊ1�����
            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(personlizeDGIs[0].second.c_str(), dataLen, 5);
            string data = personlizeDGIs[0].first + dataLen + personlizeDGIs[0].second;
            if (0x9000 != StoreDataCmd(data.c_str(), STORE_DATA_END, true)) {
                return false;
            }
        }
        else {  //DGI��������1�����
            auto firstDGI = personlizeDGIs.begin();
            auto lastDGI = personlizeDGIs.rbegin();
            for (auto DGI : personlizeDGIs) {
                char dataLen[5] = { 0 };
                Tool::GetBcdDataLen(DGI.second.c_str(), dataLen, 5);
                string data = DGI.first + dataLen + DGI.second;
                if (*firstDGI == DGI) {
                    if (0x9000 != StoreDataCmd(data.c_str(), STORE_DATA_PLANT, true)) {
                        return false;
                    }
                }
                else if (*lastDGI == DGI) {
                    if (0x9000 != StoreDataCmd(data.c_str(), STORE_DATA_END, false)) {
                        return false;
                    }
                }
                else {
                    if (0x9000 != StoreDataCmd(data.c_str(), STORE_DATA_PLANT, false)) {
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
            if (IsNeedEncrypt(cpsNodes[i].first)) { //�����Ƿ���Ҫ����
                char encSessionKey[33];
                GetScureChannelSessionEncKey(encSessionKey);
                data = EncryptDGIData(encSessionKey, cpsNodes[i].first, data);
                dataType = STORE_DATA_ENCRYPT;
            }
            int sDgi = stoi(cpsNodes[i].first, 0, 16);
            if (sDgi <= 0x0B01) {   //������Ҫ��70ģ��
                char noTemplateDataLen[5] = { 0 };
                Tool::GetBcdDataLen(data.c_str(), noTemplateDataLen, 5);
                if (data.length() > 0xFF * 2) {
                    data = "7082" + string(noTemplateDataLen) + data;
                }
                else if (data.length() > 0x79 * 2) {
                    data = "7081" + string(noTemplateDataLen) + data;
                }
                else {
                    data = "70" + string(noTemplateDataLen) + data;
                }
               
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
    //IniConfig cfgFile;

	//��ȡ���������Ϣ
	if (!cpsFile.Read(szCpsFile)){
		return false;
	}
	if (!g_cfgFile.Read(iniConfigFile)){
		return false;
	}
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) {
		return false;
	}
    //ɾ��ʵ��(ȫ��ɾ��)
    AppInfo apps[12] = { 0 };
    int appCount = 12;
    GetAppStatusCmd(apps, appCount);
    for (int i = 0; i < appCount; i++) {
        if (0x9000 != DeleteAppCmd(apps[i].aid)) {
            return false;
        }
    }
	//��װӦ��
    vector<pair<string, string>> InstallApps;
    g_cfgFile.GetValue("InstallApp", InstallApps);
    for (auto app : InstallApps) {
        if (0x9000 != InstallApp(g_cfgFile, app.second)) {
            return false;
        }
    }
    //Ӧ�ø��˻�
    for (auto app : InstallApps) {
        char resp[RESP_LEN] = { 0 };
        string aid = g_cfgFile.GetValue(app.second, "InstanceAid");
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

