#include "stdafx.h"
#include "EMV.h"
#include "Util\KeyGenerator.h"
#include "Util\Log.h"
#include "Util\Des0.h"
#include "Util\SHA1.h"
#include "Util\SM.hpp"
#include "Util\Converter.h"
#include "..\Terminal.h"

//将字符串型十六进制转为整形
int ctoi(char c)
{
	switch (toupper(c))
	{
	case '0':		return 0;
	case '1':		return 1;
	case '2':		return 2;
	case '3':		return 3;
	case '4':		return 4;
	case '5':		return 5;
	case '6':		return 6;
	case '7':		return 7;
	case '8':		return 8;
	case '9':		return 9;
	case 'A':		return 10;
	case 'B':		return 11;
	case 'C':		return 12;
	case 'D':		return 13;
	case 'E':		return 14;
	case 'F':		return 15;
	default:		return -1;
	}

	return -1;
}


EMV::EMV(IPCSC* reader)
{
	m_pReader = reader;
	m_pAPDU = reader->GetAPDU();
	m_parser = new TLVParaser();
	m_IsOnlineAuthSucessed = false;
}

//主要实例化那些不需要使用APDU指令的对象，例如脱机数据认证
EMV::EMV()
{
    m_pReader = NULL;
    m_pAPDU = NULL;
    m_parser = new TLVParaser();
    m_IsOnlineAuthSucessed = false;
}

EMV::~EMV()
{
	if (m_parser)
		delete m_parser;
}

string EMV::GenTransDate()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szTransDate[32] = { 0 };	//交易日期
	sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wYear, st.wMonth, st.wDay);

	return string(szTransDate).substr(2);
}

string EMV::GenTransTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szTransDate[32] = { 0 };	//交易日期
	sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);

	return string(szTransDate);
}

string EMV::GetResult(unsigned char SW1, unsigned char SW2)
{
	char temp[5] = { 0 };
	sprintf_s(temp, 5, "%02X%02X", SW1, SW2);

	return string(temp);
}

//格式化输出响应数据
void EMV::FormatOutputResponse(PBCD_TLV entities, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (entities[i].isTemplate)
		{
			FormatOutputResponse(entities[i].subTLVEntity, entities[i].subTLVnum);
		}
		else {
			Log->Info("[%s]=%s", entities[i].Tag, entities[i].Value);
		}
	}
}


//查找指定Tag值
string EMV::FindTagValue(const string tag, PBCD_TLV entities, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (entities[i].isTemplate)
		{
			return FindTagValue(tag, entities[i].subTLVEntity, entities[i].subTLVnum);
		}
        else {
            if (tag == string((char*)entities[i].Tag))
            {
                return string((char*)entities[i].Value);
            }
        }
    }

    return "";
}

//查找已获取的Tag值
string EMV::FindTagValue(const string tag)
{
    for (auto s : m_Tags)
    {
        if (tag == s.first)
            return s.second;
    }

    return "";
}

//保存tag值
void EMV::SaveTag(PBCD_TLV entites, int num)
{
    for (int i = 0; i < num; i++)
    {
        if (entites[i].isTemplate)
        {
            SaveTag(entites[i].subTLVEntity, entites[i].subTLVnum);
        }
        else
        {
            auto it = m_Tags.find((char*)entites[i].Tag);
            if (it != m_Tags.end())
            {
                it->second = string((char*)entites[i].Value);
            }
            else {
                m_Tags.insert(make_pair<string, string>(string((char*)entites[i].Tag), string((char*)entites[i].Value)));
            }
        }
    }
}

void EMV::SaveTag(string tag, string value)
{
    auto it = m_Tags.find(tag);
    if (it != m_Tags.end())
    {
        it->second = value;
    }
    else {
        m_Tags.insert(make_pair<string, string>(string(tag), string(value)));
    }
}

void EMV::ParseAFL(string strAFL, vector<AFL> &output)
{
    if (strAFL.length() % 8 != 0)
    {
        return;		//每个AFL占用4个字节，也就是8个BCD码
    }
    //int aflCount = AFL.length() / 8;
    for (unsigned int i = 0; i < strAFL.length(); i += 8)
    {
        AFL afl;

        afl.SFI = ((ctoi(strAFL[i]) & 0x0F) << 1) + (ctoi(strAFL[i + 1]) && 0x08);
        string strStartRecord = strAFL.substr(i + 2, 2);
        string strEndRecord = strAFL.substr(i + 4, 2);
        string strNeedValidate = strAFL.substr(i + 6, 2);
        int nStartRecord = stoi(strStartRecord, 0, 16);
        int nEndRecord = stoi(strEndRecord, 0, 16);
        int nNeedValidate = stoi(strNeedValidate, 0, 16);

        for (int k = nStartRecord; k <= nEndRecord; k++)
        {
            afl.nReadRecord = k;
            if (nNeedValidate >= k)
            {
                afl.bNeedValidate = true;
            }
            else {
                afl.bNeedValidate = false;
            }

            output.push_back(afl);
        }
    }
}

