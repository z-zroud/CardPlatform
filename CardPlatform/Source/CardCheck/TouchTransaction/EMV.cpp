#include "stdafx.h"
#include "EMV.h"
#include "Util\KeyGenerator.h"
#include "Util\Log.h"
#include "Util\Des0.h"
#include "Util\SHA1.h"
#include "Util\SM.hpp"
#include "Util\Converter.h"
#include "..\Terminal.h"

//���ַ�����ʮ������תΪ����
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

//��Ҫʵ������Щ����Ҫʹ��APDUָ��Ķ��������ѻ�������֤
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
	char szTransDate[32] = { 0 };	//��������
	sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wYear, st.wMonth, st.wDay);

	return string(szTransDate).substr(2);
}

string EMV::GenTransTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szTransDate[32] = { 0 };	//��������
	sprintf_s(szTransDate, 32, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);

	return string(szTransDate);
}

string EMV::GetResult(unsigned char SW1, unsigned char SW2)
{
	char temp[5] = { 0 };
	sprintf_s(temp, 5, "%02X%02X", SW1, SW2);

	return string(temp);
}

//��ʽ�������Ӧ����
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


//����ָ��Tagֵ
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

//�����ѻ�ȡ��Tagֵ
string EMV::FindTagValue(const string tag)
{
    for (auto s : m_Tags)
    {
        if (tag == s.first)
            return s.second;
    }

    return "";
}

//����tagֵ
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
        return;		//ÿ��AFLռ��4���ֽڣ�Ҳ����8��BCD��
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

//PSEѡ��
string EMV::SelectPSE(APP_TYPE appType)
{
    APDU_RESPONSE response;

    memset(&response, 0, sizeof(response));
    if(appType == APP_PSE)  //ִ��PSEӦ��
    {
        if (!m_pAPDU->SelectPSECommand(response))
        {
            Log->Error("ִ�� select PSE ʧ��!");
            return "";
        }
    }
    else if (appType == APP_PPSE)   //ִ��PPSEӦ��
    {
        if (!m_pAPDU->SelectPPSECommand(response))
        {
            Log->Error("ִ�� select PSE ʧ��!");
            return "";
        }
    }

	if (response.SW1 == 0x90 && response.SW2 == 0x00)
	{
		BCD_TLV entities[32] = { 0 };
		unsigned int entitiesCount = 0;

		//������Ӧ����
		m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entities, entitiesCount);
		
		//��ʽ�������Ӧ����
		FormatOutputResponse(entities, entitiesCount);
		SaveTag(entities, entitiesCount);
        if (appType == APP_PSE)
        {
            //����FCI�е�Ŀ¼�ļ�SFI��Ŀ¼�ļ�
            string SFI = FindTagValue("88", entities, entitiesCount);
            if (SFI.empty())
            {
                Log->Error("�Ҳ������ļ���ʶ��");
                return "";
            }

            //����Ŀ¼�ļ���ָ��Ŀ¼
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
                    Log->Error("ִ�� ������ʧ��");
                    return "";
                }
                if (response.SW1 == 0x6A && response.SW2 == 0x83)
                {
                    break;
                }
                recordNumber++;
                m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), dirEntites, dirEntitiesCount);

                //��ʽ�������Ӧ����
                FormatOutputResponse(dirEntites, dirEntitiesCount);
                SaveTag(dirEntites, dirEntitiesCount);
            }
        }
		//����AID
		return FindTagValue("4F");
	}
	else if (response.SW1 == 0x6A && response.SW2 == 0x81)	//����6A81
	{
		Log->Error("����ֵ[%s]: ��Ƭ���������֧��! ��ֹ����", GetResult(response.SW1, response.SW2));
		return false;
	}
	else if (response.SW1 == 0x6A && response.SW2 == 0x82) //����6A82
	{
		Log->Warning("����ֵ[%s]: ��Ƭ������PSE�����ܿ�Ƭδִ�и��˻�", GetResult(response.SW1, response.SW2));
	}
	else if (response.SW1 == 0x62 && response.SW2 == 0x83) //����6283
	{
		Log->Warning("����ֵ[%s]: PSE����");
	}
	else {	
		Log->Error("Select PSE unknown error");
	}

	return "";
}


