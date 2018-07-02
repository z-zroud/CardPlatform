// DpParse_ZJ_TL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DpParse_ZJ_TL.h"
#include "../Util/Tool.h"
#include "../Util/Des0.h"
#define DGI_NUMBER	4
#define DGI_LEN		2

bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count)
{
	ZJTLDpParse parse;
    return parse.HandleDp(szFileName, ruleFile, cpsFile, count);
}

string ZJTLDpParse::GetDpMark(ifstream& dpFile)
{
    char mic[8] = { 0 };
    GetBuffer(dpFile, mic, 7);
    
    return mic;
}

void ZJTLDpParse::ParsePSE(ifstream& dpFile, CPS& cpsItem, string dgiName)
{
    if ("86" != GetDGIStartMark(dpFile)) {
        return ;
    }
    int dgiLen;
    GetLenTypeBuffer(dpFile, dgiLen, 1);
    if (dgiLen == 0x81) {	//DGI数据为2字节
        GetLenTypeBuffer(dpFile, dgiLen, 2);
    }
    DGI dgiItem;
    string value;
    GetBCDBuffer(dpFile, value, dgiLen);
    string tag;
    if (dgiName.substr(0, 3) == "PSE")
    {
        dgiItem.dgi = "PSE";
        tag = dgiName.substr(4);
    }
    else {
        dgiItem.dgi = "PPSE";   //中信的数据都包含有6F模板，一般都固定值为42字节，去掉
        tag = dgiName.substr(5);
        char dataLen[3] = { 0 };
        Tool::GetBcdDataLen(value.substr(42).c_str(), dataLen, 3);
        value = "9102" + string(dataLen) + value.substr(42);
    }
    
    dgiItem.value.AddItem(tag, value);
    //cpsItem.items.push_back(dgiItem);
    cpsItem.AddDGI(dgiItem);
}

string ZJTLDpParse::GetDGIStartMark(ifstream& dpFile)
{
    string dgiStartMark;
    GetBCDBuffer(dpFile, dgiStartMark, 1);
    
    return dgiStartMark;
}

int ZJTLDpParse::GetDGIDataLen(ifstream& dpFile,string &dgiName)
{
    int dgiLen;
    GetLenTypeBuffer(dpFile, dgiLen, 1); //获取整个DGI数据块的长度
    if (dgiLen == 0x81)
    {	//DGI数据为2字节
        GetLenTypeBuffer(dpFile, dgiLen, 1);
    }
    else if (dgiLen == 0x82) {
        GetLenTypeBuffer(dpFile, dgiLen, 2);
    }

    string dgi;
    GetBCDBuffer(dpFile, dgi, 2);
    if (dgi != dgiName)
    {
        dgiName = dgi;  //这里要打印警告日志，列表和实际DGI顺序不一致
    }
    int dgiDataLen;
    GetLenTypeBuffer(dpFile, dgiDataLen, 1); //DGI数据的长度
    unsigned short sDgi = stoi(dgi, 0, 16);

    if (sDgi < 0x0b01)		// 读取记录模板(该模板仅当DGI序号小于0x0B01时出现)
    {
        string templateTag;
        GetBCDBuffer(dpFile, templateTag, 1);
        if (templateTag != "70") {
            return false;
        }
        GetLenTypeBuffer(dpFile, dgiDataLen, 1);
        if (dgiDataLen == 0x81) {	//DGI数据为2字节
            GetLenTypeBuffer(dpFile, dgiDataLen, 1);
        }
        else if (dgiDataLen == 0x82) {
            GetLenTypeBuffer(dpFile, dgiDataLen, 2);
        }
    }

    return dgiDataLen;
}