//PSE选择
string EMV::SelectPSE(APP_TYPE appType)
{
    APDU_RESPONSE response;

    memset(&response, 0, sizeof(response));
    if(appType == APP_PSE)  //执行PSE应用
    {
        if (!m_pAPDU->SelectPSECommand(response))
        {
            Log->Error("执行 select PSE 失败!");
            return "";
        }
    }
    else if (appType == APP_PPSE)   //执行PPSE应用
    {
        if (!m_pAPDU->SelectPPSECommand(response))
        {
            Log->Error("执行 select PSE 失败!");
            return "";
        }
    }

	if (response.SW1 == 0x90 && response.SW2 == 0x00)
	{
		BCD_TLV entities[32] = { 0 };
		unsigned int entitiesCount = 0;

		//解析响应数据
		m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entities, entitiesCount);
		
		//格式化输出响应数据
		FormatOutputResponse(entities, entitiesCount);
		SaveTag(entities, entitiesCount);
        if (appType == APP_PSE)
        {
            //根据FCI中的目录文件SFI读目录文件
            string SFI = FindTagValue("88", entities, entitiesCount);
            if (SFI.empty())
            {
                Log->Error("找不到短文件标识符");
                return "";
            }

            //根据目录文件读指定目录
            bool needContinue = true;
            int recordNumber = 1;
            while (needContinue)
            {
                BCD_TLV dirEntites[32] = { 0 };
                unsigned int dirEntitiesCount = 0;
                memset(&response, 0, sizeof(response));
                char szRecordNumber[3] = { 0 };
                sprintf_s(szRecordNumber, "%02X", recordNumber);
                if (!m_pAPDU->ReadRecordCommand(SFI, szRecordNumber, response))
                {
                    Log->Error("执行 读命令失败");
                    return "";
                }
                if (response.SW1 == 0x6A && response.SW2 == 0x83)
                {
                    break;
                }
                recordNumber++;
                m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), dirEntites, dirEntitiesCount);

                //格式化输出响应数据
                FormatOutputResponse(dirEntites, dirEntitiesCount);
                SaveTag(dirEntites, dirEntitiesCount);
            }
        }
		//返回AID
		return FindTagValue("4F");
	}
	else if (response.SW1 == 0x6A && response.SW2 == 0x81)	//返回6A81
	{
		Log->Error("返回值[%s]: 卡片锁定或命令不支持! 终止交易", GetResult(response.SW1, response.SW2));
		return false;
	}
	else if (response.SW1 == 0x6A && response.SW2 == 0x82) //返回6A82
	{
		Log->Warning("返回值[%s]: 卡片不存在PSE，可能卡片未执行个人化", GetResult(response.SW1, response.SW2));
	}
	else if (response.SW1 == 0x62 && response.SW2 == 0x83) //返回6283
	{
		Log->Warning("返回值[%s]: PSE锁定");
	}
	else {	
		Log->Error("Select PSE unknown error");
	}

	return "";
}


//AID 选择
bool EMV::SelectAID(const string &strAID)
{
	APDU_RESPONSE response;
	
	memset(&response, 0, sizeof(response));
	if (!m_pAPDU->SelectApplicationCommand(strAID, response))
		return false;

	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;

	m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//格式化输出响应数据
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	return true;
}


//应用选择
bool EMV::SelectApplication(APP_TYPE type, string app)
{	
	Log->Info("======================== 选择应用 开始 =================================");
    switch (type)
    {
    case APP_PSE:
        app = SelectPSE(APP_PSE);
        break;
    case APP_PPSE:
        app = SelectPSE(APP_PPSE);
        break;
    }

    return SelectAID(app);
}

//应用初始化
bool EMV::InitilizeApplication(ENCRYPT_TYPE type)
{
	Log->Info("======================== 应用初始化 开始 =================================");
	string PDOL = FindTagValue("9F38");
	if (PDOL.empty())
		return false;

	BCD_TL tlEntities[32] = { 0 };
	unsigned int tlEntitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)PDOL.c_str(), PDOL.length(), tlEntities, tlEntitiesCount);

	//构造PDOL响应数据字段
	string data;
	for (unsigned int i = 0; i < tlEntitiesCount; i++)
	{
        if (string((char*)tlEntities[i].tag) == "DF69")
        {
            if (type == ENCRYPT_DES)
            {
                data += "00";
            }
            else {
                data += "01";
            }
        }
        else {
            string value = CTerminal::GetTerminalData((char*)tlEntities[i].tag);
            if (value.empty())
            {
                return false;
            }
            data += value;
        }
	}

	//发送GPO命令
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
	if (!m_pAPDU->GPOCommand(data, response))
	{
		return false;
	}
	//解析响应数据
	string responseData = string(response.data);
	string AIP = responseData.substr(4, 4);
	string AFL = responseData.substr(8);

	//格式化输出响应数据
	Log->Info("[82]=%s", AIP.c_str());
	Log->Info("[94]=%s", AFL.c_str());

	SaveTag("82", AIP);
	SaveTag("94", AFL);

	return true;
}

//读应用数据
bool EMV::ReadApplicationData()
{
	Log->Info("======================== 读应用数据 开始 =================================");
	//获取AFL
	string strAFL = FindTagValue("94");
	if (strAFL.empty())
	{
		return false;
	}
	//解析AFL
	vector<AFL> vecAFL;
	ParseAFL(strAFL, vecAFL);
	int i = 1;
	for (auto v : vecAFL)
	{
		//读取每条AFL记录
		char szSFI[3] = { 0 };
		sprintf_s(szSFI, "%02X", v.SFI);


		APDU_RESPONSE response;
		memset(&response, 0, sizeof(response));

		char szReadRecord[3] = { 0 };
		sprintf_s(szReadRecord, "%02X", v.nReadRecord);

		if (!m_pAPDU->ReadRecordCommand(szSFI, szReadRecord, response))
		{
			return false;
		}

		if (v.bNeedValidate)
		{
			//保存静态签名数据
			string sigData = response.data;

			//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
			if (ctoi(sigData[2]) & 0x08)
			{
				//最高位1
				unsigned int lengthSize = 2 * ((sigData[2] & 0x07) * 8 + (sigData[3] & 0x0f));
				m_staticApplicationData += sigData.substr(lengthSize + 4);

			}
			else
			{
				m_staticApplicationData += sigData.substr(4);
			}
		}

		//解析tag
		BCD_TLV entites[32] = { 0 };
		unsigned int entitiesCount = 0;

		m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);
		FormatOutputResponse(entites, entitiesCount);
		SaveTag(entites, entitiesCount);
	}

	//获取标签列表
	char* tagList[] = {
		"9F51","9F52","9F53","9F54","9F56","9F57","9F58","9F59","9F5C","9F5D","9F72","9F75",
		"9F76","9F4F","DF4F","9F17","9F79","9F77","9F78","DF62","DF63","DF77","DF79","9F6D",
		"9F36","9F13","9F68","9F6B","DF20","DF21","DF22","DF23","DF25","DF26","DF27","DF50"
	};
	for (int i = 0; i < sizeof(tagList) / sizeof(char*); i++)
	{
		string value = ReadTag(tagList[i]);
		BCD_TLV entites[32] = { 0 };
		unsigned int entitiesCount = 0;
		m_parser->TLVConstruct((unsigned char*)value.c_str(), value.length(), entites, entitiesCount);
		if (entitiesCount == 1)
		{
			Log->Info("[%s]=%s", tagList[i], entites[0].Value);
			//m_Tags.insert(make_pair<string, string>(string(tagList[i]), string((char*)entites[0].Value)));
			SaveTag(tagList[i], (char*)entites[0].Value);
		}
		
	}

	return true;
}

