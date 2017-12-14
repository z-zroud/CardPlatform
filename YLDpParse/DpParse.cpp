#include "stdafx.h"
#include "IDpParse.h"
#include <fstream>



///**********************************************************
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
	for (auto iter : cpsItem.items)
	{
		outputFile << "[" << iter.dgi << "]" << endl;
		for (auto tlvData : iter.value.GetItems())
		{
			outputFile << tlvData.first << "=" << tlvData.second << endl;
		}
	}
	outputFile.close();
	outputFile.clear();
}


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

void IDpParse::TLVConstruct(unsigned char *buffer,			//TLV�ַ���	
	unsigned int bufferLength,		//TLV�ַ�������
	PTLV PTlvEntity,					//TLVָ��
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

					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].Tag[tagSize] = 0;//�ַ���ĩβ��0

					PTlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tagռ��1���ֽ�
					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].Tag[1] = 0;

					PTlvEntity[currentTLVIndex].TagSize = 1;

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

					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].Tag[tagSize] = 0;

					PTlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].Tag[1] = 0;

					PTlvEntity[currentTLVIndex].TagSize = 1;

					currentIndex += 1;
				}

				//������TLV�е�Tag
				int subTlvLength = 0;			//��TLV����
				unsigned char * temp;			//��TLV������������
				//���ж�length��ĳ���,length���ֽ�������λΪ1�������ֽڴ����ȣ�Ϊ0��1--7λ�������ݳ���
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
				PTlvEntity[currentTLVIndex].subTLVEntity = (PTLV)malloc(sizeof(TLV[100]));
				TLVConstruct(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

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
				PTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].Length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].LengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//���λ0
				PTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(1);
				memcpy(PTlvEntity[currentTLVIndex].Length, buffer + currentIndex, 1);
				PTlvEntity[currentTLVIndex].Length[1] = 0;
				PTlvEntity[currentTLVIndex].LengthSize = 1;

				valueSize = PTlvEntity[currentTLVIndex].Length[0];

				currentIndex += 1;
			}
			currentStatus = 'V';
			break;
		case 'V':
			PTlvEntity[currentTLVIndex].Value = (unsigned char *)malloc(valueSize);
			memset(PTlvEntity[currentTLVIndex].Value, 0, valueSize);
			memcpy(PTlvEntity[currentTLVIndex].Value, buffer + currentIndex, valueSize);
			PTlvEntity[currentTLVIndex].Value[valueSize] = 0;

			currentIndex += valueSize;

			//������һ��TLV����ѭ��
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}

bool IDpParse::TLVParseAndFindError(
	PTLV PTlvEntity,					//�����TLV�ṹ��
	unsigned int entitySize,			//TLV�ṹ�������
	unsigned char* buffer,				//������ַ���
	unsigned int& bufferLength			//�ַ����ĳ���
	)
{
	int currentIndex = 0;
	unsigned int currentTLVIndex = 0;
	unsigned long valueSize = 0;

	while (currentTLVIndex < entitySize)
	{
		valueSize = 0;
		TLVEntity entity = PTlvEntity[currentTLVIndex];

		memcpy(buffer + currentIndex, entity.Tag, entity.TagSize);  //����Tag
		currentIndex += entity.TagSize;

		for (unsigned int index = 0; index < entity.LengthSize; index++)
		{
			//�����ʾ����
			valueSize += entity.Length[index] << ((entity.LengthSize - 1 - index) * 8); //����Length��ĳ���
		}
		if (valueSize > 127)									//��ԭlength  �����λΪ1�����
		{
			buffer[currentIndex] = 0x80 | entity.LengthSize;
			currentIndex += 1;/******************************************/
		}

		memcpy(buffer + currentIndex, entity.Length, entity.LengthSize);    //����Length
		currentIndex += entity.LengthSize;
		//�ж��Ƿ������Ƕ��TLV
		if (entity.subTLVEntity == NULL)
		{
			//������
			memcpy(buffer + currentIndex, entity.Value, valueSize); //����Value
			currentIndex += valueSize;
		}
		else
		{
			unsigned int oLength;
			TLVParseAndFindError(entity.subTLVEntity, entity.subTLVnum, buffer + currentIndex, oLength); //������Ƕ��TLV
			currentIndex += oLength;
		}

		currentTLVIndex++;
	}
	buffer[currentIndex] = 0;
	bufferLength = currentIndex;

	return true;
}


void IDpParse::HandleRule(IRule ruleObj, string ruleConfig, CPS_ITEM& cpsItem)
{
	ruleObj.ParseRule(ruleConfig);
	ruleObj.HandleRule(cpsItem);
}

/////////////////////////////////////////////////////////////////////////////////////
void IRule::ParseRule(const string& ruleConfig)
{

}
void IRule::HandleRule(CPS_ITEM& cpsItem)
{

}

void IRule::HandleDGIMap()
{
	for (auto item : m_vecDGIMap)
	{
		for (auto& dgiItem : m_cpsItem.items)
		{
			if (item.first == dgiItem.dgi)
			{
				dgiItem.dgi = item.second;
				break;
			}
		}
	}
}
void IRule::HandleDGIExchange()
{
	for (auto item : m_vecDGIExchange)
	{
		int firstIndex = 0;
		int secondIndex = 0;
		while (item.first != m_cpsItem.items[firstIndex].dgi)
			firstIndex++;
		while (item.second != m_cpsItem.items[secondIndex].dgi)
			secondIndex++;
		m_cpsItem.items[firstIndex].dgi = item.second;
		m_cpsItem.items[secondIndex].dgi = item.first;
	}
}

void IRule::HandleDGIDecrypt()
{

}

void IRule::HandleDGIDelete()
{
	for (auto item : m_vecDGIDelete)
	{
		for (auto iter = m_cpsItem.items.begin(); iter != m_cpsItem.items.end(); iter++)
		{
			if (item == iter->dgi)
			{
				iter = m_cpsItem.items.erase(iter);
			}
			iter++;
		}
	}
}
void IRule::HandleTagDelete()
{
	//for (auto item : m_vecTagDelete)
	//{
	//	for (auto &dgiItem : m_cpsItem.items)
	//	{
	//		if (item.first == dgiItem.dgi)
	//		{
	//			for(auto& value : dgiItem.value.)
	//		}
	//	}
	//}
}


