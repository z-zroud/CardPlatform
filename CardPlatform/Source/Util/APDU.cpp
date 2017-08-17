#include "stdafx.h"
#include "APDU.h"
#include "Interface\CardBase.h"
#include "Des0.h"
#include "Log.h"
#include "StringParaser.h"
#include "Converter.h"
#include "Util\Base.h"

APDU::APDU(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol)
{
	m_scardHandle = scardHandle;
	m_dwActiveProtocol = protocol;
}
bool APDU::SelectApplicationCommand(const string &aid, APDU_RESPONSE &response)
{
	if (aid.length() % 2 != 0)
	{
		return false;	//AID����
	}
	char szLen[3] = { 0 };
	sprintf_s(szLen, 3, "%02X", aid.length() / 2);
	string strCommand = "00A40400" + string(szLen) + aid;
	return SendAPDU(strCommand, response);
}

//select PSE
bool APDU::SelectPSECommand(APDU_RESPONSE &response)
{
	string strCommand = "00A40400 0E 315041592E5359532E4444463031";

	return SendAPDU(strCommand, response);
}

//select PPSE
bool APDU::SelectPPSECommand(APDU_RESPONSE &response)
{
	string strCommand = "00A40400 0E 325041592E5359532E4444463031";

	return SendAPDU(strCommand, response);
}

//Initialize Update
bool APDU::InitializeUpdateCommand(string strDiv, APDU_RESPONSE &response)
{
	if (strDiv.length() != 16)
	{
		return false;
	}
	string strCommand = string("8050000008") + strDiv;

	return SendAPDU(strCommand, response);
}

/******************************************************
* ���ݸ�����ƥ��/���ұ�׼ȡ�÷����߰�ȫ�򡢿�ִ��װ���ļ���
* ��ִ��ģ�顢Ӧ�úͰ�ȫ����������ڵ�״̬��Ϣ��
*******************************************************/
bool APDU::GetApplicationStatusCommand(vector<APP_STATUS> &status)
{
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));

	string cmd = "80F24000 02 4F00";

	if (SendAPDU(cmd, response))
	{
		string strResponse = response.data;
		unsigned int i = 0;
		while (i < strResponse.length())
		{
			APP_STATUS applicationStatus;
			int len = 2 * stoi(strResponse.substr(i, 2), 0, 16);
			applicationStatus.strAID = strResponse.substr(i + 2, len);
			applicationStatus.lifeCycleStatus = strResponse.substr(i + 2 + len, 2);
			applicationStatus.privilege = strResponse.substr(i + 4 + len, 2);
			status.push_back(applicationStatus);

			i += 6 + len;
		}

		return true;
	}
	
	return false;
}

//ɾ����ִ��װ���ļ���Ӧ�õȵ�
bool APDU::DeleteCommand(string id)
{
	char szDataLen[3] = { 0 };
	char szAllLen[3] = { 0 };
	sprintf_s(szDataLen, "%02X", id.length() / 2);
	sprintf_s(szAllLen, "%02X", id.length() / 2 + 2);
	string cmd = "80E40000" + string(szAllLen) + "4F" + szDataLen + id;
	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));

	SendAPDU(cmd, response);
    //Log->Info("%s", cmd.c_str());
	if (response.SW1 != 90 && response.SW2 != 0x00)
	{
		return false;
	}

	return true;
}

bool APDU::StorePSEData(string data, STORE_DATA_TYPE dataType, bool bReset)
{
    static int count = -1;		//���ڼ���
    if (bReset)
    {
        count = 0;	//���ü�����
    }
    else {
        count++;
    }
    string cmd = "80E2";
    switch (dataType)
    {
    case STORE_DATA_COMMON:		cmd += "00"; break;
    case STORE_DATA_ENCRYPT:	cmd += "60"; break;
    case STORE_DATA_LAST:		cmd += "80"; break;
    }
    char szCount[3] = { 0 };
    sprintf_s(szCount, "%02X", count);
    cmd += string(szCount);     //����Install Dataͷ������

    char szLen[3] = { 0 };
    int len = data.length() / 2;
    sprintf_s(szLen, 3, "%02X", len);

    cmd += szLen + data;    // + data len + data

    APDU_RESPONSE response;
    if (!SendAPDU(cmd, response) || (response.SW1 != 0x90 && response.SW2 != 0x00))
    {
        return false;     // ���˻�PSEʧ��
    }

    return true;
}