//读取标签数据
string EMV::ReadTag(const string tag)
{
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));

	m_pAPDU->GetTag(tag, response);
	if (response.SW1 == 0x90 && response.SW2 == 0x00)
	{
		return string(response.data);
	}

	return "";
}

//校验静态应用数据
bool EMV::ValidateStaticApplicationData(string issuerPublicKey, ENCRYPT_TYPE encryptType)
{
	string signedStaticAppData = FindTagValue("93");
	char recoveryData[2046] = { 0 };

    Log->Info("Issuser Public Key: %s", issuerPublicKey.c_str());
    Log->Info("static app data: %s", signedStaticAppData.c_str());

    if (encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
    {
        //解密签名的静态应用数据
        RSA_STD((char*)issuerPublicKey.c_str(), "03", (char*)signedStaticAppData.c_str(), recoveryData);
        int recoveryDataLen = strlen(recoveryData);
        string strRecoveryData(recoveryData);

        if (strRecoveryData.substr(0, 4) != "6A03" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
        {//如果恢复数据的开头不是"6A02"并且结尾不是"BC",认证失败
            return false;
        }
        string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);

        //如果静态数据认证标签列表存在，并且其包含非“82”的标签，那么静态数据认证失败
        string sigDataList = FindTagValue("9F4A");
        if (!sigDataList.empty() && sigDataList != "82")
        {
            return false;
        }

        string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + m_staticApplicationData + FindTagValue("82");
        Log->Info("Hash Input: [%s]", hashDataInput.c_str());

        CSHA1 sha1;
        string hashResult = sha1.GetBCDHash(hashDataInput);
        if (hashResult == hashData)
        {
            return true;
        }
    }
    else {
        PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
        if (DllPBOC_SM2_Verify)
        {
            string signedResult = signedStaticAppData.substr(6);
            string needValidateData = signedStaticAppData.substr(0,6) + m_staticApplicationData + FindTagValue("82");
            int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey.c_str(), (char*)needValidateData.c_str(), (char*)signedResult.c_str());
            if (ret == SM_OK)
            {
                return true;
            }
        }
    }

	return false;
}

bool EMV::ValidateDynamicData(string ICCPublicKey, ENCRYPT_TYPE encryptType)
{
    string terminalData;
    string DDOL = FindTagValue("9F49");
    BCD_TL tlEntity[10] = { 0 };
    unsigned int entitiesCount = 0;
    m_parser->TLVConstruct((unsigned char*)DDOL.c_str(), DDOL.length(), tlEntity, entitiesCount);
    for (unsigned int i = 0; i < entitiesCount; i++)
    {
        terminalData += CTerminal::GetTerminalData((char*)tlEntity[i].tag);
    }
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));

	if (!m_pAPDU->InternalAuthCommand(terminalData, response))
	{
		return false;
	}

	//解析tag
	string strResponse = response.data;
    if (strResponse.empty())
    {
        Log->Error("动态签名失败,无法获取tag 9F4B");
        return false;
    }
    //判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
    if (ctoi(strResponse[2]) & 0x08)
    {
        //最高位1
        unsigned int lengthSize = 2 * ((strResponse[2] & 0x07) * 8 + (strResponse[3] & 0x0f));
        SaveTag("9F4B", strResponse.substr(lengthSize + 4));
        
    }
    else
    {
        SaveTag("9F4B", strResponse.substr(4));
    }

	string signedData = FindTagValue("9F4B");
    Log->Debug("Dynamic data=[%s]", signedData.c_str());
	char recoveryData[2046] = { 0 };
    if (encryptType == ENCRYPT_DES)
    {
        //从动态签名数据中获取恢复数据
        RSA_STD((char*)ICCPublicKey.c_str(), "03", (char*)signedData.c_str(), recoveryData);
        string strRecoveryData = recoveryData;
        int recoveryDataLen = strlen(recoveryData);
        if (recoveryDataLen == 0)
        {
            Log->Error("动态签名失败，无法获取恢复数据");
            return false;
        }
        string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);
        string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + terminalData;

        CSHA1 sha1;
        string hashResult = sha1.GetBCDHash(hashDataInput);
        if (hashResult == hashData)
        {
            return true;
        }
    }
    else {
        PDllPBOC_SM2_Verify DllPBOC_SM2_Verify = GetSMFunc<PDllPBOC_SM2_Verify>("dllPBOC_SM2_Verify");
        if (DllPBOC_SM2_Verify)
        {
            if (signedData.substr(0, 2) != "15")
            {
                return false;
            }
            int iccDynamicDataLen = stoi(signedData.substr(2, 2), 0, 16) * 2;
            string iccDynamicData = signedData.substr(4, iccDynamicDataLen);
            string signedResult = signedData.substr(4 + iccDynamicDataLen);
            string needValidateData = signedData.substr(0, 4 + iccDynamicDataLen) + terminalData;
            int ret = DllPBOC_SM2_Verify((char*)ICCPublicKey.c_str(), (char*)needValidateData.c_str(), (char*)signedResult.c_str());
            if (ret == SM_OK)
            {
                return true;
            }
        }
    }

	return false;
}

