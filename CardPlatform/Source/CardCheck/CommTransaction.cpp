#include "StdAfx.h"
#include "CommTransaction.h"
#include "Util\Log.h"
#include "Util\Converter.h"

CommTransaction::CommTransaction(IPCSC* reader)
{
	m_pReader	= reader;
	m_pAPDU		= reader->GetAPDU();
	m_pParaser	= new TLVParaser();
	m_IsOnlineAuthSucessed = false;
}

//主要实例化那些不需要使用APDU指令的对象，例如脱机数据认证
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

//格式化输出响应数据
void CommTransaction::FormatOutputResponse(PBCD_TLV entities, int num)
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
string CommTransaction::FindTagValue(const string tag, PBCD_TLV entities, int num)
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
string CommTransaction::FindTagValue(const string tag)
{
	for (auto s : m_Tags)
	{
		if (tag == s.first)
			return s.second;
	}

	return "";
}

//保存tag值
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

void CommTransaction::SaveTag(string tag, string value)
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

void CommTransaction::ParseAFL(string strAFL, vector<AFL> &output)
{
	if (strAFL.length() % 8 != 0)
	{
		return;		//每个AFL占用4个字节，也就是8个BCD码
	}
	//int aflCount = AFL.length() / 8;
	for (unsigned int i = 0; i < strAFL.length(); i += 8)
	{
		AFL afl;

		afl.SFI = ((Tool::Converter::HexToInt(strAFL[i]) & 0x0F) << 1) + (Tool::Converter::HexToInt(strAFL[i + 1]) && 0x08);
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
string CommTransaction::SelectPSE(APP_TYPE appType)
{
	APDU_RESPONSE response;

	memset(&response, 0, sizeof(response));
	if (appType == APP_PSE)  //执行PSE应用
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
		m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entities, entitiesCount);

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
				m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), dirEntites, dirEntitiesCount);

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
		Log->Warning("返回值[%s]: PSE锁定", GetResult(response.SW1, response.SW2));
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

//AID 选择
bool CommTransaction::SelectAID(const string &strAID)
{
	APDU_RESPONSE response;

	memset(&response, 0, sizeof(response));
	if (!m_pAPDU->SelectApplicationCommand(strAID, response))
		return false;

	BCD_TLV entites[32] = { 0 };
	unsigned int entitiesCount = 0;

	m_pParaser->TLVConstruct((unsigned char*)response.data, strlen(response.data), entites, entitiesCount);

	//格式化输出响应数据
	FormatOutputResponse(entites, entitiesCount);
	SaveTag(entites, entitiesCount);

	return true;
}

bool CommTransaction::SelectApplication()
{
	return false;
}
 bool CommTransaction::InitilizeApplication()
 {
	 return false;
 }
 bool CommTransaction::ReadApplicationData()
 {
	 return false;
 }
 bool CommTransaction::OfflineDataAuth()
 {
	 return false;
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
