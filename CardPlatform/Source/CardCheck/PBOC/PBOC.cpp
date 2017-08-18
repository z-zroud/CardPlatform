#include "stdafx.h"
#include "PBOC.h"
#include "Interface\InterfaceInstance.h"
#include "Util\Log.h"
#include "Util\Des0.h"
#include "Util\SHA1.h"
#include "Util\SM.hpp"
#include "Util\Converter.h"
#include "Util\Base.h"
#include "..\Terminal.h"



PBOC::PBOC(COMMUNICATION_TYPE type, IPCSC* reader) : CommTransaction(reader)
{
    m_commType = type;
	m_IsOnlineAuthSucessed = false;

    SetCommunicationType(type);
}

//��Ҫʵ������Щ����Ҫʹ��APDUָ��Ķ��������ѻ�������֤
PBOC::PBOC()
{
    //m_pReader = NULL;
    //m_pAPDU = NULL;
    //m_parser = new TLVParaser();
    //m_IsOnlineAuthSucessed = false;
}

PBOC::~PBOC()
{
}

void PBOC::DoTrans()
{
    SelectApplication();        //��һ���� Ӧ��ѡ��
    InitilizeApplication();     //�ڶ����� Ӧ�ó�ʼ��
    ReadApplicationData();      //�������� ��Ӧ������
    OfflineDataAuth();          //���Ĳ��� �ѻ�������֤
    HandleLimitation();         //���岽�� ��������
    CardHolderValidation();     //�������� �ֿ�����֤
    TerminalRiskManagement();   //���߲��� �ն˷��չ���  
    TerminalActionAnalized();   //�ڰ˲��� �ն���Ϊ����
    CardActionAnalized();       //�ھŲ��� ��Ƭ��Ϊ����
    OnlineBussiness();          //��ʮ���� ��������֤
    EndTransaction();           //��ʮһ���� ���׽�������

    if (m_bExecScript)
    {
        HandleIusserScript();   //��ʮ������ �����нű�����
    }
}

void PBOC::Clear()
{
    CommTransaction::Clear();
}

void PBOC::SetScript(const string& tag, const string& value)
{
    if(value.size() > 0)
        m_vecECLoadScript.push_back(pair<string, string>(tag, value));
}

void PBOC::ExecScript(bool bExecScript)
{
    m_bExecScript = bExecScript;
}

void PBOC::SetCommunicationType(COMMUNICATION_TYPE type)
{
    
    switch (type)
    {
    case COMM_TOUCH:
        SelectPSE(APP_TYPE::APP_PSE);
        break;
    case COMM_UNTOUCH:
        SelectPSE(APP_TYPE::APP_PPSE);
        break;
    default:
        break;
    }
}

//��鿨ƬAUC��Ϣ
void PBOC::ShowCardAUC(string AUC)
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
bool PBOC::HandleLimitation()
{
	Log->Info("======================== �������� ��ʼ =================================");
	//�����Ч����
	string effectDate = GetTagValue(Tag5F25);
	string transDate = Base::GenTransDate();
	if (!Base::CompareDate(transDate, effectDate))
	{
		Log->Error("��Ч���ڴ���: ��Ч����:[%s], ��������[%s]", effectDate.c_str(), transDate.c_str());
		m_tvr.ApplicationNotEffect = true;
	}
	else {
		Log->Info("��Ч������֤�ɹ�");
	}
	
	//���ʧЧ����
	string expireDate = GetTagValue(Tag5F24);
	if (!Base::CompareDate(expireDate, transDate))
	{
		Log->Error("ʧЧ���ڴ���: ʧЧ����:[%s],��������[%s]", expireDate.c_str(), transDate.c_str());
		m_tvr.ApplicationExpired = true;
	}
	else {
		Log->Info("ʧЧ������֤�ɹ�");
	}

	//���Ӧ�ð汾��
	string termApplicationVersion = CTerminal::GetTerminalData(Tag9F09);
	string cardApplicationVersion = GetTagValue(Tag9F08);
	if (termApplicationVersion != cardApplicationVersion)
	{
		Log->Info("Ӧ�ð汾�ż��ʧ��!��ƬӦ�ð汾��:[%s],�ն�Ӧ�ð汾��:[%s]", cardApplicationVersion, termApplicationVersion);
		m_tvr.ApplicationVersionFailed = true;
	}
	else {
		Log->Info("Ӧ�ð汾����֤�ɹ�! ��Ƭ�汾��[%s]", cardApplicationVersion.c_str());
	}

	//���AUC
	string AUC = GetTagValue(Tag9F07);
	ShowCardAUC(AUC);

	return false;
}