//脱机数据认证
bool EMV::OfflineDataAuth()
{
	Log->Info("======================== 脱机数据认证 开始 =================================");
	//静态数据认证
    string issuerPublicCert = FindTagValue("90");   //发卡行公钥证书
    string caIndex = FindTagValue("8F");    //CA公钥索引 PKI
    string issuerExponent = FindTagValue("9F32");   //发卡行公钥指数
    string ipkRemainder = FindTagValue("92");   //发卡行公钥余项
    string rid = FindTagValue("4F").substr(0,10);   //RID    
   
    IKeyGenerator *kg = new KeyGenerator();
    string caPublicKey = kg->GenCAPublicKey(caIndex, rid);

    string issuerPublicKey;
    if (m_encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
    {
        issuerPublicKey = kg->GenDesIssuerPublicKey(caPublicKey, issuerPublicCert, ipkRemainder, issuerExponent);
    }
    else {
        issuerPublicKey = kg->GenSMIssuerPublcKey(caPublicKey, issuerPublicCert);
    }

	if (!ValidateStaticApplicationData(issuerPublicKey, m_encryptType))
	{
		Log->Info("SDA validate failed.");
		return false;
	}

	Log->Info("SDA validate success.");


    if (m_authType == AUTHENCATE_TYPE::AUTH_DDA)
    {
        //动态数据认证	
        string iccPublicCert = FindTagValue("9F46");    //ICC公钥证书       
        string signedData = m_staticApplicationData + FindTagValue("82");   //签名数据
        string ICCPublicKey;
        if (m_encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
        {
            string iccRemainder = FindTagValue("9F48"); //ICC公钥余项
            string iccExponent = FindTagValue("9F47");  //ICC公钥指数
            ICCPublicKey = kg->GenDesICCpublicKey(issuerPublicKey, iccPublicCert, iccRemainder, signedData, issuerExponent);
        }
        else {
            ICCPublicKey = kg->GenSMICCpublicKey(issuerPublicKey, iccPublicCert, signedData);
        }

        Log->Debug("ICC public key=[%s]", ICCPublicKey.c_str());

        if (!ValidateDynamicData(ICCPublicKey, m_encryptType))
        {
            Log->Info("DDA validate failed.");
            return false;
        }
        Log->Info("DDA validate success.");
        return true;
    }
    else if (m_authType == AUTHENCATE_TYPE::AUTH_CDA)
    {
        //CDA认证
        //...
    }


	return true;
}

//日期比较
bool EMV::CompareDate(string first, string second)
{
	if (first.length() != 6 || second.length() != 6)
	{
		return false;
	}
	int nFirst = stoi(first, 0);
	int nSecond = stoi(second, 0);

	return nFirst >= nSecond;
}


//检查卡片AUC信息
void EMV::ShowCardAUC(string AUC)
{
	int nAUC1 = stoi(AUC.substr(0, 2), 0, 16);
	int nAUC2 = stoi(AUC.substr(2, 2), 0, 16);

	Log->Info("AUC: %s", AUC.c_str());
	if (nAUC1 & 0x80) { Log->Info("国内现金交易有效"); }
	if (nAUC1 & 0x40) { Log->Info("国际现金交易有效"); }
	if (nAUC1 & 0x20) { Log->Info("国内商品有效"); }
	if (nAUC1 & 0x10) { Log->Info("国际商品有效"); }
	if (nAUC1 & 0x08) { Log->Info("国内服务有效"); }
	if (nAUC1 & 0x04) { Log->Info("国际服务有效"); }
	if (nAUC1 & 0x02) { Log->Info("ATM有效"); }
	if (nAUC1 & 0x01) { Log->Info("除ATM外的终端有效"); }
	if (nAUC1 & 0x80) { Log->Info("允许国内返现"); }
	if (nAUC1 & 0x40) { Log->Info("允许国际返现"); }
}

/**************************************************************
* 处理限制主要包括以下方面的检查，并将检查结果记录到TVR中
*	1.应用生效日期检查
*	2.应用失效日期检查
*	3.应用版本号检查
*	4.应用用途控制检查AUC
*	5.发卡行国家代码检查
***************************************************************/
bool EMV::HandleLimitation()
{
	Log->Info("======================== 处理限制 开始 =================================");
	//检查生效日期
	string effectDate = FindTagValue("5F25");
	string transDate = GenTransDate();
	if (!CompareDate(transDate, effectDate))
	{
		Log->Error("生效日期错误: 生效日期:[%s], 交易日期[%s]", effectDate.c_str(), transDate.c_str());
		m_tvr.ApplicationNotEffect = true;
	}
	else {
		Log->Info("生效日期验证成功");
	}
	
	//检查失效日期
	string expireDate = FindTagValue("5F24");
	if (!CompareDate(expireDate, transDate))
	{
		Log->Error("失效日期错误: 失效日期:[%s],交易日期[%s]", expireDate.c_str(), transDate.c_str());
		m_tvr.ApplicationExpired = true;
	}
	else {
		Log->Info("失效日期验证成功");
	}

	//检查应用版本号
	string termApplicationVersion = CTerminal::GetTerminalData("9F09");
	string cardApplicationVersion = FindTagValue("9F08");
	if (termApplicationVersion != cardApplicationVersion)
	{
		Log->Info("应用版本号检查失败!卡片应用版本号:[%s],终端应用版本号:[%s]", cardApplicationVersion, termApplicationVersion);
		m_tvr.ApplicationVersionFailed = true;
	}
	else {
		Log->Info("应用版本号验证成功! 卡片版本号[%s]", cardApplicationVersion.c_str());
	}


	//检查AUC
	string AUC = FindTagValue("9F07");
	ShowCardAUC(AUC);

	return false;
}

//显示CVM方法
void EMV::ShowCVMMethod(int method)
{
	switch (method)
	{
	case 0x00:
		Log->Info("CVM失败处理");
		break;
	case 0x01:
		Log->Info("卡片执行明文PIN核对");
		break;
	case 0x02:
		Log->Info("联机加密PIN验证");
		break;
	case 0x03:
		Log->Info("卡片执行明文PIN核对+签名(纸上)");
		break;
	case 0x1E:
		Log->Info("签名(纸上)");
		break;
	case 0x1F:
		Log->Info("无需CVM");
		break;
	case 0x20:
		Log->Info("持卡人证据出示");
		break;
	default:
		Log->Error("CVM验证方法保留或有误");
		break;
	}
}

//显示CVM条件
void EMV::ShowCVMCondition(int condition)
{
	switch (condition)
	{
	case 0x00:
		Log->Info("总是");
		break;
	case 0x01:
		Log->Info("如果是ATM现金交易");
		break;
	case 0x02:
		Log->Info("如果不是ATM现金或有人值守现金或返现交易");
		break;
	case 0x03:
		Log->Info("如果终端支持这个CVM");
		break;
	case 0x04:
		Log->Info("如果是人工值守现金交易");
		break;
	case 0x05:
		Log->Info("如果是返现交易");
		break;
	case 0x06:
		Log->Info("如果交易货币等于应用货币代码而且小于X值");
		break;
	case 0x07:
		Log->Info("如果交易货币等于应用货币代码而且大于X值");
		break;
	case 0x08:
		Log->Info("如果交易货币等于应用货币代码而且小于Y值");
		break;
	case 0x09:
		Log->Info("如果交易货币等于应用货币代码而且大于Y值");
		break;
	default:
		Log->Error("CVM条件有误或为保留值");
		break;
	}
}

/************************************************************
* 持卡人验证用于确保持卡人身份合法以及卡片没有丢失。在持卡人验证处
* 理中，终端决定要使用的CVM并执行选的的持卡人验证方法。
*************************************************************/
bool EMV::CardHolderValidation()
{
	Log->Info("======================== 持卡人验证 开始 =================================");
	CVM cvm;
	string strCVM = FindTagValue("8E");

	//解析CMV值
	cvm.X = strCVM.substr(0, 8);
	cvm.Y = strCVM.substr(8, 8);

	for (unsigned int i = 16; i < strCVM.length(); i += 4)
	{
		CVMLIST cvmList;
		cvmList.code = strCVM.substr(i, 2);
		cvmList.condition = strCVM.substr(i + 2, 2);
		cvm.cvmList.push_back(cvmList);
	}

	for (auto s : cvm.cvmList)
	{
		int code = stoi(s.code, 0, 16);
		int condition = stoi(s.condition, 0, 16);
		int bAcceptRule = code & 0x80;
		if (bAcceptRule == 0) {
			Log->Info("只有符合此规范的取值");
		}
		else {
			Log->Info("有自定义的值");
		}
		int executeNextCVM = code & 0x040;
		if (executeNextCVM)
		{
			Log->Info("如果此CVM失败，应用后续的");
		}
		else {
			Log->Info("如果此CVM失败，则持卡人验证失败");
		}
		int cvmMethod = code & 0x3F;

		ShowCVMCondition(condition);
		ShowCVMMethod(cvmMethod);	
	}	

	return false;
}

/************************************************************
* 终端风险管理为大额交易提供了发卡行授权，确保芯片交易可以周期性
* 的进行联机处理，防止过度欠款和在脱机环境中不易察觉的攻击。
* 主要执行以下检查，并将结果记录到TVR中:
	1. 终端异常文件检查(忽略)
	2. 商户强制交易联机(忽略)
	3. 最低限额检查
	4. 频度检查
	5. 新卡检查
*************************************************************/
bool EMV::TerminalRiskManagement()
{
	Log->Info("======================== 终端风险管理 开始 =================================");
	//最低限额检查
	string transMoney = ReadTag("81");
	if (transMoney.empty())
	{
		Log->Error("无法获取授权金额!");
	}
	else {
		string termTransFloorLimit = CTerminal::GetTerminalData("9F1B");
		if (stoi(transMoney, 0, 16) < stoi(termTransFloorLimit, 0, 16))
		{
			Log->Error("最低限额检查失败! 授权金额[%s]低于最低交易金额[%s]", transMoney.c_str(), termTransFloorLimit.c_str());
			m_tvr.TransExceedFloorLimitation = true;
		}
		else {
			Log->Info("最低限额检查成功!");
		}
	}
	
	//频度检查
	string lastOnlineATC = FindTagValue("9F13");
	string ATC = FindTagValue("9F36");
	string offlineTransFloorLimit = FindTagValue("9F23");
	string offlineTransUpLimit = FindTagValue("9F14");

	int nOfflineTransFloorLimit = stoi(offlineTransFloorLimit, 0, 16);
	int nOfflineTransUpLimit = stoi(offlineTransUpLimit, 0, 16);

	if (lastOnlineATC.empty() || ATC.empty())
	{
		Log->Error("频度检查失败!无法获取上次联机ATC或者应用交易ATC");
		m_tvr.ExceedContinuedTransUpLimitation = true;
		m_tvr.ExceedContinuedTransFloorLimitation = true;
	}
	Log->Info("上次联机ATC:[%s]", lastOnlineATC.c_str());
	Log->Info("应用交易计数器ATC:[%s]", ATC.c_str());

	int offlineTransCount = stoi(ATC, 0, 16) - stoi(lastOnlineATC, 0, 16);
	if (offlineTransCount > nOfflineTransUpLimit || offlineTransCount < nOfflineTransFloorLimit)
	{
		if (offlineTransCount > nOfflineTransUpLimit)
		{
			Log->Error("频度检查失败,超出脱机交易上限! 连续脱机交易次数[%02X],脱机交易上限[%02X]", offlineTransCount, nOfflineTransUpLimit);
			m_tvr.ExceedContinuedTransUpLimitation = true;
		}
		if(offlineTransCount < nOfflineTransFloorLimit) 
		{
			Log->Error("频度检查失败,超出脱机交易下限! 连续脱机交易次数[%02X],脱机交易下限[%02X]", offlineTransCount, nOfflineTransFloorLimit);
			m_tvr.ExceedContinuedTransFloorLimitation = true;
		}
	}
	else {
		Log->Info("频度检查成功!");
	}

	//新卡检查
	if (stoi(lastOnlineATC, 0, 16) == 0)
	{
		Log->Info("此卡不是新卡");
	}
	else {
		Log->Info("此卡是新卡");
		m_tvr.IsNewCard = true;
	}
	return false;
}

void EMV::ParseGACResponseData(const string buffer)
{
	SaveTag("9F27", string(buffer.substr(0, 2)));   //密文信息数据 表明卡片返回的密文类型并指出终端要进行的操作
	SaveTag("9F36", string(buffer.substr(2, 4)));   //应用交易计数器(ATC)
	SaveTag("9F26", string(buffer.substr(6, 16)));  //应用密文 8字节
	SaveTag("9F10", string(buffer.substr(22))); //发卡行应用数据 在一个联机交易中，要传送到发卡行的专有应用数据。
}

void EMV::ShowCardTransType()
{
	string cardTransType = FindTagValue("9F27");
	if (cardTransType.length() != 2)
	{
		return;
	}
	if (cardTransType == "80")
	{
		Log->Info("卡片处理结果[ARQC联机]");
	}
	else if (cardTransType == "00")
	{
		Log->Info("卡片处理结果[AAC拒绝]");
	}
	else if (cardTransType == "40")
	{
		Log->Info("卡片处理结果[TC缺省]");
	}
}

/******************************************************************
* 卡片行为分析允许发卡行设置在卡片内部执行频度检查和其他风险管理。
* 包括的检查有(卡片行为分析是卡片根据交易结果设置CVR，由卡片内置程序执行)：
*	1.上次交易行为
*	2.卡片是否为新卡
*	3.脱机交易计数和累计脱机金额
* 最后，卡片根据CVR决定卡片对交易做AAC/ARQC/TC处理
*******************************************************************/
bool EMV::CardActionAnalized(TERM_TRANS_TYPE type)
{
	Log->Info("======================== 卡片行为分析 开始 =================================");
	GACControlParam P1;
	switch (type)
	{
	case AAC:
		P1 = GACControlParam::AAC;
		break;
	case ARQC:
		P1 = GACControlParam::ARQC;
		break;
	case TC:
		P1 = GACControlParam::TC;
		break;
	default:
		break;
	}
	
	/******************************** GAC 1 ******************************/
	string CDOL1 = FindTagValue("8C");
	BCD_TL CDOL1tlEntities[32] = { 0 };
	unsigned int CDOL1tlEntitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)CDOL1.c_str(), CDOL1.length(), CDOL1tlEntities, CDOL1tlEntitiesCount);
	string termData;
	for (unsigned int i = 0; i < CDOL1tlEntitiesCount; i++)
	{
		termData += CTerminal::GetTerminalData((char*)CDOL1tlEntities[i].tag);
	}

	APDU_RESPONSE response1;
	memset(&response1, 0, sizeof(response1));
	m_pAPDU->GACCommand(P1, termData, response1);

	//解析卡片响应数据
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)response1.data, strlen(response1.data), entites, entitiesCount);

	//格式化输出响应数据
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//分析GAC 卡片响应数据
	ParseGACResponseData(FindTagValue("80"));
	ShowCardTransType();

	return true;
}

