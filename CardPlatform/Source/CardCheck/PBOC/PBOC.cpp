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

//主要实例化那些不需要使用APDU指令的对象，例如脱机数据认证
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
    SelectApplication();        //第一步： 应用选择
    InitilizeApplication();     //第二步： 应用初始化
    ReadApplicationData();      //第三步： 读应用数据
    OfflineDataAuth();          //第四步： 脱机数据认证
    HandleLimitation();         //第五步： 处理限制
    CardHolderValidation();     //第六步： 持卡人验证
    TerminalRiskManagement();   //第七步： 终端风险管理  
    TerminalActionAnalized();   //第八步： 终端行为分析
    CardActionAnalized();       //第九步： 卡片行为分析
    OnlineBussiness();          //第十步： 发卡行认证
    EndTransaction();           //第十一步： 交易结束处理

    if (m_bExecScript)
    {
        HandleIusserScript();   //第十二步： 发卡行脚本处理
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

//检查卡片AUC信息
void PBOC::ShowCardAUC(string AUC)
{
	if (AUC.length() < 2)
	{
		Log->Error("AUC format error. value[%s]", AUC.c_str());
		return;
	}
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
bool PBOC::HandleLimitation()
{
	Log->Info("======================== 处理限制 开始 =================================");
	//检查生效日期
	string effectDate = GetTagValue(Tag5F25);
	string transDate = Base::GenTransDate();
	if (!Base::CompareDate(transDate, effectDate))
	{
		Log->Error("生效日期错误: 生效日期:[%s], 交易日期[%s]", effectDate.c_str(), transDate.c_str());
		m_tvr.ApplicationNotEffect = true;
	}
	else {
		Log->Info("生效日期验证成功");
	}
	
	//检查失效日期
	string expireDate = GetTagValue(Tag5F24);
	if (!Base::CompareDate(expireDate, transDate))
	{
		Log->Error("失效日期错误: 失效日期:[%s],交易日期[%s]", expireDate.c_str(), transDate.c_str());
		m_tvr.ApplicationExpired = true;
	}
	else {
		Log->Info("失效日期验证成功");
	}

	//检查应用版本号
	string termApplicationVersion = CTerminal::GetTerminalData(Tag9F09);
	string cardApplicationVersion = GetTagValue(Tag9F08);
	if (termApplicationVersion != cardApplicationVersion)
	{
		Log->Info("应用版本号检查失败!卡片应用版本号:[%s],终端应用版本号:[%s]", cardApplicationVersion.c_str(), termApplicationVersion.c_str());
		m_tvr.ApplicationVersionFailed = true;
	}
	else {
		Log->Info("应用版本号验证成功! 卡片版本号[%s]", cardApplicationVersion.c_str());
	}

	//检查AUC
	string AUC = GetTagValue(Tag9F07);
	ShowCardAUC(AUC);

	return false;
}

//显示CVM方法
void PBOC::ShowCVMMethod(int method)
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
void PBOC::ShowCVMCondition(int condition)
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
bool PBOC::CardHolderValidation()
{
	Log->Info("======================== 持卡人验证 开始 =================================");
	CVM cvm;
	string strCVM = GetTagValue(Tag8E);

	if (strCVM.length() < 16)
	{
		Log->Error("CMV format error. value[%s]", strCVM.c_str());
		return false;
	}
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
bool PBOC::TerminalRiskManagement()
{
	Log->Info("======================== 终端风险管理 开始 =================================");
	//最低限额检查
	string transMoney = ReadTagValueFromCard("81");
	if (transMoney.empty())
	{
		//Log->Error("无法获取授权金额!");
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
	string lastOnlineATC = ReadTagValueFromCard("9F13");
	string ATC = ReadTagValueFromCard("9F36");
	string offlineTransFloorLimit = GetTagValue("9F23");
	string offlineTransUpLimit = GetTagValue("9F14");

	if (offlineTransFloorLimit.length() <= 0)
	{
		Log->Error("Offline trans floor limit format error. value[%s]", offlineTransFloorLimit.c_str());
		return false;
	}
	if (offlineTransUpLimit.length() <= 0)
	{
		Log->Error("Offline trans up limit format error. value[%s]", offlineTransUpLimit.c_str());
		return false;
	}

	int nOfflineTransFloorLimit = stoi(offlineTransFloorLimit, 0, 16);
	int nOfflineTransUpLimit = stoi(offlineTransUpLimit, 0, 16);

	if (lastOnlineATC.empty() || ATC.empty())
	{
		Log->Error("频度检查失败!无法获取上次联机ATC或者应用交易ATC");
		m_tvr.ExceedContinuedTransUpLimitation = true;
		m_tvr.ExceedContinuedTransFloorLimitation = true;
        ATC = ATC.empty() ? _T("00") : ATC;
        lastOnlineATC = lastOnlineATC.empty() ? _T("00") : lastOnlineATC;
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

void PBOC::ParseGACResponseData(const string buffer)
{
	if (buffer.length() < 22)
	{
		Log->Error("GAC return data incorrected. value [%s]", buffer.c_str());
		return;
	}
	SaveTag("9F27", string(buffer.substr(0, 2)));   //密文信息数据 表明卡片返回的密文类型并指出终端要进行的操作
	SaveTag("9F36", string(buffer.substr(2, 4)));   //应用交易计数器(ATC)
	SaveTag("9F26", string(buffer.substr(6, 16)));  //应用密文 8字节
	SaveTag("9F10", string(buffer.substr(22))); //发卡行应用数据 在一个联机交易中，要传送到发卡行的专有应用数据。
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
bool PBOC::CardActionAnalized()
{
	Log->Info("======================== 卡片行为分析 开始 =================================");
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
	
    //卡片行为分析之前，需要将终端验证结果保存下来，并传递到卡片中
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

	//解析卡片响应数据
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
    m_pParaser->TLVConstruct((unsigned char*)response1.data, strlen(response1.data), entites, entitiesCount);

	//格式化输出响应数据
	PrintTags(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	//分析GAC 卡片响应数据
	ParseGACResponseData(GetTagValue("80"));
	ShowCardTransType();

	return true;
}

//比较IAC和TAC结果
bool PBOC::CompareIACAndTVR(string IAC, string strTVR)
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
bool PBOC::TerminalActionAnalized()
{
	Log->Info("======================== 终端行为分析 开始 =================================");
	m_termTransType = GetTermAnanlizedResult();

	return false;
}

/**************************************************************
* 功能：根据脱机数据认证、终端风险管理等步骤，生成终端行为代码(TAC)
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
* 功能：获取主账号
****************************************************************/
string PBOC::GetAccount()
{
	string tag57 = GetTagValue(Tag57);
	int index = tag57.find('D');
	if (index == string::npos)
	{//未找到，直接返回空
		return _T("");
	}
	return tag57.substr(0, index);
}

/******************************************************************
* 联机处理允许发卡行使用发卡行主机系统中的风险管理参数对交易进行检查，作
* 出批准或拒绝交易的决定。
* 发卡行的响应包括卡片的二次发卡更新和一个发卡行生成的密文。卡片验证密文
* 确保响应来自一个有效的发卡行。此验证叫发卡行认证。
*******************************************************************/
bool PBOC::OnlineBussiness()
{
	Log->Info("======================== 联机处理 开始 =================================");
	string CID = GetTagValue(Tag9F27);		// 	密文信息数据
	string ATC = GetTagValue(Tag9F36);
	string AC = GetTagValue(Tag9F26);		//应用密文（AC）
	string issuerApplicationData = GetTagValue(Tag9F10);	//发卡行应用数据
	
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
        Log->Error("联机处理 生成ARPC失败");
        return false;
    }
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

void PBOC::ParseTransLog(const string buffer)
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

/*************************************************************
* 功能： 处理发卡行脚本
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
        Log->Error(_T("MAC 不能为空"));
        return false;
    }

    //执行电子现金相关脚本
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
            Log->Error(_T("执行脚本失败"));
        }
    }
    
	return true;
}

void PBOC::ShowLog()
{
    //获取电子现金圈存日志入口
    string logCashEntry = GetTagValue(TagDF4D);
    string logEntry = GetTagValue(Tag9F4D);

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
}

//交易结束
bool PBOC::EndTransaction()
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
    //发送GAC2时，需要重置终端处理结果。
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

	//解析卡片响应数据
	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;
    m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//格式化输出响应数据
	PrintTags(entites, entitiesCount);
	//SaveTag(entites, entitiesCount);  //不保存该响应值，会覆盖第一次GAC响应，而脚本处理需要第一次应用密文。  
	ShowCardTransType(string((char*)entites[0].Value));

	return true;
}
