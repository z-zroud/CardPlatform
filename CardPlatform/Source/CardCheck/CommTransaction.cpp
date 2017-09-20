#include "StdAfx.h"
#include "CommTransaction.h"
#include "Terminal.h"
#include "Util\Log.h"
#include "Util\Converter.h"
#include "Util\KeyGenerator.h"
#include "Util\SHA1.h"
#include "Util\SM.hpp"
#include "Util\Des0.h"

CommTransaction::CommTransaction(IPCSC* reader)
{
	m_pReader	= reader;
	m_pAPDU		= reader->GetAPDU();
	m_pParaser	= new TLVParaser();
	m_IsOnlineAuthSucessed = false;
}

//��Ҫʵ������Щ����Ҫʹ��APDUָ��Ķ��������ѻ�������֤
CommTransaction::CommTransaction()
{
	m_pReader	= NULL;
	m_pAPDU		= NULL;
	m_pParaser	= new TLVParaser();
	m_IsOnlineAuthSucessed = false;
}

CommTransaction::~CommTransaction()
{
	if (m_pParaser)
		delete m_pParaser;
}

void CommTransaction::Clear()
{    
    if (m_pReader)
        delete m_pReader;
    if (m_pAPDU)
        delete m_pAPDU;
    if (m_pParaser)
        delete m_pParaser;
    m_tags.clear();

    m_acceptAuthData = _T("");
    m_mdkAuth   = _T("");
    m_mdkMac    = _T("");
    m_mdkEnc    = _T("");
    m_udkAuth   = _T("");
    m_udkMac    = _T("");
    m_udkEnc    = _T("");
    m_IsOnlineAuthSucessed = false;;
}

/******************************************************************
* ���ܣ� ��ӡentities�����е�tagֵ
*******************************************************************/
void CommTransaction::PrintTags(PBCD_TLV entities, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (entities[i].isTemplate)
		{
			PrintTags(entities[i].subTLVEntity, entities[i].subTLVnum);
		}
		else {
			Log->Info("[%s]=%s", entities[i].Tag, entities[i].Value);
		}
	}
}