//比较IAC和TAC结果
bool EMV::CompareIACAndTVR(string IAC, string strTVR)
{
	if (IAC.length() != strTVR.length())
	{
		Log->Error("IAC与TAC长度不一致");
		return false;
	}
	if (IAC.length() != 10)
	{
		Log->Error("IAC长度有误");
		return false;
	}

	//逐个字节进行比较
	for (int i = 0; i < 10; i += 2)
	{
		int nIAC = stoi(IAC.substr(i, 2), 0, 16);
		int nTVR = stoi(strTVR.substr(i, 2), 0, 16);

		if (nIAC & nTVR)
			return true;
	}

	return false;
}

TERM_TRANS_TYPE EMV::GetTermAnanlizedResult()
{
	TERM_TRANS_TYPE type = TERM_TRANS_TYPE::TERM_TC;
	string IACDefault = FindTagValue("9F0D");
	string IACRejection = FindTagValue("9F0E");
	string IACOnline = FindTagValue("9F0F");

	if (IACDefault.empty())
		IACDefault = "0000000000";
	if (IACRejection.empty())
		IACRejection = "0000000000";
	if (IACOnline.empty())
		IACOnline = "0000000000";
	string strTVR = GetTVR();

	if (CompareIACAndTVR(IACRejection, strTVR))
	{//如果TVR和IAC-拒绝有同位置1
		Log->Info("IAC Reject: %s", IACRejection.c_str());
		Log->Info("TVR: %s", strTVR.c_str());
		Log->Info("终端执行 TAC-拒绝(AAC)");
		type = TERM_TRANS_TYPE::TERM_AAC;
	}
	else {
		//TVR和IAC-拒绝没有同位置1
		//联机处理
		if (CompareIACAndTVR(IACOnline, strTVR))
		{
			Log->Info("IAC Online: %s", IACOnline.c_str());
			Log->Info("TVR: %s", strTVR.c_str());
			Log->Info("终端执行 TAC-联机(ARQC)");
			type = TERM_TRANS_TYPE::TERM_ARQC;
		}
		else {
			//使用缺省
			Log->Info("IAC Default: %s", IACDefault.c_str());
			Log->Info("TVR: %s", strTVR.c_str());
			Log->Info("终端执行 TAC-缺省(TC)");
			type = TERM_TRANS_TYPE::TERM_TC;
		}
	}

	return type;
}