//AID ѡ��
bool EMV::SelectAID(const string &strAID)
{
	APDU_RESPONSE response;
	
	memset(&response, 0, sizeof(response));
	if (!m_pAPDU->SelectApplicationCommand(strAID, response))
		return false;

	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;

	m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//��ʽ�������Ӧ����
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	return true;
}


//Ӧ��ѡ��
bool EMV::SelectApplication(APP_TYPE type, string app)
{	
	Log->Info("======================== ѡ��Ӧ�� ��ʼ =================================");
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

//Ӧ�ó�ʼ��
bool EMV::InitilizeApplication(ENCRYPT_TYPE type)
{
	Log->Info("======================== Ӧ�ó�ʼ�� ��ʼ =================================");
	string PDOL = FindTagValue("9F38");
	if (PDOL.empty())
		return false;

	BCD_TL tlEntities[32] = { 0 };
	unsigned int tlEntitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)PDOL.c_str(), PDOL.length(), tlEntities, tlEntitiesCount);

	//����PDOL��Ӧ�����ֶ�
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

	//����GPO����
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
	if (!m_pAPDU->GPOCommand(data, response))
	{
		return false;
	}
	//������Ӧ����
	string responseData = string(response.data);
	string AIP = responseData.substr(4, 4);
	string AFL = responseData.substr(8);

	//��ʽ�������Ӧ����
	Log->Info("[82]=%s", AIP.c_str());
	Log->Info("[94]=%s", AFL.c_str());

	SaveTag("82", AIP);
	SaveTag("94", AFL);

	return true;
}

//��Ӧ������
bool EMV::ReadApplicationData()
{
	Log->Info("======================== ��Ӧ������ ��ʼ =================================");
	//��ȡAFL
	string strAFL = FindTagValue("94");
	if (strAFL.empty())
	{
		return false;
	}
	//����AFL
	vector<AFL> vecAFL;
	ParseAFL(strAFL, vecAFL);
	int i = 1;
	for (auto v : vecAFL)
	{
		//��ȡÿ��AFL��¼
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
			//���澲̬ǩ������
			string sigData = response.data;

			//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
			if (ctoi(sigData[2]) & 0x08)
			{
				//���λ1
				unsigned int lengthSize = 2 * ((sigData[2] & 0x07) * 8 + (sigData[3] & 0x0f));
				m_staticApplicationData += sigData.substr(lengthSize + 4);

			}
			else
			{
				m_staticApplicationData += sigData.substr(4);
			}
		}

		//����tag
		BCD_TLV entites[32] = { 0 };
		unsigned int entitiesCount = 0;

		m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);
		FormatOutputResponse(entites, entitiesCount);
		SaveTag(entites, entitiesCount);
	}

	//��ȡ��ǩ�б�
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

//��ȡ��ǩ����
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