bool ZJTLDpParse::HandleDp(const char* fileName, const char* ruleFile, char** cpsFile, int& count)
{
    //Test magistrap
    //ifstream dpFile1;
    //if (!OpenDpFile(fileName, dpFile1)) {
    //    return false;
    //}
    //long dpFileSize1 = GetFileSize(dpFile1);
    //ofstream outputFile("D:\\tet.txt");
    //while (dpFile1.tellg() < dpFileSize1)
    //{
    //    string magistrpBuff;
    //    GetBCDBuffer(dpFile1, magistrpBuff, 176);
    //    int unit = 16;
    //    string strResult;
    //    string tk = "3D370479B77FFCCCC6B89CC6FFDD570E";
    //    for (int i = 0; i < 352; i += unit)
    //    {
    //        char* pOutput = new char[unit + 1];
    //        memset(pOutput, 0, unit + 1);
    //        _Des3(pOutput, (char*)tk.c_str(), (char*)magistrpBuff.substr(i, unit).c_str());
    //        strResult += string(pOutput);
    //    }
    //    outputFile << strResult << endl;
    //    GetBCDBuffer(dpFile1, magistrpBuff, 1);
    //}
    //outputFile.close();
    //end Test
	ifstream dpFile;
	if (!OpenDpFile(fileName, dpFile)) {
		return false;
	}
    long dpFileSize = GetFileSize(dpFile);
    if ("CITICDP" != GetDpMark(dpFile)){
        return false;
    }
    int followedDataLen;
    GetLenTypeBuffer(dpFile, followedDataLen, 8);

	ReadDGIName(dpFile);		//读取DGI分组

	while (dpFile.tellg() < dpFileSize)		//遍历后续数据，每一次遍历解析一个卡片数据
	{
		string cardSeqNo;
		GetBCDBuffer(dpFile, cardSeqNo, 4);	//读取卡片序列号		
		int oneCardDataLen;
		GetLenTypeBuffer(dpFile, oneCardDataLen, 2); //读取该卡片个人化数据内容总长度

		CPS cpsItem;
		for (unsigned int i = 0; i < m_vecDGI.size(); i++)	//解析每张卡片数据
		{
            DGI dgiItem;
			dgiItem.dgi = m_vecDGI[i];
			if (dgiItem.dgi.substr(0, 3) == "PSE" ||
                dgiItem.dgi.substr(0,4) == "PPSE")	//对于DGIF001和PSE/PPSE数据，需要特殊处理
			{
                ParsePSE(dpFile, cpsItem, dgiItem.dgi);
				continue;
			}
            if (dgiItem.dgi.substr(0, 3) == "DGI") {    //删除DGI中的"DGI"字符串
                dgiItem.dgi = dgiItem.dgi.substr(3);
            }
            if (GetDGIStartMark(dpFile) != "86") {
                return false;
            }
			
            int dgiDataLen = GetDGIDataLen(dpFile, dgiItem.dgi);
            string buffer;
            GetBCDBuffer(dpFile, buffer, dgiDataLen);
			//判断是否仅包含数据内容，不是标准的TLV结构
			if (IsTlvStruct((char*)buffer.c_str(), dgiDataLen * 2)) {
				ParseTLV((char*)buffer.c_str(), dgiDataLen * 2, dgiItem.value);
			}
			else {	

				dgiItem.value.AddItem(dgiItem.dgi, buffer);
			}
			cpsItem.dgis.push_back(dgiItem);
		}
		//int pos = string(fileName).find_last_of('\\');
		//string path = string(fileName).substr(0, pos + 1);
        cpsItem.fileName = Tool::GetDirectory(fileName) + "conv\\" +  GetAccount(cpsItem) + ".txt";

        if (ruleFile) { //处理规则
            HandleRule(ruleFile, cpsItem);
        }

		Save(cpsItem);
	}

	return true;
}

/********************************************************************
* 功能：读取DGI分组名称
*********************************************************************/
void ZJTLDpParse::ReadDGIName(ifstream &dpFile)
{
	int dgiNum;
	GetLenTypeBuffer(dpFile, dgiNum, 4);

	for (int i = 0; i < dgiNum; i++)
	{
		int dgiLen;
		GetLenTypeBuffer(dpFile, dgiLen, 2);
		char dgiName[12] = { 0 };
		GetBuffer(dpFile, dgiName, dgiLen);
		m_vecDGI.push_back(dgiName);
	}
}



