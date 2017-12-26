#include "stdafx.h"
#include "IDpParse.h"
#include "Des0.h"
#include <fstream>
#include "../Util/IniConfig.h"


//**********************************************************
//* 功能： 检查目录是否存在
//***********************************************************/
bool  CheckFolderExist(const string &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;
}

string StrToHex(const char* strBin, int len, bool bIsUpper)
{
	char *pCopy = new char[len + 1];
	memset(pCopy, 0, len + 1);
	memcpy(pCopy, strBin, len);
	string strHex;
	//strHex.resize(strBin.size() * 2);
	strHex.resize(len * 2);
	for (int i = 0; i < len; i++)
	{
		uint8_t cTemp = pCopy[i];
		for (size_t j = 0; j < 2; j++)
		{
			uint8_t cCur = (cTemp & 0x0f);
			if (cCur < 10)
			{
				cCur += '0';
			}
			else
			{
				cCur += ((bIsUpper ? 'A' : 'a') - 10);
			}
			strHex[2 * i + 1 - j] = cCur;
			cTemp >>= 4;
		}
	}

	return strHex;
}

//删除字符串中的空格
string DeleteSpace(string s)
{
	string strResult;
	int len = s.length();
	for (auto c : s)
	{
		if (c != ' ')
		{
			strResult.push_back(c);
		}
	}
	return strResult;
	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}
	return strResult;
}

bool ExistedInList(string value, vector<string> collection)
{
    for (auto item : collection)
    {
        if (item == value) {
            return true;
        }
    }
    return false;
}

string Dict::GetItem()
{
    if (m_vecItems.size() > 1)
        return m_vecItems[0].second;
    
    return "";
}

void Dict::ReplaceItem(string key, string value)
{
    for (auto& item : m_vecItems) {
        if (item.first == key) {
            item.second = value;
            break;
        }
    }
}

void Dict::DeleteItem(string key)
{
    for (auto iter = m_vecItems.begin(); iter != m_vecItems.end(); iter++) {
        if (iter->first == key) {
            m_vecItems.erase(iter);
            break;
        }
    }
}

/*****************************************************
* 保存CPS数据
******************************************************/
void IDpParse::Save(CPS_ITEM cpsItem)
{
	int pos = cpsItem.fileName.find_last_of('\\');
	string folder = cpsItem.fileName.substr(0, pos);
	if (!CheckFolderExist(folder))
	{
		CreateDirectory(folder.c_str(), NULL);
	}
	ofstream outputFile(cpsItem.fileName.c_str());
	if (!outputFile)
		return;
    string privousDGI = "";
	for (auto iter : cpsItem.items)
	{
        if (iter.dgi != privousDGI) {
            outputFile << "[" << iter.dgi << "]" << endl;
        }
		for (auto tlvData : iter.value.GetItems())
		{
			outputFile << tlvData.first << "=" << tlvData.second << endl;
		}
        privousDGI = iter.dgi;
	}
	outputFile.close();
	outputFile.clear();
}

string IDpParse::DecryptDGI(string tk, string encryptData, bool padding80)
{
	string strResult;

	if (padding80)
	{
		if (encryptData.length() % 16 == 0)
		{
			encryptData += "8000000000000000";
		}
		else {
			encryptData += "80";
			int remaindZero = encryptData.length() % 16;
			encryptData.append(remaindZero, '0');
		}
	}

	int len = encryptData.length();
	int unit = 16;
	for (int i = 0; i < len; i += unit)
	{
		char* pOutput = new char[unit + 1];
		memset(pOutput, 0, unit + 1);
		_Des3(pOutput, (char*)tk.c_str(), (char*)encryptData.substr(i, unit).c_str());
		strResult += string(pOutput);
	}

	return strResult;
}

/******************************************************************
* 获取文件大小
*******************************************************************/
long IDpParse::GetFileSize(ifstream& dpFile)
{
	dpFile.seekg(0, ios::end);
	long dpFileSize = static_cast<long>(dpFile.tellg());
	dpFile.seekg(0, ios::beg);  //重置文件指针至保留数据末尾位置

	return dpFileSize;
}