//������������ ���ڸ��˻�
bool APDU::StoreDataCommand(string DGI, string DGIData, STORE_DATA_TYPE dataType, bool bReset)
{
	static int count = -1;		//���ڼ���
	if (bReset)
	{
		count = 0;	//���ü�����
    }
    else {
        count++;
    }
	char szCount[3] = { 0 };
	sprintf_s(szCount, "%02X", count);
	string cmd = "80E2";
	switch (dataType)
	{
	case STORE_DATA_COMMON:		cmd += "00"; break;
	case STORE_DATA_ENCRYPT:	cmd += "60"; break;
	case STORE_DATA_LAST:		cmd += "80"; break;
	}
	cmd += string(szCount);     //����Install Dataͷ������

   /* char szDataLen[5] = { 0 };
    sprintf_s(szDataLen, 5, "%02X", DGIData.length() / 2);*/

    string sDataLen = Base::GetDataHexLen(DGIData);
    //���DGI�������ݹ�������Ҫ���λ����ϴ�(������������δ洢���������ε��ݲ�����(δ�����������))
    if (DGIData.length() / 2 >= 0xFE)
    {
        string data1 = DGI + sDataLen + DGIData.substr(0, 0xDD * 2);    // DGI + �����ݳ��� + ��һ���洢��data����
        string cmdFirst = cmd + Base::GetDataHexLen(data1) + data1;

        count++;
        memset(szCount, 0, sizeof(szCount));
        sprintf_s(szCount, "%02X", count);

        //����ͷ + �ڶ��������ݳ��� + �ڶ��δ洢data
        string data2 = DGIData.substr(0xDD * 2);
        string cmdSecond = cmd.substr(0, 6) + szCount + Base::GetDataHexLen(data2) + data2;

        APDU_RESPONSE response;
        if (SendAPDU(cmdFirst, response) && (response.SW1 == 0x90 && response.SW2 == 0x00))
        {
            return SendAPDU(cmdSecond, response) && (response.SW1 == 0x90 && response.SW2 == 0x00);
        }
        return false;
    }
    else {
       

        char szTotalLen[5] = { 0 };
        sprintf_s(szTotalLen, 5, "%02X", (DGI.length() + DGIData.length()) / 2 + 1);
        cmd += szTotalLen + DGI;        // header + totalLen + DGI

        cmd += string(sDataLen) + DGIData; // + DGI data len + DGI data

        APDU_RESPONSE response;
        if (SendAPDU(cmd, response) && (response.SW1 == 0x90 && response.SW2 == 0x00))
        {
            return true;
        }
    }


	return false;
}

// install command
bool APDU::InstallCommand(string exeLoadFile, string exeModule, string application, string privilege, string installParam, string token)
{
	char szExeLoadFileAIDLen[3] = { 0 };	//��ִ�а�װ�ļ�AID�ĳ���
	char szExeModuleAIDLen[3] = { 0 };	//��ִ��ģ��AID�ĳ���
	char szApplicationAIDLen[3] = { 0 };	//Ӧ��AID�ĳ���
	char szPrivilegeLen[3] = { 0 };		//Ӧ��Ȩ�޵ĳ���
	char szInstallParamLen[3] = { 0 };	//��װ�����ĳ���
	char szTokenLen[3] = { 0 };	//��װToken�ĳ���
	char szTotalLen[3] = { 0 };	//�����ܳ���
	sprintf_s(szExeLoadFileAIDLen, "%02X", exeLoadFile.length() / 2);
	sprintf_s(szExeModuleAIDLen, "%02X", exeModule.length() / 2);
	sprintf_s(szApplicationAIDLen, "%02X", application.length() / 2);
	sprintf_s(szPrivilegeLen, "%02X", privilege.length() / 2);
	sprintf_s(szInstallParamLen, "%02X", installParam.length() / 2);
	sprintf_s(szTokenLen, "%02X", token.length() / 2);

	sprintf_s(szTotalLen, "%02X", 6 + exeLoadFile.length() / 2 +
		exeModule.length() / 2 +
		application.length() / 2 +
		privilege.length() / 2 +
		installParam.length() / 2 +
		token.length() / 2);

	string cmd = "80E60C00" + string(szTotalLen) + 
		string(szExeLoadFileAIDLen) + exeLoadFile +
		string(szExeModuleAIDLen) + exeModule +
		string(szApplicationAIDLen) + application +
		string(szPrivilegeLen) + privilege +
		string(szInstallParamLen) + installParam +
		string(szTokenLen) + token;

	APDU_RESPONSE response;
	memset(&response, 0, sizeof(response));
    //Log->Info("%s", cmd.c_str());
	if (!SendAPDU(cmd, response))
	{
		return false;
	}
	if (strcmp(response.data, "00") == 0)
	{
		return true;
	}

	return false;
}