/*************************************************************
* 终端行为分析包括步骤：
* 1. 检查脱机处理结果。
	终端检查TVR中的脱机处理结果，以决定交易是否脱机批准、脱机拒绝
	或请求联机授权。本过程终端需要将TVR与由发卡行设定在卡片中的规
	则(发卡行行为代码IAC)和收单行设定在终端中的规则(终端行为代码
	TAC)进行比较
* 2. 请求密文处理(该GAC转移到卡片行为分析中发送)
	终端根据第一步的判断结果向卡片请求相应的应用密文。
**************************************************************/
bool EMV::TerminalActionAnalized()
{
	TERM_TRANS_TYPE type = TERM_TRANS_TYPE::TERM_TC;
	Log->Info("======================== 终端行为分析 开始 =================================");
	type = GetTermAnanlizedResult();

	return false;
}

bool EMV::GetTagAfterGAC()
{
	string tags[4] = { "9F5D","9F77","9F78","9F79" };
	for (int i = 0; i < 4; i++)
	{
		string response = ReadTag(tags[i]);
		if (response.empty())
		{
			return false;
		}
		Log->Info("[%s]=%s", tags[i].c_str(), response.c_str());
	}

	return true;
}

//获取终端TVR
string EMV::GetTVR()
{
	int byte1 = 0;
	int byte2 = 0;
	int byte3 = 0;
	int byte4 = 0;
	int byte5 = 0;

	m_tvr.RequirePin = true;

	byte1 += ((m_tvr.OfflineDataValidationIsDone ? 1 : 0) << 7) +
		((m_tvr.SDAFailed ? 1 : 0) << 6) +
		((m_tvr.ICCDataabsence ? 1 : 0) << 5) +
		((m_tvr.CardInTermExceptionFile ? 1 : 0) << 4) +
		((m_tvr.DDAFailed ? 1 : 0) << 3) +
		((m_tvr.CDAFailed ? 1 : 0) << 2);
	byte2 += ((m_tvr.ApplicationVersionFailed ? 1 : 0) << 7) +
		((m_tvr.ApplicationExpired ? 1 : 0) << 6) +
		((m_tvr.ApplicationNotEffect ? 1 : 0) << 5) +
		((m_tvr.ServiceDenyByCard ? 1 : 0) << 4) +
		((m_tvr.IsNewCard ? 1 : 0) << 3);
	byte3 += ((m_tvr.CardOnwerValidationFailed ? 1 : 0) << 7) +
		((m_tvr.UnknownCVM ? 1 : 0) << 6) +
		((m_tvr.PinRetryExceedLimitation ? 1 : 0) << 5) +
		((m_tvr.RequirePinWithoutKeyborad ? 1 : 0) << 4) +
		((m_tvr.RequirePinButNotInput ? 1 : 0) << 3) +
		((m_tvr.RequirePin ? 1 : 0) << 2);
	byte4 += ((m_tvr.TransExceedFloorLimitation ? 1 : 0) << 7) +
		((m_tvr.ExceedContinuedTransFloorLimitation ? 1 : 0) << 6) +
		((m_tvr.ExceedContinuedTransUpLimitation ? 1 : 0) << 5) +
		((m_tvr.TransUseRandomOnlineHandle ? 1 : 0) << 4) +
		((m_tvr.CustomRequireOnlineTrans ? 1 : 0) << 3);
	byte5 += ((m_tvr.UseDefaultTDOL ? 1 : 0) << 7) +
		((m_tvr.IssuerValidationFailed ? 1 : 0) << 6) +
		((m_tvr.HandlePSFailedBeforeLastGAC ? 1 : 0) << 5) +
		((m_tvr.HandlePSFailedAfterLastGAC ? 1 : 0) << 4);

	char szTVR[32] = { 0 };
	sprintf_s(szTVR, 32, "%02X%02X%02X%02X%02X", byte1, byte2, byte3, byte4, byte5);

	return string(szTVR);
}

