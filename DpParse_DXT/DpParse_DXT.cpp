// DpParse_DXT.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DpParse_DXT.h"

bool HandleDp(const char* szFileName, const char* szRuleFile)
{
	DXTDpParse parse;
	return parse.HandleDp(szFileName, szRuleFile);
}

bool DXTDpParse::HandleDp(const char* szFileName, const char* szRuleFile)
{
	ifstream dpFile;
	dpFile.open(szFileName, ios::in | ios::binary);
	if (!dpFile.is_open())
		return false;


	vector<string> vecDGIs = { "8000","8201","8202","8203" ,"8204","8205","8020","8302","8002" };

	char fileIdentity[12] = { 0 };
	GetBuffer(dpFile, fileIdentity, 10);

	char version[5] = { 0 };
	GetBuffer(dpFile, version, 4);

	char macKeyIdentity[13] = { 0 };
	GetBuffer(dpFile, macKeyIdentity, 12);

	string hash;
	GetBCDBuffer(dpFile, hash, 20);

	string macKey;
	GetBCDBuffer(dpFile, macKey, 16);

	string mac;
	GetBCDBuffer(dpFile, mac, 4);

	string batchNo;
	GetBCDBuffer(dpFile, batchNo, 4);

	char produceTime[15] = { 0 };
	GetBuffer(dpFile, produceTime, 14);

	char mark[130] = { 0 };
	GetBuffer(dpFile, mark, 128);

	string recordNo;
	GetBCDBuffer(dpFile, recordNo, 4);
    string i = "1";
    while (!dpFile.eof())
    {
        string recordLen;
        GetBCDBuffer(dpFile, recordLen, 4);

        char moduleName1[8] = { 0 };
        GetBuffer(dpFile, moduleName1, 7);
        char szModuleLen[8] = { 0 };
        streampos pos = GetBuffer(dpFile, szModuleLen, 7);

        int moduleLen = stoi(szModuleLen);

        dpFile.seekg(pos + (streampos)moduleLen);

        char moduleName2[8] = { 0 };
        GetBuffer(dpFile, moduleName2, 7);
        char szModuleLen2[8] = { 0 };
        streampos pos2 = GetBuffer(dpFile, szModuleLen2, 7);

        string appNum;
        GetBCDBuffer(dpFile, appNum, 1);
        int nAppNum = stoi(appNum);

        CPS_ITEM cpsItem;
        i.append("1");
        cpsItem.fileName = "F:\\CardPlatform\\bin\\huaxin3" + i + ".txt";
        for (int j = 0; j < nAppNum; j++)
        {
            string appDataLen;
            GetBCDBuffer(dpFile, appDataLen, 2);
            int dataLen = stoi(appDataLen);

            string aidLen;
            GetBCDBuffer(dpFile, aidLen, 1);
            int nAidLen = stoi(aidLen, 0, 16);
            string aid;
            GetBCDBuffer(dpFile, aid, nAidLen);

            char tkIdentity[13] = { 0 };
            GetBuffer(dpFile, tkIdentity, 12);

            string argorth;
            GetBCDBuffer(dpFile, argorth, 1);

            string logLen;
            GetBCDBuffer(dpFile, logLen, 2);
            int nLogLen = stoi(logLen, 0, 16);

            string log;
            GetBCDBuffer(dpFile, log, nLogLen);

            string icDataLen;
            GetBCDBuffer(dpFile, icDataLen, 2);
            int nICDataLen = stoi(icDataLen, 0, 16);

            vector<pair<string, string>> dgis;
            int i = 0;

            DGI_ITEM item;
            item.dgi = aid;
            while (i < nICDataLen)
            {
                string dgi;
                GetBCDBuffer(dpFile, dgi, 2);
                string dgiLen;
                GetBCDBuffer(dpFile, dgiLen, 2);
                int nDgiLen = stoi(dgiLen, 0, 16);
                string dgiValue;
                GetBCDBuffer(dpFile, dgiValue, nDgiLen);
                //printf("[%s]=%s\n", dgi.c_str(), dgiValue.c_str());
                for (auto encryptDGI : vecDGIs)
                {
                    if (dgi == encryptDGI) {
                        if (dgi == "8000" || dgi == "8020" || dgi == "8002") {
                            dgiValue = DecryptDGI("3E0D1C0E9DF146380B346EE3C2FD979D", dgiValue, false);
                        }
                        else {
                            dgiValue = DecryptDGI("3E0D1C0E9DF146380B346EE3C2FD979D", dgiValue, true);
                        }

                        break;
                    }
                }
                item.value.InsertItem(dgi, dgiValue);
                i += 4 + nDgiLen;
            }

            cpsItem.items.push_back(item);
            string dataMacLen;
            GetBCDBuffer(dpFile, dataMacLen, 1);
            string dataMac;
            int nDataMacLen = stoi(dataMacLen, 0, 16);
            GetBCDBuffer(dpFile, dataMac, nDataMacLen);
        }

        Save(cpsItem);
    }
	return true;
}