// Get Tag
bool APDU::GetTag(const string &tag, APDU_RESPONSE &response)
{
	string strCommand;
	if (tag.length() != 2 && tag.length() != 4)
	{
		return false;
	}
	if (tag.length() == 2)
	{
		strCommand = "80CA00" + tag;
	}
	else {
		strCommand = "80CA" + tag;
	}

	return SendAPDU(strCommand, response);
}

//�ڲ���֤ ����
bool APDU::InternalAuthCommand(const string &randNumber, APDU_RESPONSE &response)
{
	char szLen[16] = { 0 };
	sprintf_s(szLen, "%02X", randNumber.length() / 2);
	string strCommand = "00880000" + string(szLen) + randNumber;

	return SendAPDU(strCommand, response);
}

//�ⲿ��֤ ����
bool APDU::ExternalAuthCommand(const string cardRandomNum,
	const string termRandomNum,
	const string sessionAuthKey,
	const string seesionMacKey,
	SECURE_LEVEL nSecureLevel,
	int nSCP,
	APDU_RESPONSE &response)
{
	char szMac[17] = { 0 };
	string strCommand;
	string input = cardRandomNum + termRandomNum;
	Full_3DES_CBC_MAC((char*)input.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac);
	switch (nSecureLevel)
	{
	case SL_NO_SECURE:	strCommand = "8482000010"; break;
	case SL_MAC:		strCommand = "8482010010"; break;
	case SL_MAC_ENC:	strCommand = "8482030010"; break;
	default:
		return false;
	}
	strCommand += szMac;
	memset(szMac, 0, sizeof(szMac));
	switch (nSCP)
	{
	case 1: Full_3DES_CBC_MAC((char*)strCommand.c_str(), (char*)sessionAuthKey.c_str(), "0000000000000000", szMac); break;
	case 2: Common_MAC((char*)strCommand.c_str(), (char*)seesionMacKey.c_str(), "0000000000000000", szMac); break;
	default:
		return false;
	}
	strCommand += szMac;

	return SendAPDU(strCommand, response);
}

//�ⲿ��֤
bool APDU::ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response)
{
	if (ARPC.length() != 16 && authCode.length() != 4)
	{
		return false;
	}
	string cmd = "008200000A" + ARPC + authCode;

	return SendAPDU(cmd, response);
}

bool APDU::ReadRecordCommand(const string &SFI, const string strRecordNumber, APDU_RESPONSE &response)
{
	int temp = (stoi(SFI, 0, 16) << 3) + 4;
	char szP2[3] = { 0 };

	sprintf_s(szP2, "%02X", temp);

	string strCommand = "00B2" + strRecordNumber + string(szP2);

	return SendAPDU(strCommand, response);

}
bool APDU::PutDataCommand(const string &tag, const string &value, const string &mac)
{
    if (mac.length() % 8 != 0)
    {
        return false;
    }
    string cmd = _T("04DA");
    cmd += tag;
    string dataLen = Base::GetDataHexLen(value + mac);
    cmd += dataLen + value + mac;
    APDU_RESPONSE response;

    return SendAPDU(cmd, response) && (response.SW1 == 0x90 && response.SW2 == 0x00);
}

bool APDU::PutKeyCommand(const string keyVersion,
    const string authKeyWithKcv,
    const string macKeyWithKcv,
    const string encKeyWithKcv)
{
    string cmd = "80D8";
    if (keyVersion == "FF")
    {
        cmd += "00" + string("8143") + string("01");
    }
    else {
        cmd += keyVersion + "8143" + keyVersion;
    }
    cmd += authKeyWithKcv + macKeyWithKcv + encKeyWithKcv;

    APDU_RESPONSE response;
    memset(&response, 0, sizeof(response));

    return SendAPDU(cmd, response);
}

bool APDU::UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response)
{
	return false;
}

//��ȡ����ѡ�� GPO����
bool APDU::GPOCommand(const string &strCommand, APDU_RESPONSE &response)
{

	int dataLen = strCommand.length() / 2;
	int totalLen = dataLen + 2;
	char szTotalLen[3] = { 0 };
	char szDataLen[3] = { 0 };

	sprintf_s(szTotalLen, "%02X", totalLen);
	sprintf_s(szDataLen, "%02X", dataLen);

	string cmd = "80A80000" + string(szTotalLen) + "83" + string(szDataLen) + strCommand;

	return SendAPDU(cmd, response);
}

