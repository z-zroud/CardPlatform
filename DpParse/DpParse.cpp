#include "stdafx.h"
#include "IDpParse.h"
#include "Des0.h"
#include <fstream>
#include "../Util/IniConfig.h"


//**********************************************************
//* ���ܣ� ���Ŀ¼�Ƿ����
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

//ɾ���ַ����еĿո�
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
    if (m_vecItems.size() > 0)
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

void Dict::ReplaceKey(string oldKey, string newKey)
{
    for (auto& item : m_vecItems) {
        if (item.first == oldKey) {
            item.first = newKey;
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
* ����CPS����
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
* ��ȡ�ļ���С
*******************************************************************/
long IDpParse::GetFileSize(ifstream& dpFile)
{
	dpFile.seekg(0, ios::end);
	long dpFileSize = static_cast<long>(dpFile.tellg());
	dpFile.seekg(0, ios::beg);  //�����ļ�ָ������������ĩβλ��

	return dpFileSize;
}

/******************************************************************
* ���ļ�
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
* ��ȡDP�ļ��У��ӵ�ǰ��ָ��λ�õ�bufferLen�������ݣ�������ָ��λ��
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
* �ж�ĳ�������Ƿ�ΪTLV�ṹ
*****************************************************************/
bool IDpParse::IsTlvStruct(unsigned char* buffer, unsigned int bufferLength)
{
	unsigned int currentIndex = 0;							//���ڱ��buffer
															//�ж���Tag�Ƿ�Ϊ���ֽ�,�ֽڵ�1--5λ�Ƿ�Ϊ1���ǵĻ��к����ֽ�
	while (currentIndex < bufferLength)
	{
		if ((buffer[currentIndex] & 0x1f) == 0x1f)  //tagΪ���ֽ�
		{
			int endTagIndex = currentIndex;
			while ((buffer[++endTagIndex] & 0x80) == 0x80); //���һ���ֽڵ����λΪ0
			int tagSize = endTagIndex - currentIndex + 1; //�����ǩ��ռ���ֽ�
			currentIndex += tagSize;
		}
		else       //tagΪ���ֽ�
		{
			currentIndex += 1;
		}

		int dataLen = 0;
		unsigned int lenSize = 0;
		//����Length�򳤶�
		if ((buffer[currentIndex] & 0x80) == 0x80)
		{
			//���λΪ1
			lenSize = buffer[currentIndex] & 0x7f;
			for (unsigned int index = 0; index < lenSize; index++)
			{
				dataLen += buffer[currentIndex + 1 + index] << ((lenSize - 1 - index) * 8);
			}
			lenSize += 1;
		}
		else  //���λΪ0
		{
			dataLen = buffer[currentIndex];
			lenSize = 1;
		}
		currentIndex += lenSize + dataLen;
	}

	return currentIndex == bufferLength;
}

/**********************************************************************
* ����TLV���ݽṹ
***********************************************************************/
void IDpParse::ParseTLV(unsigned char *buffer,			//TLV�ַ���	
	unsigned int bufferLength,		//TLV�ַ�������
	PTLVEntity PTlvEntity,					//TLVָ��
	unsigned int& entitySize			//TLV�ṹ����������ʱ�õ�
	)
{
	unsigned int currentIndex = 0;							//���ڱ��buffer
	int currentTLVIndex = 0;						//��ǰTLV�ṹ���
	int currentStatus = 'T';						//״̬�ַ�

	unsigned long valueSize = 0;					//���ݳ���

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //����
						   //�ж�TLV�Ƿ�Ϊ��һ�ṹ,�ֽڵĵ�6λ�Ƿ�Ϊ1
			if ((buffer[currentIndex] & 0x20) != 0x20)
			{
				//��һ�ṹ
				PTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				PTlvEntity[currentTLVIndex].subTLVnum = 0;		//��TLV������Ϊ��
																//�ж���Tag�Ƿ�Ϊ���ֽ�,�ֽڵ�1--5λ�Ƿ�Ϊ1���ǵĻ��к����ֽ�
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					//TagΪ���ֽ�
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //���һ���ֽڵ����λΪ0
					int tagSize = endTagIndex - currentIndex + 1; //�����ǩ��ռ���ֽ�

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;//�ַ���ĩβ��0

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tagռ��1���ֽ�
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}
			}
			else
			{
				//���Ͻṹ
				//�ж��Ƿ�Ϊ���ֽ�
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

				//������TLV�е�Tag
				int subTlvLength = 0;			//��TLV����
				unsigned char * temp;			//��TLV������������
												//���ж�length��ĳ���,length���ֽ�������λΪ1�������ֽڴ������ȣ�Ϊ0��1--7λ�������ݳ���
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					//���λΪ1
					unsigned int lengthSize = buffer[currentIndex] & 0x7f;
					for (unsigned int index = 0; index < lengthSize; index++)
					{
						//�����ʾ����
						subTlvLength += buffer[currentIndex + 1 + index] << ((lengthSize - 1 - index) * 8);

						/* �����С�˵Ļ�
						subTlvLength += buffer[currentIndex + 1 + index] << (index * 8);
						*/
					}


					//����һ��subTlvlength��С���ڴ��Ÿ�TLV������
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + currentIndex + 1 + lengthSize, subTlvLength);

				}
				else
				{
					//���λΪ0
					subTlvLength = buffer[currentIndex];
					temp = (unsigned char *)malloc(subTlvLength);

					memcpy(temp, buffer + currentIndex + 1, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//����ж��ٸ�ͬ�ȼ�����TLV������ʱҲӦ���õ�
								   //�������TLVͬ�ȼ���TLV�ж��ٸ�������100TLV��С���ڴ�϶�����
				PTlvEntity[currentTLVIndex].subTLVEntity = (PTLVEntity)malloc(sizeof(PTLVEntity[100]));
				ParseTLV(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				PTlvEntity[currentTLVIndex].subTLVnum = oSize; //������TLV������

			}
			currentStatus = 'L';
			break;
		case 'L':
			//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
			if ((buffer[currentIndex] & 0x80) == 0x80)
			{
				//���λ1
				unsigned int lengthSize = buffer[currentIndex] & 0x7f;
				currentIndex += 1; //����һ���ֽڿ�ʼ��Length��
				for (unsigned int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << ((lengthSize - 1 - index) * 8); //����Length��ĳ���
				}
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//���λ0
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

			//������һ��TLV����ѭ��
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
���ڽ���ruleFile�ļ�
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
    //����˳��DGIӳ��-->DGI����-->DGIɾ��-->DGI����-->DGIɾ��tag
    HandleDGIMap(cpsItem);
    HandleDGIExchange(cpsItem);
    HandleDGIDelete(cpsItem);
    HandleDGIEncrypt(cpsItem);
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
                dgiItem.value.ReplaceKey(item.first, item.second);
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
        cpsItem.items[firstIndex].value.ReplaceKey(item.first, item.second);
        cpsItem.items[secondIndex].dgi = item.first;
        cpsItem.items[secondIndex].value.ReplaceKey(item.second, item.first);
	}
}

/***************************************************************
* ����DGI���ݣ�ע���DGIֻӦֻ��һ��tag=encryptData, ����tag==DGI
****************************************************************/
void IRule::HandleDGIEncrypt(CPS_ITEM& cpsItem)
{
   
    for (auto& item : cpsItem.items)
    {
        string decryptedData;
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