/******************************************************************
* 打开文件
*******************************************************************/
bool IDpParse::OpenDpFile(const char* fileName, ifstream& dpFile)
{
	int pos = string(fileName).find_last_of('\\');
	string path = string(fileName).substr(0, pos + 1);
	dpFile.open(fileName, ios::in | ios::binary);
	if (!dpFile.is_open())
		return false;

	return true;
}


/******************************************************************
* 获取DP文件中，从当前流指针位置到bufferLen长的数据，返回流指针位置
*******************************************************************/
streampos IDpParse::GetBuffer(ifstream &dpFile, char* buffer, int bufferLen)
{
	dpFile.read(buffer, bufferLen);

	return dpFile.tellg();
}

streampos IDpParse::GetBCDBuffer(ifstream &dpFile, string& buffer, int len)
{
	char * readBuffer = new char[len + 1];
	memset(readBuffer, 0, len + 1);

	dpFile.read(readBuffer, len);
	buffer = StrToHex(readBuffer, len);
	delete[] readBuffer;

	return dpFile.tellg();
}

streampos IDpParse::GetBCDBufferLittleEnd(ifstream &dpFile, string &buffer, int len)
{
    string data;
    streampos pos = GetBCDBuffer(dpFile, data, len);
    if (data.length() % 2 != 0) {
        return pos;
    }
    int i = data.length() - 2;
    while(i >= 0) {
        buffer += data.substr(i, 2);
        i -= 2;
    }

    return pos;
}

streampos IDpParse::GetLenTypeBuffer(ifstream &dpFile, int &dataLen, int len)
{
	string buffer;
	GetBCDBuffer(dpFile, buffer, len);
	dataLen = stoi(buffer, 0, 16);

	return dpFile.tellg();
}

streampos IDpParse::GetLenTypeBufferLittleEnd(ifstream &dpFile, int &dataLen, int len)
{
    string buffer;
    GetBCDBufferLittleEnd(dpFile, buffer, len);
    dataLen = stoi(buffer, 0, 16);

    return dpFile.tellg();
}
/****************************************************************
* 判断某段数据是否为TLV结构
*****************************************************************/
bool IDpParse::IsTlvStruct(unsigned char* buffer, unsigned int bufferLength)
{
	unsigned int currentIndex = 0;							//用于标记buffer
															//判断是Tag是否为多字节,字节的1--5位是否都为1，是的话有后续字节
	while (currentIndex < bufferLength)
	{
		if ((buffer[currentIndex] & 0x1f) == 0x1f)  //tag为多字节
		{
			int endTagIndex = currentIndex;
			while ((buffer[++endTagIndex] & 0x80) == 0x80); //最后一个字节的最高位为0
			int tagSize = endTagIndex - currentIndex + 1; //计算标签所占用字节
			currentIndex += tagSize;
		}
		else       //tag为单字节
		{
			currentIndex += 1;
		}

		int dataLen = 0;
		unsigned int lenSize = 0;
		//计算Length域长度
		if ((buffer[currentIndex] & 0x80) == 0x80)
		{
			//最高位为1
			lenSize = buffer[currentIndex] & 0x7f;
			for (unsigned int index = 0; index < lenSize; index++)
			{
				dataLen += buffer[currentIndex + 1 + index] << ((lenSize - 1 - index) * 8);
			}
			lenSize += 1;
		}
		else  //最高位为0
		{
			dataLen = buffer[currentIndex];
			lenSize = 1;
		}
		currentIndex += lenSize + dataLen;
	}

	return currentIndex == bufferLength;
}