//����Ӧ������ ����
bool APDU::GACCommand(GACControlParam parm1, const string terminalData, APDU_RESPONSE &response)
{
	char szDataLen[3] = { 0 };
	sprintf_s(szDataLen, "%02X", terminalData.length() / 2);
	string P1;
	switch (parm1)
	{
	case AAC:
		P1 = "00";
		break;
	case ARQC:
		P1 = "80";
		break;
	case TC:
		P1 = "40";
		break;
	case CDA:
		P1 = "10";
		break;
	default:
		break;
	}
	string cmd = "80AE" + P1 + "00" + szDataLen + terminalData;

	return SendAPDU(cmd, response);
}

bool APDU::WriteDataCommand(const string &strCommand, APDU_RESPONSE &response)
{
	return false;
}

//APDU��Ӧ����
bool APDU::GetAPDUResponseCommand(unsigned int len, unsigned char* szResponse, DWORD* pResponseLen)
{
	char szLen[3] = { 0 };
	//_itoa_s(len, szLen, 16);
	sprintf_s(szLen, "%02X", len);
	string szCommand = "00C00000" + string(szLen);
	if (*pResponseLen < len)
	{
		m_Error = "����APDU��Ӧ���ݵĻ�������С";
		return false;
	}
	return SendAPDU(szCommand, szResponse, pResponseLen);
}

//����APDU����
bool APDU::SendAPDU(string &strCommand, APDU_RESPONSE &response)
{
	bool bResult = false;
	unsigned char SW1, SW2;

	const int outputLen = 1024;
	DWORD dwResponseLen = outputLen;
	unsigned char szResponse[outputLen] = { 0 };
    Log->Info("%s", strCommand.c_str());
	if (!SendAPDU(strCommand, szResponse, &dwResponseLen))
	{
		return false;
	}

	SW1 = szResponse[dwResponseLen - 2];
	SW2 = szResponse[dwResponseLen - 1];

	if (SW1 == 0x61)	//ͨ��00c0�����ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
	{
		memset(szResponse, 0, outputLen);
		dwResponseLen = outputLen;
		bResult = GetAPDUResponseCommand(SW2, szResponse, &dwResponseLen);
		SW1 = szResponse[dwResponseLen - 2];
		SW2 = szResponse[dwResponseLen - 1];
	}
	else if (SW1 == 0x6C)	//ͨ��ԭ�ȵ������ȡ��Ӧ����, SW2Ϊ��Ӧ���ݷ��صĳ���
	{
		memset(szResponse, 0, outputLen);
		dwResponseLen = outputLen;
		char temp[3] = { 0 };
		_itoa_s(SW2, temp, 16);
		sprintf_s(temp, 3, "%02X", SW2);
		strCommand.append(temp);
		bResult = SendAPDU(strCommand, szResponse, &dwResponseLen);
		SW1 = szResponse[dwResponseLen - 2];
		SW2 = szResponse[dwResponseLen - 1];
	}
	else {
		bResult = true;
	}
	memset(response.data, 0, MAX_DATA_LEN);
	Tool::Converter::BcdToAsc(response.data, (char*)szResponse, dwResponseLen * 2 - 4);
	response.SW1 = SW1;
	response.SW2 = SW2;

	return bResult & (SW1 == 0x90 && SW2 == 0x00);
}

bool APDU::SendAPDU(string strCommand, unsigned char* szResponse, DWORD* pResponseLen)
{
	HRESULT hRet = S_FALSE;

	//ɾ���ַ����еĿո�
	strCommand = Tool::Stringparser::DeleteSpace(strCommand);
	if (strCommand.length() < 8 || strCommand.length() % 2 != 0)
	{
		Log->Error("APDU Command length error. [%s]", strCommand.c_str());
		return false;
	}
	unsigned char szAPDU[256] = { 0 };
	Tool::Converter::AscToBcd(szAPDU, (unsigned char*)strCommand.c_str(), strCommand.length());

	switch (m_dwActiveProtocol)
	{
	case T0:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T0, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	case T1:
		hRet = SCardTransmit(m_scardHandle, SCARD_PCI_T1, szAPDU, strCommand.length() / 2, NULL, szResponse, pResponseLen);
		break;
	default:
		Log->Warning("���ݴ���Э��δ֪ %d", m_dwActiveProtocol);
		return false;
	}
	if (hRet != SCARD_S_SUCCESS)
	{
		return false;
	}

	return true;
}

