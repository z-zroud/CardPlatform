// CPS.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ICPS.h"
#include "IniConfig.h"
#include "Des0.h"
#include "../ApduCmd/IApdu.h"
#include "../Util/Tool.h"
#include "../Util/Log.h"
#include "../Util/rapidxml/rapidxml.hpp"
#include "../Util/rapidxml/rapidxml_utils.hpp"

using namespace rapidxml;

struct AppParams
{
    string appType;         //Ӧ������ PSE,PPSE,UICS,PBOC...
	string packageAid;      //��id
	string appletAid;       //applet id
	string instanceAid;     //instance id
	string privilege;
	string installParam;
	string token;
};

struct EncryptDGI
{
    string dgi;
    bool isPadding80;
};

/****************************************/
string		g_kmc;
string		g_isd;
int			g_divMethod = 0;
int			g_secureLevel = 0;
vector<AppParams> g_vecAppParams;
vector<EncryptDGI> g_vecEncryptDGI;
int         g_cpsCount = 1024;
char*       g_cpsFile[1024] = { 0 };
/****************************************/

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
	return pHandleDpFunc(szDpFile, szRuleFile, g_cpsFile, g_cpsCount);
}

void GetCpsFiles(char** cpsFiles, int& count)
{
    count = g_cpsCount;
    for (int i = 0; i < g_cpsCount; i++)
    {
        cpsFiles[i] = new char[1024];
        memset(cpsFiles[i], 0, 1024);
        strncpy_s(cpsFiles[i], 1024, g_cpsFile[i], strlen(g_cpsFile[i]));
    }
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

string EncryptDGIData(string encSessionKey, string dgi, string data, bool isPadding80)
{
    char decryptData[2048] = { 0 };
   
    if (isPadding80) 
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
    Des3_ECB(decryptData, (char*)encSessionKey.c_str(), (char*)data.c_str(), data.length());
	return decryptData;
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
            string data = personlizeDGIs[0].second;
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
                string data = DGI.second;
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

bool ParsePersoConfigFile(const char* configFile)
{
    g_vecAppParams.clear();    //ʹ��֮ǰɾ���ɵģ���ֹ������
    g_vecEncryptDGI.clear();
    //ʹ��rapidxml::file��ȡ�ļ�������  
    rapidxml::file<char> fdoc(configFile);

    rapidxml::xml_document<> doc;
    doc.parse<0>(fdoc.data());

    //��XML�ĵ���Ѱ�ҵ�һ���ڵ�  
    const rapidxml::xml_node<> *root = doc.first_node("PersonlizeConfiguration");
    if (NULL == root)
    {
        return false;
    }
    for (rapidxml::xml_node<> *node = root->first_node(); node != NULL; node = node->next_sibling())
    {
        string nodeName = node->name();
        if (nodeName == "App") 
        {
            AppParams app;
            app.packageAid = node->first_attribute("packageAid")->value();
            app.appletAid = node->first_attribute("appletAid")->value();
            app.instanceAid = node->first_attribute("instanceAid")->value();
            app.privilege = node->first_attribute("priviliage")->value();
            app.installParam = node->first_attribute("installParam")->value();
            app.token = node->first_attribute("token")->value();
            app.appType = node->first_attribute("type")->value();
            if (app.token.empty())
                app.token = "00";
            g_vecAppParams.push_back(app);
        }
        else if (nodeName == "Encrypt") 
        {
            EncryptDGI encryptDGI;
            encryptDGI.dgi = node->first_attribute("DGI")->value();
            string padding80 = node->first_attribute("padding80")->value();
            if (padding80 == "false")encryptDGI.isPadding80 = false;
            if (padding80 == "true")encryptDGI.isPadding80 = true;
            g_vecEncryptDGI.push_back(encryptDGI);
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

        //if (cpsNodes.size() < 3) {
        //    return false;
        //}
        int pbocDGIs = cpsNodes.size();
        int passPseCount = 0;
        for (int i = 0; i < pbocDGIs; i++)
        {
            if (cpsNodes[i].first == "PSE" || cpsNodes[i].first == "PPSE")
            {
                passPseCount++;
                continue;
            }
            int dataType = STORE_DATA_PLANT;
            bool bReset = false;
            string data;
            string dataContainedDGI;
            for (auto item : cpsNodes[i].second.m_collection) {
                data += item.second;    //ƴ��DGI����������Tag��ֵ
            }
            //Store Data�����Ƿ���Ҫ����
            for (auto encryptItem : g_vecEncryptDGI)
            {
                if (cpsNodes[i].first == encryptItem.dgi)
                {
                    char dekSessionKey[33];
                    GetScureChannelSessionDekKey(dekSessionKey);
                    data = EncryptDGIData(dekSessionKey, cpsNodes[i].first, data, encryptItem.isPadding80);
                    dataType = STORE_DATA_ENCRYPT;
                }
            }

            //�����Ƿ���Ҫ��70ģ��
            int sDgi = stoi(cpsNodes[i].first, 0, 16);
            if (sDgi <= 0x0B01) {   
                char noTemplateDataLen[5] = { 0 };
                Tool::GetBcdDataLen(data.c_str(), noTemplateDataLen, 5);
                if (data.length() > 0xFF * 2) {
                    data = "7082" + string(noTemplateDataLen) + data;
                }
                else if (data.length() > 0x80 * 2) {
                    data = "7081" + string(noTemplateDataLen) + data;
                }
                else {
                    data = "70" + string(noTemplateDataLen) + data;
                }
               
            }
            //��һ������ or ���������
            if (i == 0) {   
                bReset = true;
            }
            else if (i == pbocDGIs + passPseCount - 3) {
                dataType = STORE_DATA_END;
            }

            //���ݹ�������ֶ�store data����
            char dataLen[5] = { 0 };
            Tool::GetBcdDataLen(data.c_str(), dataLen, 5);
            if (data.length() > 0xFC * 2) {               
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
bool DoPersonlization(const char* szCpsFile, const char* szConfigFile)
{
	IniConfig cpsFile;
    Log->Info("==================== Begin personlization ===================");
	//��ȡ���������Ϣ
	if (!cpsFile.Read(szCpsFile)){
        Log->Error("Open cps file [%s] failed.", szCpsFile);
		return false;
	}
	if (!ParsePersoConfigFile(szConfigFile)){
        Log->Error("Parse install param configuration file [%s] failed.", szConfigFile);
		return false;
	}
    char resp[RESP_LEN] = { 0 };
    int sw = SelectAppCmd(g_isd.c_str(), resp);
    if (0x9000 != sw)
    {
        Log->Error("Select aid [%s] failed. sw = %d", g_isd.c_str(), sw);
        return false;
    }
	if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) 
    {
        Log->Error("Open secure channel failed, kmc[%s],div method[%d],secure level[%d]", g_kmc.c_str(), g_divMethod, g_secureLevel);
		return false;
	}

    //ɾ��ʵ��(ȫ��ɾ��)
    
    AppInfo apps[12] = { 0 };
    int appCount = 12;
    GetAppStatusCmd(apps, appCount);
    for (int i = 0; i < appCount; i++) 
    {
        Log->Info("Delete instance %s", apps[i].aid);
        sw = DeleteAppCmd(apps[i].aid);
        if (0x9000 != sw) 
        {
            Log->Error("Delete instance [%s] failed. sw = %d", apps[i].aid, sw);
            return false;
        }
    }

	//��װӦ��
    for (auto app : g_vecAppParams)
    {
        Log->Info("Install instance %s", app.instanceAid.c_str());
        sw = InstallAppCmd(app.packageAid.c_str(), app.appletAid.c_str(), app.instanceAid.c_str(), app.privilege.c_str(), app.installParam.c_str(), app.token.c_str());
        if (sw != 0x9000)
        {
            Log->Error("Install application failed. sw = [%d]", sw);
            return false;
        }           
    }

    //Ӧ�ø��˻�
    for (auto app : g_vecAppParams)
    {
        Log->Info("Starting personlize instance [%s] ...", app.instanceAid.c_str());
        char resp[RESP_LEN] = { 0 };
        if (0x9000 != SelectAppCmd(app.instanceAid.c_str(), resp)) 
        {
            Log->Error("Select instance [%s] failed.", app.instanceAid.c_str());
            return false;
        }
        if (!OpenSecureChannel(g_kmc.c_str(), g_divMethod, g_secureLevel)) 
        {
            Log->Error("Open secure channel failed, kmc[%s],div method[%d],secure level[%d]", g_kmc.c_str(), g_divMethod, g_secureLevel);
            return false;
        }
        if (app.appType == "PSE" || app.appType == "PPSE") 
        {
            if (!PersonlizePSE(cpsFile,app.appType)) 
            {
                Log->Error("Personlize instance [%s] failed.", app.instanceAid.c_str());
                return false;
            }
        }
        else 
        {
            if (!PersonlizePBOC(cpsFile)) 
            {
                Log->Error("Personlize instance [%s] failed.", app.instanceAid.c_str());
                return false;
            }
        }
        Log->Info("Personlize instance [%s] end.", app.instanceAid.c_str());
    }
    Log->Info("Personlize cps sucess.");

    return true;
}

