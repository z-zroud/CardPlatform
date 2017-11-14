// CPS.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ICPS.h"
#include "IniConfig.h"
#include "../ApduCmd/IApdu.h"

string g_kmc;
int g_divMethod = 0;
int g_secureLevel = 0;


/******************************************************
* ����ͳһCPS�ļ�,���ڸ��˻�
* ����˵���� szDllName ���ڴ���DP�ļ���Dll����
* ����˵���� szFileName DP�ļ�·��
* ����ֵ�� DP�ļ�����ɹ�����true, ���򷵻�false
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



bool SetPersonlizationConfig(const char* kmc, int divMethod, int secureLevel)
{
	g_kmc = kmc;
	g_divMethod = divMethod;
	g_secureLevel = secureLevel;
}

/******************************************************
* ͨ��CPS�ļ�����ɿ�Ƭ���˻�
*******************************************************/
bool DoPersonlization(const char* szCpsFile, const char* iniConfigFile)
{
	IniConfig cpsFile;
	IniConfig cfgFile;

	//��ȡ���������Ϣ
	if (!cpsFile.Read(szCpsFile))
	{
		return false;
	}
	if (!cfgFile.Read(iniConfigFile))
	{
		return false;
	}
	OpenSecureChannel(g_kmc, g_divMethod, g_secureLevel);
	auto DGIs = cpsFile.GetAllNodes();

	//����TLV DGI����
	for (auto dgiNode : DGIs)
	{
		string dgi = dgiNode.first;
		string dgiValue;
		//����ÿһ��TAG��TLV�ṹ
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
		//������ÿһ��TAG��TLV�ṹ֮���ٹ���DGI��TLV�ṹ
		int dgiLen = dgiValue.length() / 2;
		char szDgiLen[5] = { 0 };
		if (dgiLen <= 0xFF) {
			sprintf_s(szDgiLen, "%02X", dgiLen);
		}
		else {
			sprintf_s(szDgiLen, "82%04X", dgiLen);
		}
		string data = dgi + szDgiLen + dgiValue;	//����DGI�����TLV�������

		//����Store Data������data�򳤶ȴ���0xFF,����Ҫ���Ͷ��store data����
		if (data.length() / 2 > 0xFF)
		{

		}
		else {
			//StoreDataCmd(data,)
		}
	}
	//bool bResult = SetSelectedApplication(aid.GetData());
	//if (!bResult)
	//	return;

	////ɾ����ǰӦ��
	//pAPDU->GetAppStatusCmd(status, reponse);
	//for (auto v : status)
	//{
	//	pAPDU->DeleteAppCmd(v.strAID);
	//}

	////���°�װӦ��
	//CDuiString instCfgPath = m_pPM->GetInstancePath() + _T("Configuration\\InstallParams\\") + instCfg;
	//CInstallCfg cfg(instCfgPath.GetData());
	//for (int i = INSTALL_APP; i < INSTALL_MAX; i++)
	//{
	//	INSTALL_PARAM param;
	//	cfg.GetInstallCfg((INSTALL_TYPE)i, param);
	//	pAPDU->InstallAppCmd(param.strExeLoadFileAID,
	//		param.strExeModuleAID,
	//		param.strApplicationAID,
	//		param.strPrivilege,
	//		param.strInstallParam,
	//		param.strToken,
	//		reponse);
	//}
	//IniParser ini;
	//if (!ini.Read(cpfFile.GetData()))
	//{
	//	return;     //��ȡCPS�ļ�ʧ��
	//}

	////���˻�PSE
	//string pseAid = cfg.GetApplicationAID(INSTALL_PSE);
	//if (!SetSelectedApplication(pseAid))
	//	return;
	//string pse1 = ini.GetValue("Store_PSE_1", "Store_PSE_1");
	//pse1 = "0101" + Base::GetDataHexLen(pse1) + pse1;

	//string pse2 = ini.GetValue("Store_PSE_2", "Store_PSE_2");
	//string pse2Len = Base::GetDataHexLen(pse2);
	//pse2 = _T("9102") + Base::Increase(pse2Len, 5) + _T("A5") + Base::Increase(pse2Len, 3) + _T("88") + _T("0101") + pse2;
	////if (!pAPDU->StorePSEData(pse1, STORE_DATA_COMMON, true))
	////{
	////    return;     //���˻�PSEʧ��
	////}
	////if (!pAPDU->StorePSEData(pse2, STORE_DATA_LAST, false))
	////{
	////    return;     //���˻�PSEʧ��
	////}

	////���˻�PPSE
	//string ppseAid = cfg.GetApplicationAID(INSTALL_PPSE);
	//if (!SetSelectedApplication(ppseAid))
	//	return;
	//string ppse = ini.GetValue("Store_PPSE", "Store_PPSE");
	//string ppseLen = Base::GetDataHexLen(ppse);
	//ppse = _T("9102") + Base::Increase(ppseLen, 5) + _T("A5") + Base::Increase(ppseLen, 3) + _T("BF0C") + ppseLen + ppse;
	////if (!pAPDU->StorePSEData(ppse, STORE_DATA_LAST, true))
	////{
	////    return;     // ���˻�PPSEʧ��
	////}

	////���˻�PBOC
	//string pbocAid = cfg.GetApplicationAID(INSTALL_APP);
	//if (!SetSelectedApplication(pbocAid))
	//	return;
	//auto vec = ConcatNodeWithSameSection(ini);
	//int nVecCount = 0;
	//for (auto v : vec)
	//{
	//	nVecCount++;
	//	if (v.first == "Store_PSE_1" || v.first == "Store_PSE_2" || v.first == "Store_PPSE")
	//	{
	//		continue;
	//	}
	//	else {

	//		string sDataLen;
	//		if (v.second.length() > 0xFE)
	//		{
	//			sDataLen = _T("81") + Base::GetDataHexLen(v.second);
	//		}
	//		else {
	//			sDataLen = Base::GetDataHexLen(v.second);
	//		}

	//		int nDGI = stoi(v.first, 0, 16);
	//		if (nDGI <= 0x0501) //С��0x0501��GDI������Ҫ���70ģ��
	//		{
	//			v.second = _T("70") + sDataLen + v.second;
	//		}

	//		STORE_DATA_TYPE type;
	//		if (vec.size() - 2 == nVecCount)    //PSE��PPSE���ļ�β����֮��
	//		{
	//			type = STORE_DATA_LAST;
	//		}
	//		else {
	//			type = GetStoreDataType(v.first);
	//		}

	//		if (type == STORE_DATA_ENCRYPT)
	//		{   //������������
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
	//		//�洢���˻�����
	//		if (!pAPDU->StoreDataCmd(v.first, v.second, type, false, response))
	//		{
	//			return;     // ���˻�PBOCʧ��
	//		}
	//	}
	//}
	//���˻�����ɹ�
return false;
}