/**********************************************************************
* 解析TLV数据结构
***********************************************************************/
void IDpParse::ParseTLV(unsigned char *buffer,			//TLV字符串	
	unsigned int bufferLength,		//TLV字符串长度
	PTLVEntity PTlvEntity,					//TLV指针
	unsigned int& entitySize			//TLV结构数量，解析时用到
	)
{
	unsigned int currentIndex = 0;							//用于标记buffer
	int currentTLVIndex = 0;						//当前TLV结构标记
	int currentStatus = 'T';						//状态字符

	unsigned long valueSize = 0;					//数据长度

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //清零
						   //判断TLV是否为单一结构,字节的第6位是否为1
			if ((buffer[currentIndex] & 0x20) != 0x20)
			{
				//单一结构
				PTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				PTlvEntity[currentTLVIndex].subTLVnum = 0;		//子TLV的数量为零
																//判断是Tag是否为多字节,字节的1--5位是否都为1，是的话有后续字节
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					//Tag为多字节
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //最后一个字节的最高位为0
					int tagSize = endTagIndex - currentIndex + 1; //计算标签所占用字节

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;//字符串末尾置0

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tag占用1个字节
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}
			}
			else
			{
				//复合结构
				//判断是否为多字节
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80);
					int tagSize = endTagIndex - currentIndex + 1;

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}

				//分析子TLV中的Tag
				int subTlvLength = 0;			//子TLV长度
				unsigned char * temp;			//子TLV所包含的数据
												//先判断length域的长度,length域字节如果最高位为1，后续字节代表长度，为0，1--7位代表数据长度
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					//最高位为1
					unsigned int lengthSize = buffer[currentIndex] & 0x7f;
					for (unsigned int index = 0; index < lengthSize; index++)
					{
						//大端显示数据
						subTlvLength += buffer[currentIndex + 1 + index] << ((lengthSize - 1 - index) * 8);

						/* 如果是小端的话
						subTlvLength += buffer[currentIndex + 1 + index] << (index * 8);
						*/
					}


					//申请一段subTlvlength大小的内存存放该TLV的内容
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + currentIndex + 1 + lengthSize, subTlvLength);

				}
				else
				{
					//最高位为0
					subTlvLength = buffer[currentIndex];
					temp = (unsigned char *)malloc(subTlvLength);

					memcpy(temp, buffer + currentIndex + 1, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//输出有多少个同等级的子TLV，解析时也应该用到
								   //不清楚子TLV同等级的TLV有多少个，申请100TLV大小的内存肯定够用
				PTlvEntity[currentTLVIndex].subTLVEntity = (PTLVEntity)malloc(sizeof(PTLVEntity[100]));
				ParseTLV(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				PTlvEntity[currentTLVIndex].subTLVnum = oSize; //填入子TLV的数量

			}
			currentStatus = 'L';
			break;
		case 'L':
			//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
			if ((buffer[currentIndex] & 0x80) == 0x80)
			{
				//最高位1
				unsigned int lengthSize = buffer[currentIndex] & 0x7f;
				currentIndex += 1; //从下一个字节开始算Length域
				for (unsigned int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << ((lengthSize - 1 - index) * 8); //计算Length域的长度
				}
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//最高位0
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(1);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, 1);
				PTlvEntity[currentTLVIndex].length[1] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = 1;

				valueSize = PTlvEntity[currentTLVIndex].length[0];

				currentIndex += 1;
			}
			currentStatus = 'V';
			break;
		case 'V':
			PTlvEntity[currentTLVIndex].value = (unsigned char *)malloc(valueSize);
			memset(PTlvEntity[currentTLVIndex].value, 0, valueSize);
			memcpy(PTlvEntity[currentTLVIndex].value, buffer + currentIndex, valueSize);
			PTlvEntity[currentTLVIndex].value[valueSize] = 0;

			currentIndex += valueSize;

			//进入下一个TLV构造循环
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}

void IDpParse::HandleRule(string ruleConfig, CPS_ITEM& cpsItem)
{
    IRule ruleObj;
	ruleObj.SetRule(ruleConfig);
	ruleObj.HandleRule(cpsItem);
}