//У�龲̬Ӧ������
bool EMV::ValidateStaticApplicationData(string issuerPublicKey, ENCRYPT_TYPE encryptType)
{
	string signedStaticAppData = FindTagValue("93");
	char recoveryData[2046] = { 0 };

    Log->Info("Issuser Public Key: %s", issuerPublicKey.c_str());
    Log->Info("static app data: %s", signedStaticAppData.c_str());

    if (encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
    {
        //����ǩ���ľ�̬Ӧ������
        RSA_STD((char*)issuerPublicKey.c_str(), "03", (char*)signedStaticAppData.c_str(), recoveryData);
        int recoveryDataLen = strlen(recoveryData);
        string strRecoveryData(recoveryData);

        if (strRecoveryData.substr(0, 4) != "6A03" || strRecoveryData.substr(recoveryDataLen - 2, 2) != "BC")
        {//����ָ����ݵĿ�ͷ����"6A02"���ҽ�β����"BC",��֤ʧ��
            return false;
        }
        string hashData = strRecoveryData.substr(recoveryDataLen - 42, 40);

        //�����̬������֤��ǩ�б���ڣ�����������ǡ�82���ı�ǩ����ô��̬������֤ʧ��
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

	//����tag
	string strResponse = response.data;
    if (strResponse.empty())
    {
        Log->Error("��̬ǩ��ʧ��,�޷���ȡtag 9F4B");
        return false;
    }
    //�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
    if (ctoi(strResponse[2]) & 0x08)
    {
        //���λ1
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
        //�Ӷ�̬ǩ�������л�ȡ�ָ�����
        RSA_STD((char*)ICCPublicKey.c_str(), "03", (char*)signedData.c_str(), recoveryData);
        string strRecoveryData = recoveryData;
        int recoveryDataLen = strlen(recoveryData);
        if (recoveryDataLen == 0)
        {
            Log->Error("��̬ǩ��ʧ�ܣ��޷���ȡ�ָ�����");
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

//�ѻ�������֤
bool EMV::OfflineDataAuth()
{
	Log->Info("======================== �ѻ�������֤ ��ʼ =================================");
	//��̬������֤
    string issuerPublicCert = FindTagValue("90");   //�����й�Կ֤��
    string caIndex = FindTagValue("8F");    //CA��Կ���� PKI
    string issuerExponent = FindTagValue("9F32");   //�����й�Կָ��
    string ipkRemainder = FindTagValue("92");   //�����й�Կ����
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
        //��̬������֤	
        string iccPublicCert = FindTagValue("9F46");    //ICC��Կ֤��       
        string signedData = m_staticApplicationData + FindTagValue("82");   //ǩ������
        string ICCPublicKey;
        if (m_encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
        {
            string iccRemainder = FindTagValue("9F48"); //ICC��Կ����
            string iccExponent = FindTagValue("9F47");  //ICC��Կָ��
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
        //CDA��֤
        //...
    }


	return true;
}

//���ڱȽ�
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


//��鿨ƬAUC��Ϣ
void EMV::ShowCardAUC(string AUC)
{
	int nAUC1 = stoi(AUC.substr(0, 2), 0, 16);
	int nAUC2 = stoi(AUC.substr(2, 2), 0, 16);

	Log->Info("AUC: %s", AUC.c_str());
	if (nAUC1 & 0x80) { Log->Info("�����ֽ�����Ч"); }
	if (nAUC1 & 0x40) { Log->Info("�����ֽ�����Ч"); }
	if (nAUC1 & 0x20) { Log->Info("������Ʒ��Ч"); }
	if (nAUC1 & 0x10) { Log->Info("������Ʒ��Ч"); }
	if (nAUC1 & 0x08) { Log->Info("���ڷ�����Ч"); }
	if (nAUC1 & 0x04) { Log->Info("���ʷ�����Ч"); }
	if (nAUC1 & 0x02) { Log->Info("ATM��Ч"); }
	if (nAUC1 & 0x01) { Log->Info("��ATM����ն���Ч"); }
	if (nAUC1 & 0x80) { Log->Info("������ڷ���"); }
	if (nAUC1 & 0x40) { Log->Info("������ʷ���"); }
}

/**************************************************************
* ����������Ҫ�������·���ļ�飬�����������¼��TVR��
*	1.Ӧ����Ч���ڼ��
*	2.Ӧ��ʧЧ���ڼ��
*	3.Ӧ�ð汾�ż��
*	4.Ӧ����;���Ƽ��AUC
*	5.�����й��Ҵ�����
***************************************************************/
bool EMV::HandleLimitation()
{
	Log->Info("======================== �������� ��ʼ =================================");
	//�����Ч����
	string effectDate = FindTagValue("5F25");
	string transDate = GenTransDate();
	if (!CompareDate(transDate, effectDate))
	{
		Log->Error("��Ч���ڴ���: ��Ч����:[%s], ��������[%s]", effectDate.c_str(), transDate.c_str());
		m_tvr.ApplicationNotEffect = true;
	}
	else {
		Log->Info("��Ч������֤�ɹ�");
	}
	
	//���ʧЧ����
	string expireDate = FindTagValue("5F24");
	if (!CompareDate(expireDate, transDate))
	{
		Log->Error("ʧЧ���ڴ���: ʧЧ����:[%s],��������[%s]", expireDate.c_str(), transDate.c_str());
		m_tvr.ApplicationExpired = true;
	}
	else {
		Log->Info("ʧЧ������֤�ɹ�");
	}

	//���Ӧ�ð汾��
	string termApplicationVersion = CTerminal::GetTerminalData("9F09");
	string cardApplicationVersion = FindTagValue("9F08");
	if (termApplicationVersion != cardApplicationVersion)
	{
		Log->Info("Ӧ�ð汾�ż��ʧ��!��ƬӦ�ð汾��:[%s],�ն�Ӧ�ð汾��:[%s]", cardApplicationVersion, termApplicationVersion);
		m_tvr.ApplicationVersionFailed = true;
	}
	else {
		Log->Info("Ӧ�ð汾����֤�ɹ�! ��Ƭ�汾��[%s]", cardApplicationVersion.c_str());
	}


	//���AUC
	string AUC = FindTagValue("9F07");
	ShowCardAUC(AUC);

	return false;
}

//��ʾCVM����
void EMV::ShowCVMMethod(int method)
{
	switch (method)
	{
	case 0x00:
		Log->Info("CVMʧ�ܴ���");
		break;
	case 0x01:
		Log->Info("��Ƭִ������PIN�˶�");
		break;
	case 0x02:
		Log->Info("��������PIN��֤");
		break;
	case 0x03:
		Log->Info("��Ƭִ������PIN�˶�+ǩ��(ֽ��)");
		break;
	case 0x1E:
		Log->Info("ǩ��(ֽ��)");
		break;
	case 0x1F:
		Log->Info("����CVM");
		break;
	case 0x20:
		Log->Info("�ֿ���֤�ݳ�ʾ");
		break;
	default:
		Log->Error("CVM��֤��������������");
		break;
	}
}

//��ʾCVM����
void EMV::ShowCVMCondition(int condition)
{
	switch (condition)
	{
	case 0x00:
		Log->Info("����");
		break;
	case 0x01:
		Log->Info("�����ATM�ֽ���");
		break;
	case 0x02:
		Log->Info("�������ATM�ֽ������ֵ���ֽ���ֽ���");
		break;
	case 0x03:
		Log->Info("����ն�֧�����CVM");
		break;
	case 0x04:
		Log->Info("������˹�ֵ���ֽ���");
		break;
	case 0x05:
		Log->Info("����Ƿ��ֽ���");
		break;
	case 0x06:
		Log->Info("������׻��ҵ���Ӧ�û��Ҵ������С��Xֵ");
		break;
	case 0x07:
		Log->Info("������׻��ҵ���Ӧ�û��Ҵ�����Ҵ���Xֵ");
		break;
	case 0x08:
		Log->Info("������׻��ҵ���Ӧ�û��Ҵ������С��Yֵ");
		break;
	case 0x09:
		Log->Info("������׻��ҵ���Ӧ�û��Ҵ�����Ҵ���Yֵ");
		break;
	default:
		Log->Error("CVM���������Ϊ����ֵ");
		break;
	}
}

/************************************************************
* �ֿ�����֤����ȷ���ֿ�����ݺϷ��Լ���Ƭû�ж�ʧ���ڳֿ�����֤��
* ���У��ն˾���Ҫʹ�õ�CVM��ִ��ѡ�ĵĳֿ�����֤������
*************************************************************/
bool EMV::CardHolderValidation()
{
	Log->Info("======================== �ֿ�����֤ ��ʼ =================================");
	CVM cvm;
	string strCVM = FindTagValue("8E");

	//����CMVֵ
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
			Log->Info("ֻ�з��ϴ˹淶��ȡֵ");
		}
		else {
			Log->Info("���Զ����ֵ");
		}
		int executeNextCVM = code & 0x040;
		if (executeNextCVM)
		{
			Log->Info("�����CVMʧ�ܣ�Ӧ�ú�����");
		}
		else {
			Log->Info("�����CVMʧ�ܣ���ֿ�����֤ʧ��");
		}
		int cvmMethod = code & 0x3F;

		ShowCVMCondition(condition);
		ShowCVMMethod(cvmMethod);	
	}	

	return false;
}

/************************************************************
* �ն˷��չ���Ϊ�����ṩ�˷�������Ȩ��ȷ��оƬ���׿���������
* �Ľ�������������ֹ����Ƿ������ѻ������в��ײ���Ĺ�����
* ��Ҫִ�����¼�飬���������¼��TVR��:
	1. �ն��쳣�ļ����(����)
	2. �̻�ǿ�ƽ�������(����)
	3. ����޶���
	4. Ƶ�ȼ��
	5. �¿����
*************************************************************/
bool EMV::TerminalRiskManagement()
{
	Log->Info("======================== �ն˷��չ��� ��ʼ =================================");
	//����޶���
	string transMoney = ReadTag("81");
	if (transMoney.empty())
	{
		Log->Error("�޷���ȡ��Ȩ���!");
	}
	else {
		string termTransFloorLimit = CTerminal::GetTerminalData("9F1B");
		if (stoi(transMoney, 0, 16) < stoi(termTransFloorLimit, 0, 16))
		{
			Log->Error("����޶���ʧ��! ��Ȩ���[%s]������ͽ��׽��[%s]", transMoney.c_str(), termTransFloorLimit.c_str());
			m_tvr.TransExceedFloorLimitation = true;
		}
		else {
			Log->Info("����޶���ɹ�!");
		}
	}
	
	//Ƶ�ȼ��
	string lastOnlineATC = FindTagValue("9F13");
	string ATC = FindTagValue("9F36");
	string offlineTransFloorLimit = FindTagValue("9F23");
	string offlineTransUpLimit = FindTagValue("9F14");

	int nOfflineTransFloorLimit = stoi(offlineTransFloorLimit, 0, 16);
	int nOfflineTransUpLimit = stoi(offlineTransUpLimit, 0, 16);

	if (lastOnlineATC.empty() || ATC.empty())
	{
		Log->Error("Ƶ�ȼ��ʧ��!�޷���ȡ�ϴ�����ATC����Ӧ�ý���ATC");
		m_tvr.ExceedContinuedTransUpLimitation = true;
		m_tvr.ExceedContinuedTransFloorLimitation = true;
	}
	Log->Info("�ϴ�����ATC:[%s]", lastOnlineATC.c_str());
	Log->Info("Ӧ�ý��׼�����ATC:[%s]", ATC.c_str());

	int offlineTransCount = stoi(ATC, 0, 16) - stoi(lastOnlineATC, 0, 16);
	if (offlineTransCount > nOfflineTransUpLimit || offlineTransCount < nOfflineTransFloorLimit)
	{
		if (offlineTransCount > nOfflineTransUpLimit)
		{
			Log->Error("Ƶ�ȼ��ʧ��,�����ѻ���������! �����ѻ����״���[%02X],�ѻ���������[%02X]", offlineTransCount, nOfflineTransUpLimit);
			m_tvr.ExceedContinuedTransUpLimitation = true;
		}
		if(offlineTransCount < nOfflineTransFloorLimit) 
		{
			Log->Error("Ƶ�ȼ��ʧ��,�����ѻ���������! �����ѻ����״���[%02X],�ѻ���������[%02X]", offlineTransCount, nOfflineTransFloorLimit);
			m_tvr.ExceedContinuedTransFloorLimitation = true;
		}
	}
	else {
		Log->Info("Ƶ�ȼ��ɹ�!");
	}

	//�¿����
	if (stoi(lastOnlineATC, 0, 16) == 0)
	{
		Log->Info("�˿������¿�");
	}
	else {
		Log->Info("�˿����¿�");
		m_tvr.IsNewCard = true;
	}
	return false;
}

void EMV::ParseGACResponseData(const string buffer)
{
	SaveTag("9F27", string(buffer.substr(0, 2)));   //������Ϣ���� ������Ƭ���ص��������Ͳ�ָ���ն�Ҫ���еĲ���
	SaveTag("9F36", string(buffer.substr(2, 4)));   //Ӧ�ý��׼�����(ATC)
	SaveTag("9F26", string(buffer.substr(6, 16)));  //Ӧ������ 8�ֽ�
	SaveTag("9F10", string(buffer.substr(22))); //������Ӧ������ ��һ�����������У�Ҫ���͵������е�ר��Ӧ�����ݡ�
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
		Log->Info("��Ƭ������[ARQC����]");
	}
	else if (cardTransType == "00")
	{
		Log->Info("��Ƭ������[AAC�ܾ�]");
	}
	else if (cardTransType == "40")
	{
		Log->Info("��Ƭ������[TCȱʡ]");
	}
}

/******************************************************************
* ��Ƭ��Ϊ�����������������ڿ�Ƭ�ڲ�ִ��Ƶ�ȼ����������չ���
* �����ļ����(��Ƭ��Ϊ�����ǿ�Ƭ���ݽ��׽������CVR���ɿ�Ƭ���ó���ִ��)��
*	1.�ϴν�����Ϊ
*	2.��Ƭ�Ƿ�Ϊ�¿�
*	3.�ѻ����׼������ۼ��ѻ����
* ��󣬿�Ƭ����CVR������Ƭ�Խ�����AAC/ARQC/TC����
*******************************************************************/
bool EMV::CardActionAnalized(TERM_TRANS_TYPE type)
{
	Log->Info("======================== ��Ƭ��Ϊ���� ��ʼ =================================");
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

	//������Ƭ��Ӧ����
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)response1.data, strlen(response1.data), entites, entitiesCount);

	//��ʽ�������Ӧ����
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//����GAC ��Ƭ��Ӧ����
	ParseGACResponseData(FindTagValue("80"));
	ShowCardTransType();

	return true;
}

//�Ƚ�IAC��TAC���
bool EMV::CompareIACAndTVR(string IAC, string strTVR)
{
	if (IAC.length() != strTVR.length())
	{
		Log->Error("IAC��TAC���Ȳ�һ��");
		return false;
	}
	if (IAC.length() != 10)
	{
		Log->Error("IAC��������");
		return false;
	}

	//����ֽڽ��бȽ�
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
	{//���TVR��IAC-�ܾ���ͬλ��1
		Log->Info("IAC Reject: %s", IACRejection.c_str());
		Log->Info("TVR: %s", strTVR.c_str());
		Log->Info("�ն�ִ�� TAC-�ܾ�(AAC)");
		type = TERM_TRANS_TYPE::TERM_AAC;
	}
	else {
		//TVR��IAC-�ܾ�û��ͬλ��1
		//��������
		if (CompareIACAndTVR(IACOnline, strTVR))
		{
			Log->Info("IAC Online: %s", IACOnline.c_str());
			Log->Info("TVR: %s", strTVR.c_str());
			Log->Info("�ն�ִ�� TAC-����(ARQC)");
			type = TERM_TRANS_TYPE::TERM_ARQC;
		}
		else {
			//ʹ��ȱʡ
			Log->Info("IAC Default: %s", IACDefault.c_str());
			Log->Info("TVR: %s", strTVR.c_str());
			Log->Info("�ն�ִ�� TAC-ȱʡ(TC)");
			type = TERM_TRANS_TYPE::TERM_TC;
		}
	}

	return type;
}

/*************************************************************
* �ն���Ϊ�����������裺
* 1. ����ѻ���������
	�ն˼��TVR�е��ѻ����������Ծ��������Ƿ��ѻ���׼���ѻ��ܾ�
	������������Ȩ���������ն���Ҫ��TVR���ɷ������趨�ڿ�Ƭ�еĹ�
	��(��������Ϊ����IAC)���յ����趨���ն��еĹ���(�ն���Ϊ����
	TAC)���бȽ�
* 2. �������Ĵ���(��GACת�Ƶ���Ƭ��Ϊ�����з���)
	�ն˸��ݵ�һ�����жϽ����Ƭ������Ӧ��Ӧ�����ġ�
**************************************************************/
bool EMV::TerminalActionAnalized()
{
	TERM_TRANS_TYPE type = TERM_TRANS_TYPE::TERM_TC;
	Log->Info("======================== �ն���Ϊ���� ��ʼ =================================");
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

//��ȡ�ն�TVR
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
	{//δ�ҵ���ֱ�ӷ��ؿ�
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
* ����������������ʹ�÷���������ϵͳ�еķ��չ�������Խ��׽��м�飬��
* ����׼��ܾ����׵ľ�����
* �����е���Ӧ������Ƭ�Ķ��η������º�һ�����������ɵ����ġ���Ƭ��֤����
* ȷ����Ӧ����һ����Ч�ķ����С�����֤�з�������֤��
*******************************************************************/
bool EMV::OnlineBussiness()
{
	Log->Info("======================== �������� ��ʼ =================================");
	string CID = FindTagValue("9F27");		// 	������Ϣ����
	string ATC = FindTagValue("9F36");
	string AC = FindTagValue("9F26");		//Ӧ�����ģ�AC��
	string issuerApplicationData = FindTagValue("9F10");	//������Ӧ������
	
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
	
	string strARPC = GenARPC(AC, "3030");
	if (!m_pAPDU->ExternalAuthcateCommand(strARPC, "3030", response))
	{
		m_tvr.IssuerValidationFailed = true;
		Log->Error("�������� �ⲿ��֤�ɹ�");
		return false;
	}
	if (response.SW1 == 0x90 && response.SW2 == 00)
	{
		m_IsOnlineAuthSucessed = true;
		Log->Info("�������� �ⲿ��֤�ɹ�");
	}
	else {
		Log->Warning("�������� �ⲿ��֤ʧ��");
		m_tvr.IssuerValidationFailed = true;
	}

	return true;
}

void EMV::ParseTransLog(const string buffer)
{
	string transDate;		//��������
	string transTime;		//����ʱ��
	string money;			//��Ȩ���
	string othermoney;		//�������
	string termCountryCode;	//�ն˹��Ҵ���
	string transCode;		//���׻��Ҵ���
	string customName;		//�̻�����
	string transType;		//��������
	string appTransCounter;	//Ӧ�ý��׼�����

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

//�����нű�����
bool EMV::DealIusserScript()
{
	//��ȡ�����ֽ�Ȧ����־���
	string logCashEntry = FindTagValue("DF4D");
	string logEntry = FindTagValue("9F4D");

	string cashSFI = logCashEntry.substr(0, 2);
	int cashLogNum = stoi(logCashEntry.substr(2), 0, 16);
	string logSFI = logEntry.substr(0, 2);
	int logNum = stoi(logEntry.substr(2), 0, 16);

	//��ʾ�����ֽ�Ȧ����־
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

	//��ʾ��־
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

//���׽���
bool EMV::EndTransaction()
{
	Log->Info("======================== ���׽��� ��ʼ =================================");
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

	//������Ƭ��Ӧ����
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
	m_parser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//��ʽ�������Ӧ����
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//����GAC ��Ƭ��Ӧ����
	ParseGACResponseData(FindTagValue("80"));
	ShowCardTransType();


	return true;
}