string EMV::GetPAN()
{
	string tag57 = FindTagValue("57");
	int index = tag57.find('D');
	if (index == string::npos)
	{//未找到，直接返回空
		return "";
	}
	return tag57.substr(0, index);
}

string EMV::GenARPC(string AC, string authCode)
{
	authCode += "000000000000";
	char* szAC = (char*)AC.c_str();
	char* szAuthCode = (char*)authCode.c_str();
	str_xor(szAC, szAuthCode, 16);
	char szARPC[32] = { 0 };

	KeyGenerator kg;
	string sudkAuth;
	string ATC = FindTagValue("9F36");
	string cardSeq = FindTagValue("5F34");
	string PAN = GetPAN();
	if (!m_mdkAuth.empty())
	{
		m_udkAuth = kg.GenUDKAuthFromMDKAuth(m_mdkAuth, ATC, PAN, cardSeq);
	}
	if (!m_udkAuth.empty())
	{
		sudkAuth = kg.GenSUDKAuthFromUDKAuth(m_udkAuth, ATC);
	}
	else {
		return "";
	}

	Des3(szARPC, (char*)sudkAuth.c_str(), szAC);

	return string(szARPC);
}

/******************************************************************
* 联机处理允许发卡行使用发卡行主机系统中的风险管理参数对交易进行检查，作
* 出批准或拒绝交易的决定。
* 发卡行的响应包括卡片的二次发卡更新和一个发卡行生成的密文。卡片验证密文
* 确保响应来自一个有效的发卡行。此验证叫发卡行认证。
*******************************************************************/
bool EMV::OnlineBussiness()
{
	Log->Info("======================== 联机处理 开始 =================================");
	string CID = FindTagValue("9F27");		// 	密文信息数据
	string ATC = FindTagValue("9F36");
	string AC = FindTagValue("9F26");		//应用密文（AC）
	string issuerApplicationData = FindTagValue("9F10");	//发卡行应用数据
	
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
	
	string strARPC = GenARPC(AC, "3030");
	if (!m_pAPDU->ExternalAuthcateCommand(strARPC, "3030", response))
	{
		m_tvr.IssuerValidationFailed = true;
		Log->Error("联机处理 外部认证成功");
		return false;
	}
	if (response.SW1 == 0x90 && response.SW2 == 00)
	{
		m_IsOnlineAuthSucessed = true;
		Log->Info("联机处理 外部认证成功");
	}
	else {
		Log->Warning("联机处理 外部认证失败");
		m_tvr.IssuerValidationFailed = true;
	}

	return true;
}