//��ʾCVM����
void PBOC::ShowCVMMethod(int method)
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
void PBOC::ShowCVMCondition(int condition)
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
bool PBOC::CardHolderValidation()
{
	Log->Info("======================== �ֿ�����֤ ��ʼ =================================");
	CVM cvm;
	string strCVM = GetTagValue(Tag8E);

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
bool PBOC::TerminalRiskManagement()
{
	Log->Info("======================== �ն˷��չ��� ��ʼ =================================");
	//����޶���
	string transMoney = ReadTagValue("81");
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
	string lastOnlineATC = GetTagValue("9F13");
	string ATC = GetTagValue("9F36");
	string offlineTransFloorLimit = GetTagValue("9F23");
	string offlineTransUpLimit = GetTagValue("9F14");

	int nOfflineTransFloorLimit = stoi(offlineTransFloorLimit, 0, 16);
	int nOfflineTransUpLimit = stoi(offlineTransUpLimit, 0, 16);

	if (lastOnlineATC.empty() || ATC.empty())
	{
		Log->Error("Ƶ�ȼ��ʧ��!�޷���ȡ�ϴ�����ATC����Ӧ�ý���ATC");
		m_tvr.ExceedContinuedTransUpLimitation = true;
		m_tvr.ExceedContinuedTransFloorLimitation = true;
        ATC = ATC.empty() ? _T("00") : ATC;
        lastOnlineATC = lastOnlineATC.empty() ? _T("00") : lastOnlineATC;
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

void PBOC::ParseGACResponseData(const string buffer)
{
	SaveTag("9F27", string(buffer.substr(0, 2)));   //������Ϣ���� ������Ƭ���ص��������Ͳ�ָ���ն�Ҫ���еĲ���
	SaveTag("9F36", string(buffer.substr(2, 4)));   //Ӧ�ý��׼�����(ATC)
	SaveTag("9F26", string(buffer.substr(6, 16)));  //Ӧ������ 8�ֽ�
	SaveTag("9F10", string(buffer.substr(22))); //������Ӧ������ ��һ�����������У�Ҫ���͵������е�ר��Ӧ�����ݡ�
}

void PBOC::ShowCardTransType(const string transType)
{
    string cardTransType;
    if (transType.empty())
    {
        cardTransType  = GetTagValue("9F27");
    }
    else {
        cardTransType = transType.substr(0,2);
    }

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
bool PBOC::CardActionAnalized()
{
	Log->Info("======================== ��Ƭ��Ϊ���� ��ʼ =================================");
	GACControlParam P1;
	switch (m_termTransType)
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
	
    //��Ƭ��Ϊ����֮ǰ����Ҫ���ն���֤������������������ݵ���Ƭ��
    CTerminal::SetTerminalData(Tag95, GetTVR());

	/******************************** GAC 1 ******************************/
	string CDOL1 = GetTagValue("8C");
	BCD_TL CDOL1tlEntities[32] = { 0 };
	unsigned int CDOL1tlEntitiesCount = 0;
	m_pParaser->TLVConstruct((unsigned char*)CDOL1.c_str(), CDOL1.length(), CDOL1tlEntities, CDOL1tlEntitiesCount);
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
    m_pParaser->TLVConstruct((unsigned char*)response1.data, strlen(response1.data), entites, entitiesCount);

	//��ʽ�������Ӧ����
	PrintTags(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//����GAC ��Ƭ��Ӧ����
	ParseGACResponseData(GetTagValue("80"));
	ShowCardTransType();

	return true;
}

//�Ƚ�IAC��TAC���
bool PBOC::CompareIACAndTVR(string IAC, string strTVR)
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

TERM_TRANS_TYPE PBOC::GetTermAnanlizedResult()
{
	TERM_TRANS_TYPE type = TERM_TRANS_TYPE::TERM_TC;
	string IACDefault = GetTagValue("9F0D");
	string IACRejection = GetTagValue("9F0E");
	string IACOnline = GetTagValue("9F0F");

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
bool PBOC::TerminalActionAnalized()
{
	Log->Info("======================== �ն���Ϊ���� ��ʼ =================================");
	m_termTransType = GetTermAnanlizedResult();

	return false;
}

/**************************************************************
* ���ܣ������ѻ�������֤���ն˷��չ���Ȳ��裬�����ն���Ϊ����(TAC)
***************************************************************/
string PBOC::GetTVR()
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

/***************************************************************
* ���ܣ���ȡ���˺�
****************************************************************/
string PBOC::GetAccount()
{
	string tag57 = GetTagValue(Tag57);
	int index = tag57.find('D');
	if (index == string::npos)
	{//δ�ҵ���ֱ�ӷ��ؿ�
		return _T("");
	}
	return tag57.substr(0, index);
}

/******************************************************************
* ����������������ʹ�÷���������ϵͳ�еķ��չ�������Խ��׽��м�飬��
* ����׼��ܾ����׵ľ�����
* �����е���Ӧ������Ƭ�Ķ��η������º�һ�����������ɵ����ġ���Ƭ��֤����
* ȷ����Ӧ����һ����Ч�ķ����С�����֤�з�������֤��
*******************************************************************/
bool PBOC::OnlineBussiness()
{
	Log->Info("======================== �������� ��ʼ =================================");
	string CID = GetTagValue(Tag9F27);		// 	������Ϣ����
	string ATC = GetTagValue(Tag9F36);
	string AC = GetTagValue(Tag9F26);		//Ӧ�����ģ�AC��
	string issuerApplicationData = GetTagValue(Tag9F10);	//������Ӧ������
	
	APDU_RESPONSE response;
    string strARPC;
    IKeyGenerator *pKg = GetKeyGeneratorInterface();

    if (!m_udkAuth.empty()) {
        strARPC = pKg->GenARPCByUdkAuth(m_udkAuth, AC, "3030", ATC);
    }
    else if (!m_mdkAuth.empty()) {
        string cardSeq = GetTagValue(Tag5F34);
        string account = GetAccount();
        strARPC = pKg->GenARPCByMdkAuth(m_mdkAuth, AC, "3030", ATC, cardSeq, account);
    }
    else {
        Log->Error("�������� ����ARPCʧ��");
        return false;
    }
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

void PBOC::ParseTransLog(const string buffer)
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

/*************************************************************
* ���ܣ� �������нű�
**************************************************************/
bool PBOC::HandleIusserScript()
{
    IKeyGenerator *pKg = GetKeyGeneratorInterface();
    string atc = GetTagValue(Tag9F36);
    string appCip = GetTagValue(Tag9F26);
    if (!m_mdkMac.empty())
    {
        string cardSeq = GetTagValue(Tag5F34);
        string account = GetAccount();
        m_udkMac = pKg->GenUDKAuthFromMDKAuth(m_mdkMac, atc, account, cardSeq);
    }
    if (m_udkMac.empty())
    {
        Log->Error(_T("MAC ����Ϊ��"));
        return false;
    }

    //ִ�е����ֽ���ؽű�
    for (auto v : m_vecECLoadScript)
    {
        string len = Base::GetDataHexLen(v.second);
        len = Base::Increase(len, 4);
        string data = _T("04DA") + v.first + len + atc + appCip + v.second;
        if (data.length() % 16 != 0)
        {
            data += _T("80");
            if (data.length() % 16 != 0)
            {
                int absent = 16 - data.length() % 16;
                data.append(absent, '0');
            }
        }
        string macResult = pKg->GenScriptMac(m_udkMac, atc, data);
        if (!m_pAPDU->PutDataCommand(v.first, v.second, macResult))
        {
            Log->Error(_T("ִ�нű�ʧ��"));
        }
    }
    
	return true;
}

void PBOC::ShowLog()
{
    //��ȡ�����ֽ�Ȧ����־���
    string logCashEntry = GetTagValue(TagDF4D);
    string logEntry = GetTagValue(Tag9F4D);

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
}

//���׽���
bool PBOC::EndTransaction()
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
    //����GAC2ʱ����Ҫ�����ն˴�������
    CTerminal::SetTerminalData(Tag95, GetTVR());
	/******************************** GAC 2 ******************************/
	string CDOL2 = GetTagValue(Tag8D);
	BCD_TL CDOL2tlEntities[32] = { 0 };
	unsigned int CDOL2tlEntitiesCount = 0;
    m_pParaser->TLVConstruct((unsigned char*)CDOL2.c_str(), CDOL2.length(), CDOL2tlEntities, CDOL2tlEntitiesCount);
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
    m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//��ʽ�������Ӧ����
	PrintTags(entites, entitiesCount);
	//SaveTag(entites, entitiesCount);  //���������Ӧֵ���Ḳ�ǵ�һ��GAC��Ӧ�����ű�������Ҫ��һ��Ӧ�����ġ�  
	ShowCardTransType(string((char*)entites[0].Value));

	return true;
}