/*******************************************************************
* ���ܣ���entities�����л�ȡָ����tagֵ
********************************************************************/
string CommTransaction::GetTagValue(const string tag, PBCD_TLV entities, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (entities[i].isTemplate)
		{
			return GetTagValue(tag, entities[i].subTLVEntity, entities[i].subTLVnum);
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

/*******************************************************************
* ���ܣ����Ѵ洢��tag�л�ȡtagֵ
********************************************************************/
string CommTransaction::GetTagValue(const string tag)
{
	for (auto s : m_tags)
	{
		if (tag == s.first)
			return s.second;
	}

	return _T("");
}

/********************************************************************
* ���ܣ��洢��ǰӦ�������漰����tagֵ
*********************************************************************/
void CommTransaction::SaveTag(PBCD_TLV entites, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (entites[i].isTemplate)
		{
			SaveTag(entites[i].subTLVEntity, entites[i].subTLVnum);
		}
		else
		{
			auto it = m_tags.find((char*)entites[i].Tag);
			if (it != m_tags.end())
			{
				it->second = string((char*)entites[i].Value);
			}
			else {
                m_tags.insert(make_pair<string, string>(string((char*)entites[i].Tag), string((char*)entites[i].Value)));
			}
		}
	}
}

/********************************************************************
* ���ܣ��洢��ǰӦ�������漰����tagֵ
*********************************************************************/
void CommTransaction::SaveTag(string tag, string value)
{
	auto it = m_tags.find(tag);
	if (it != m_tags.end())
	{
		it->second = value;
	}
	else {
        m_tags.insert(make_pair<string, string>(string(tag), string(value)));
	}
}

/*********************************************************************
* ���ܣ�����AFL���ݽṹ�����ɵ����ļ�¼�ţ�����ÿ����¼�Ŵ洢��output��
**********************************************************************/
void CommTransaction::ParaseAFL(string strAFL, vector<AFL> &output)
{
	if (strAFL.length() % 8 != 0)
	{
        //ÿ��AFLռ��4���ֽڣ�Ҳ����8��BCD��
        Log->Error(_T("Please check AFL data: [%s]"), strAFL.c_str());
		return;		
	}
	for (unsigned int i = 0; i < strAFL.length(); i += 8)
	{
		AFL afl;

		afl.SFI = ((Tool::Converter::HexToInt(strAFL[i]) & 0x0F) << 1) + (Tool::Converter::HexToInt(strAFL[i + 1]) && 0x08);
		string strStartRecord = strAFL.substr(i + 2, 2);
		string strEndRecord = strAFL.substr(i + 4, 2);
		string strNeedValidate = strAFL.substr(i + 6, 2);
		int nStartRecord = stoi(strStartRecord, 0, 16);
		int nEndRecord = stoi(strEndRecord, 0, 16);
		int nAcceptAuthencation = stoi(strNeedValidate, 0, 16);

		for (int k = nStartRecord; k <= nEndRecord; k++)
		{
			afl.nRecordNo = k;
			if (nAcceptAuthencation >= k)
			{
				afl.bAcceptAuthencation = true;
			}
			else {
				afl.bAcceptAuthencation = false;
			}

			output.push_back(afl);
		}
	}
}

/*********************************************************************
* ���ܣ�ѡ��PSE ���� PPSE
**********************************************************************/
string CommTransaction::SelectPSE(APP_TYPE appType)
{
	APDU_RESPONSE response;
	if (appType == APP_PSE)  //ִ��PSEӦ��
	{
		if (!m_pAPDU->SelectAppCmd(PSE_AID,response))
		{
			Log->Error("ִ�� select PSE ʧ��!");
			return "";
		}
	}
	else if (appType == APP_PPSE)   //ִ��PPSEӦ��
	{
		if (!m_pAPDU->SelectAppCmd(PPSE_AID,response))
		{
			Log->Error("ִ�� select PPSE ʧ��!");
			return "";
		}
	}

	if (response.SW1 == 0x90 && response.SW2 == 0x00)
	{
		BCD_TLV entities[32] = { 0 };
		unsigned int entitiesCount = 0;

		//������Ӧ����
		m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entities, entitiesCount);

		//��ʽ�������Ӧ����
		PrintTags(entities, entitiesCount);
		SaveTag(entities, entitiesCount);
		if (appType == APP_PSE)
		{
			//����FCI�е�Ŀ¼�ļ�SFI��Ŀ¼�ļ�
			string SFI = GetTagValue("88", entities, entitiesCount);
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
				m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), dirEntites, dirEntitiesCount);

				//��ʽ�������Ӧ����
				PrintTags(dirEntites, dirEntitiesCount);
				SaveTag(dirEntites, dirEntitiesCount);
			}
		}
		//����AID
		return GetTagValue("4F");
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
		Log->Warning("����ֵ[%s]: PSE����", GetResult(response.SW1, response.SW2));
	}
	else {
		Log->Error("Select PSE unknown error");
	}

	return "";
}

string CommTransaction::GetResult(unsigned char SW1, unsigned char SW2)
{
	char temp[5] = { 0 };
	sprintf_s(temp, 5, "%02X%02X", SW1, SW2);

	return string(temp);
}

/************************************************************
* ���ܣ� ѡ��Ӧ��
*************************************************************/
bool CommTransaction::SelectApplication()
{
    APDU_RESPONSE response;
    string aid = GetTagValue(Tag4F);
    if (aid.empty())
    {
        Log->Error("AID can not be empty");
        return false;
    }
    if (!m_pAPDU->SelectAppCmd(aid, response))
    {
        Log->Error("Send Select cmmand failed.");
        return false;
    }

    BCD_TLV entites[32] = { 0 };
    unsigned int entitiesCount = 0;
    m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

    //��ʽ�������Ӧ����
    PrintTags(entites, entitiesCount);
    SaveTag(entites, entitiesCount);

    return true;
}