void EMV::ParseTransLog(const string buffer)
{
	string transDate;		//交易日期
	string transTime;		//交易时间
	string money;			//授权金额
	string othermoney;		//其他金额
	string termCountryCode;	//终端国家代码
	string transCode;		//交易货币代码
	string customName;		//商户名称
	string transType;		//交易类型
	string appTransCounter;	//应用交易计数器

	if (buffer.length() < 86)
	{
		return;
	}
	transDate = buffer.substr(0, 6);
	transTime = buffer.substr(6, 6);
	money = buffer.substr(12, 12);
	othermoney = buffer.substr(24, 12);
	termCountryCode = buffer.substr(36, 4);
	transCode = buffer.substr(40, 4);
	customName = buffer.substr(44, 40);
	transType = buffer.substr(84, 2);
	appTransCounter = buffer.substr(86);

}

//发卡行脚本处理
bool EMV::DealIusserScript()
{
	//获取电子现金圈存日志入口
	string logCashEntry = FindTagValue("DF4D");
	string logEntry = FindTagValue("9F4D");

	string cashSFI = logCashEntry.substr(0, 2);
	int cashLogNum = stoi(logCashEntry.substr(2), 0, 16);
	string logSFI = logEntry.substr(0, 2);
	int logNum = stoi(logEntry.substr(2), 0, 16);

	//显示电子现金圈存日志
	for (int i = 0; i < cashLogNum; i++)
	{
		APDU_RESPONSE response;
		memset(&response, 0, sizeof(response));
		char seq[3] = { 0 };
		sprintf_s(seq, 3, "%02X", i);
		m_pAPDU->ReadRecordCommand(cashSFI, seq, response);
		if (response.SW1 != 0x90 && response.SW2 != 0x00)
		{
			break;
		}
		Log->Info("Cash Log: [%d]=%s", i, response.data);
	}

	//显示日志
	for (int j = 0; j < logNum; j++)
	{
		APDU_RESPONSE response;
		memset(&response, 0, sizeof(response));
		char seq[3] = { 0 };
		sprintf_s(seq, 3, "%02X", j);
		m_pAPDU->ReadRecordCommand(logSFI, seq, response);
		if (response.SW1 != 0x90 && response.SW2 != 0x00)
		{
			break;
		}
		Log->Info("Card Log: [%d]=%s", j, response.data);
	}

	return true;
}

//交易结束
bool EMV::EndTransaction()
{
	Log->Info("======================== 交易结束 开始 =================================");
	GACControlParam P1;
	string authCode;
	if (m_IsOnlineAuthSucessed)
	{
		P1 = GACControlParam::TC;
		authCode = "3030";
	}
	else {
		P1 = GACControlParam::AAC;
		authCode = "2020";
	}

	/******************************** GAC 2 ******************************/
	string CDOL2 = FindTagValue("8D");
	BCD_TL CDOL2tlEntities[32] = { 0 };
	unsigned int CDOL2tlEntitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)CDOL2.c_str(), CDOL2.length(), CDOL2tlEntities, CDOL2tlEntitiesCount);
	string termData;
	for (unsigned int i = 0; i < CDOL2tlEntitiesCount; i++)
	{
		if (string((char*)CDOL2tlEntities[i].tag) == "8A")
		{
			termData += authCode;
			continue;
		}
		termData += CTerminal::GetTerminalData((char*)CDOL2tlEntities[i].tag);
	}

	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
	m_pAPDU->GACCommand(P1, termData, response);

	//解析卡片响应数据
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//格式化输出响应数据
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//分析GAC 卡片响应数据
	ParseGACResponseData(FindTagValue("80"));
	ShowCardTransType();


	return true;
}