string APDU::GetAPDUError()
{
	return m_Error;
}

//��ȡAPDU������Ϣ
void APDU::GetAPDUError(unsigned char SW1, unsigned char SW2, char* szError, const int len)
{
	memset(szError, 0x00, len);
	switch (SW1)
	{
	case 0x61:
		sprintf_s(szError, len, "Command processed without error. The command prepared %02X bytes of data that can be retrieved using the Get Response command", SW2);
		break;
	case 0x62:
		if (SW2 == 0x81)
			sprintf_s(szError, len, "the relevant data may be corrupted");//FCI�ļ���������ȷѡDFʱ���ô�
		else
			sprintf_s(szError, len, "Unknown error!");
		break;
	case 0x63:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Authentication failed");
		else
		{
			SW2 = SW2 - 0xC0;
			sprintf_s(szError, len, "Authentication failed, %d tries left", SW2);
		}
		break;
	case 0x64:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Wrong context(for PK Crypto commands)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x65:
		if (SW2 == 0x81)
			sprintf_s(szError, len, "Write problem / Memory failure (ISO class byte)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x67:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Incorrect length (Lc or Le)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x69:
		switch (SW2)
		{
		case 0x00:sprintf_s(szError, len, "Unable to process"); break;
		case 0x01:sprintf_s(szError, len, "Command not allowed, invalid state"); break;
		case 0x81:sprintf_s(szError, len, "Command incompatible with file organization"); break;
		case 0x82:sprintf_s(szError, len, "Security status not satisfied"); break;
		case 0x83:sprintf_s(szError, len, "Authentication method blocked"); break;
		case 0x84:sprintf_s(szError, len, "Challenge not present or PIN is blocked"); break;
		case 0x85:sprintf_s(szError, len, "Conditions of use not satisfied"); break;//GemGold R3 PBOC EF15�ļ��Ƿ�֧��ED/EP��
		case 0x86:sprintf_s(szError, len, "No current EF selected"); break;
		case 0x87:sprintf_s(szError, len, "Secure messaging object missing (that is, MAC missing)"); break;
		case 0x88:sprintf_s(szError, len, "Secure messaging data object incorrect"); break;
		default:sprintf_s(szError, len, "Unkonwn error!"); break;
		}
		break;
	case 0x6A:
		switch (SW2)
		{
		case 0x80:sprintf_s(szError, len, "Incorrect parameters in the data field"); break;
		case 0x81:sprintf_s(szError, len, "Function not supported, invalidated file"); break;
		case 0x82:sprintf_s(szError, len, "File not found"); break;
		case 0x83:sprintf_s(szError, len, "Record not found"); break;
		case 0x84:sprintf_s(szError, len, "Not enough memory space in the file"); break;
		case 0x86:sprintf_s(szError, len, "Incorrect parameters(P1 & P2)"); break;
		case 0x88:sprintf_s(szError, len, "Referenced data not found"); break;
		default:sprintf_s(szError, len, "Unkonwn error!"); break;
		}
		break;
	case 0x6B:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Incorrect offset");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6C:
		sprintf_s(szError, len, "Repeat the command using Le=%02X", SW2);
		break;
	case 0x6D:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Unknown instruction code(command is not present)");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6E:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "Wrong instruction class given in the command");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x6F:
		if (SW2 == 0x00)
			sprintf_s(szError, len, "No data available for GET RESPONSE");
		else
			sprintf_s(szError, len, "Unkonwn error!");
		break;
	case 0x90:
		//sprintf(info,"����ִ�гɹ�");
		break;
	case 0x93:
		switch (SW2) {
		case 0x02:
			sprintf_s(szError, len, "Invalid MAC.");
			break;
		case 0x03:
			sprintf_s(szError, len, "Application permanently blocked.");
			break;
		default:
			sprintf_s(szError, len, "Unkonwn error!");
			break;
		}
		break;
	case 0x94:
		switch (SW2) {
		case 0x01:
			sprintf_s(szError, len, "Insufficient funds");
			break;
		case 0x02:
			sprintf_s(szError, len, "Transaction counter overflow");
			break;
		case 0x03:
			sprintf_s(szError, len, "Key Index not supported");//������Կ�����ڻ���TAC��Կ
			break;
		case 0x06:
			sprintf_s(szError, len, "Requested TAC/MAC not available");
			break;
		default:
			sprintf_s(szError, len, "Unkonwn error!");
			break;
		}
		break;
	default:
		sprintf_s(szError, len, "Unkonwn error!");
		break;
	}
	return;
}