/****************************************************************
* ���ܣ�Ӧ�ó�ʼ�����ն˸���PDOL����������ݣ�ͨ��GPO�������뿨Ƭ
*****************************************************************/
bool CommTransaction::InitilizeApplication()
 {
     Log->Info("======================== Ӧ�ó�ʼ�� ��ʼ =================================");
     string pdol = GetTagValue(Tag9F38);
     if (pdol.empty())
     {
         Log->Error("tag 9F38 can not be found!");
         return false;
     }
         
     BCD_TL tlEntities[32] = { 0 };
     unsigned int tlEntitiesCount = 0;
     m_pParaser->TLVConstruct((unsigned char*)pdol.c_str(), pdol.length(), tlEntities, tlEntitiesCount);

     //����PDOL��Ӧ�����ֶ�
     string gpoData;
     for (unsigned int i = 0; i < tlEntitiesCount; i++)
     {
        string value = CTerminal::GetTerminalData((char*)tlEntities[i].tag);
        if (value.empty())
        {
            Log->Error("terminal tag %s is empty string", tlEntities[i].tag);
            return false;
        }
        gpoData += value;
     }

     //����GPO����
     APDU_RESPONSE response;
     if (!m_pAPDU->GPOCommand(gpoData, response))
     {
         Log->Error("Send GPO command failed.");
         return false;
     }

     //������Ӧ����
     string responseData = string(response.data);
     string aip = responseData.substr(4, 4);
     string afl = responseData.substr(8);

     //��ʽ�������Ӧ����
     Log->Info("[82]=%s", aip.c_str());
     Log->Info("[94]=%s", afl.c_str());

     SaveTag("82", aip);
     SaveTag("94", afl);

     return true;
}

 /***************************************************************
 * ���ܣ� ��ȡAFL��¼���ݣ�Ҳ���Ǹ�Ӧ����Ҫ�����ݡ�
 ****************************************************************/
 bool CommTransaction::ReadApplicationData()
 {
     Log->Info("======================== ��Ӧ������ ��ʼ =================================");
     //��ȡAFL
     string strAFL = GetTagValue(Tag94);
     if (strAFL.empty())
     {
         Log->Error(_T("tag 94 can not be found!"));
         return false;
     }
     //����AFL
     vector<AFL> vecAFL;
     ParaseAFL(strAFL, vecAFL);

     //��ȡÿ��AFL��¼
     for (auto v : vecAFL)
     {        
        char szSFI[3] = { 0 };
        char szReadRecord[3] = { 0 };
        APDU_RESPONSE response;

        sprintf_s(szSFI, "%02X", v.SFI);        
        sprintf_s(szReadRecord, "%02X", v.nRecordNo);
        
        if (!m_pAPDU->ReadRecordCommand(szSFI, szReadRecord, response))
        {
            Log->Error("��ȡAFL��¼ʧ��!");
            return false;
        }
        //���澲̬ǩ������
        if (v.bAcceptAuthencation)
        {           
            string needSigData = response.data;
            if (Tool::Converter::HexToInt(needSigData[2]) & 0x08)   //���λ1
            {                
                unsigned int lengthSize = 2 * ((needSigData[2] & 0x07) * 8 + (needSigData[3] & 0x0f));
				m_acceptAuthData += needSigData.substr(lengthSize + 4);
            }
            else
            {
				m_acceptAuthData += needSigData.substr(4);
            }
        }

        //����tag
        BCD_TLV entites[32] = { 0 };
        unsigned int entitiesCount = 0;

        m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);
        PrintTags(entites, entitiesCount);
        SaveTag(entites, entitiesCount);
    }

    return true;
 }

 //У�龲̬Ӧ������
 bool CommTransaction::SDA(string issuerPublicKey, ENCRYPT_TYPE encryptType)
 {
     string signedStaticAppData = GetTagValue(Tag93);
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
         string sigDataList = GetTagValue("9F4A");
         if (!sigDataList.empty() && sigDataList != "82")
         {
             return false;
         }

         string hashDataInput = strRecoveryData.substr(2, recoveryDataLen - 44) + m_acceptAuthData + GetTagValue("82");
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
             string needValidateData = signedStaticAppData.substr(0, 6) + m_acceptAuthData + GetTagValue("82");
             int ret = DllPBOC_SM2_Verify((char*)issuerPublicKey.c_str(), (char*)needValidateData.c_str(), (char*)signedResult.c_str());
             if (ret == SM_OK)
             {
                 return true;
             }
         }
     }

     return false;
 }

 bool CommTransaction::DDA(string ICCPublicKey, ENCRYPT_TYPE encryptType)
 {
     string terminalData;
     string DDOL = GetTagValue("9F49");
     BCD_TL tlEntity[10] = { 0 };
     unsigned int entitiesCount = 0;
     m_pParaser->TLVConstruct((unsigned char*)DDOL.c_str(), DDOL.length(), tlEntity, entitiesCount);
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
     if (Tool::Converter::HexToInt(strResponse[2]) & 0x08)
     {
         //���λ1
         unsigned int lengthSize = 2 * ((strResponse[2] & 0x07) * 8 + (strResponse[3] & 0x0f));
         SaveTag("9F4B", strResponse.substr(lengthSize + 4));

     }
     else
     {
         SaveTag("9F4B", strResponse.substr(4));
     }

     string signedData = GetTagValue("9F4B");
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
 bool CommTransaction::OfflineDataAuth()
 {
     Log->Info("======================== �ѻ�������֤ ��ʼ =================================");

     string issuerPublicCert    = GetTagValue(Tag90);           //�����й�Կ֤��
     string caIndex             = GetTagValue(Tag8F);           //CA��Կ���� PKI
     string issuerExponent      = GetTagValue(Tag9F32);         //�����й�Կָ��
     string ipkRemainder        = GetTagValue(Tag92);           //�����й�Կ����
     string rid                 = GetTagValue(Tag4F).substr(0, 10); //RID    

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

     if (!SDA(issuerPublicKey, m_encryptType))
     {
         Log->Info("SDA validate failed.");
         return false;
     }
     Log->Info("SDA validate success.");
     if (m_authType == AUTHENCATE_TYPE::AUTH_DDA)
     {
         //��̬������֤	
         string iccPublicCert = GetTagValue(Tag9F46);    //ICC��Կ֤��       
         string signedData = m_acceptAuthData + GetTagValue(Tag82);   //ǩ������
         string ICCPublicKey;
         if (m_encryptType == ENCRYPT_TYPE::ENCRYPT_DES)
         {
             string iccRemainder = GetTagValue(Tag9F48); //ICC��Կ����
             string iccExponent = GetTagValue(Tag9F47);  //ICC��Կָ��
             ICCPublicKey = kg->GenDesICCpublicKey(issuerPublicKey, iccPublicCert, iccRemainder, signedData, issuerExponent);
         }
         else {
             ICCPublicKey = kg->GenSMICCpublicKey(issuerPublicKey, iccPublicCert, signedData);
         }

         Log->Debug("ICC public key=[%s]", ICCPublicKey.c_str());

         if (!DDA(ICCPublicKey, m_encryptType))
         {
             Log->Info("DDA validate failed.");
             return false;
         }
         Log->Info("DDA validate success.");
         return true;
     }
     return true;
 }


 bool CommTransaction::HandleLimitation()
 {
	 return false;
 }
 bool CommTransaction::CardHolderValidation()
 {
	 return false;
 }
 bool CommTransaction::TerminalRiskManagement()
 {
	 return false;
 }
 bool CommTransaction::CardActionAnalized()
 {
	 return false;
 }
 bool CommTransaction::TerminalActionAnalized()
 {
	 return false;
 }
 bool CommTransaction::OnlineBussiness()
 {
	 return false;
 }
 bool CommTransaction::HandleIusserScript()
 {
	 return false;
 }
 bool CommTransaction::EndTransaction()
 {
	 return false;
 }

 /********************************************************************
 * ���ܣ� ��ȡָ��tagֵ������
 *********************************************************************/
 string CommTransaction::ReadTagValueFromCard(const string &tag)
 {
     APDU_RESPONSE response;
     m_pAPDU->ReadTagCmd(tag, response);
     if (response.SW1 == 0x90 && response.SW2 == 0x00)
     {
         return string(response.data);
     }

     return _T("");
 }