/////////////////////////////////////////////////////////////////////////////////////
/**********************************************************
用于解析ruleFile文件
***********************************************************/
void IRule::SetRule(const string& ruleFile)
{
    IniConfig ruleCfg;
    if (ruleCfg.Read(ruleFile)) 
    {
        m_tk = ruleCfg.GetValue("TK", "key");

        vector<pair<string, string>> deleteDGIs;
        ruleCfg.GetValue("DeleteDGI", deleteDGIs);
        for (auto item : deleteDGIs) {
            m_vecDGIDelete.push_back(item.second);
        }

        vector<pair<string, string>> encryptDGIs;
        ruleCfg.GetValue("EncryptDGI", encryptDGIs);
        for (auto item : encryptDGIs) {
            m_vecDGIEncrypt.push_back(item.second);
        }

        vector<pair<string, string>> padding80DGIs;
        ruleCfg.GetValue("EncryptDGIPadding80", padding80DGIs);
        for (auto item : padding80DGIs) {
            m_vecDGIpadding80.push_back(item.second);
        }

        ruleCfg.GetValue("ExchangeDGIData", m_vecDGIExchange);
        ruleCfg.GetValue("DGIMap", m_vecDGIMap);
        ruleCfg.GetValue("DeleteTag", m_vecTagDelete);
    }
}

void IRule::HandleRule(CPS_ITEM& cpsItem)
{
    //处理顺序：DGI映射-->DGI交换-->DGI删除-->DGI解密-->DGI删除tag
    HandleDGIMap(cpsItem);
    HandleDGIExchange(cpsItem);
    HandleDGIDelete(cpsItem);
    //HandleDGIEncrypt(cpsItem);
    HandleTagDelete(cpsItem);
}

void IRule::HandleDGIMap(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIMap)
	{
		for (auto& dgiItem : cpsItem.items)
		{
			if (item.first == dgiItem.dgi)
			{
				dgiItem.dgi = item.second;
				break;
			}
		}
	}
}
void IRule::HandleDGIExchange(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIExchange)
	{
		int firstIndex = 0;
		int secondIndex = 0;
		while (item.first != cpsItem.items[firstIndex].dgi)
			firstIndex++;
		while (item.second != cpsItem.items[secondIndex].dgi)
			secondIndex++;
        cpsItem.items[firstIndex].dgi = item.second;
        cpsItem.items[secondIndex].dgi = item.first;
	}
}

/***************************************************************
* 解密DGI数据，注意该DGI只应只有一个tag=encryptData, 并且tag==DGI
****************************************************************/
void IRule::HandleDGIEncrypt(CPS_ITEM& cpsItem)
{
    string decryptedData;
    for (auto& item : cpsItem.items)
    {
        if (ExistedInList(item.dgi, m_vecDGIEncrypt))
        {
            string encryptData = item.value.GetItem();
            if (ExistedInList(item.dgi,m_vecDGIpadding80))
            {
                if (encryptData.length() % 16 == 0)
                {
                    encryptData += "8000000000000000";
                }
                else {
                    encryptData += "80";
                    int remaindZero = encryptData.length() % 16;
                    encryptData.append(remaindZero, '0');
                }
            }

            int len = encryptData.length();
            int unit = 16;
            for (int i = 0; i < len; i += unit)
            {
                char* pOutput = new char[unit + 1];
                memset(pOutput, 0, unit + 1);
                _Des3(pOutput, (char*)m_tk.c_str(), (char*)encryptData.substr(i, unit).c_str());
                decryptedData += string(pOutput);
            }
            item.value.ReplaceItem(item.dgi, decryptedData);
        }
    }
}

void IRule::HandleDGIDelete(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecDGIDelete)
	{
		for (auto iter = cpsItem.items.begin(); iter != cpsItem.items.end(); iter++)
		{
			if (item == iter->dgi)
			{
				iter = cpsItem.items.erase(iter);
                break;
			}
		}
	}
}

void IRule::HandleTagDelete(CPS_ITEM& cpsItem)
{
	for (auto item : m_vecTagDelete)
	{
		for (auto &dgiItem : cpsItem.items)
		{
			if (item.first == dgiItem.dgi)
			{
                dgiItem.value.DeleteItem(item.second);
                break;
			}
		}
	}
}


