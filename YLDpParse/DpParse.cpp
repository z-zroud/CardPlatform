#include "stdafx.h"
#include "IDpParse.h"
#include <fstream>



///**********************************************************
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

void IDpParse::TLVConstruct(unsigned char *buffer,			//TLV字符串	
	unsigned int bufferLength,		//TLV字符串长度
	PTLV PTlvEntity,					//TLV指针
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

					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].Tag[tagSize] = 0;//字符串末尾置0

					PTlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tag占用1个字节
					PTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].Tag[1] = 0;

					PTlvEntity[currentTLVIndex].TagSize = 1;

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
				PTlvEntity[currentTLVIndex].subTLVEntity = (PTLV)malloc(sizeof(TLV[100]));
				TLVConstruct(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

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
				PTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].Length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].LengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//最高位0
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

			//进入下一个TLV构造循环
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}

bool IDpParse::TLVParseAndFindError(
	PTLV PTlvEntity,					//输入的TLV结构体
	unsigned int entitySize,			//TLV结构体的数量
	unsigned char* buffer,				//输出的字符串
	unsigned int& bufferLength			//字符串的长度
	)
{
	int currentIndex = 0;
	unsigned int currentTLVIndex = 0;
	unsigned long valueSize = 0;

	while (currentTLVIndex < entitySize)
	{
		valueSize = 0;
		TLVEntity entity = PTlvEntity[currentTLVIndex];

		memcpy(buffer + currentIndex, entity.Tag, entity.TagSize);  //解析Tag
		currentIndex += entity.TagSize;

		for (unsigned int index = 0; index < entity.LengthSize; index++)
		{
			//大端显示数据
			valueSize += entity.Length[index] << ((entity.LengthSize - 1 - index) * 8); //计算Length域的长度
		}
		if (valueSize > 127)									//还原length  当最高位为1的情况
		{
			buffer[currentIndex] = 0x80 | entity.LengthSize;
			currentIndex += 1;/******************************************/
		}

		memcpy(buffer + currentIndex, entity.Length, entity.LengthSize);    //解析Length
		currentIndex += entity.LengthSize;
		//判断是否包含子嵌套TLV
		if (entity.subTLVEntity == NULL)
		{
			//不包含
			memcpy(buffer + currentIndex, entity.Value, valueSize); //解析Value
			currentIndex += valueSize;
		}
		else
		{
			unsigned int oLength;
			TLVParseAndFindError(entity.subTLVEntity, entity.subTLVnum, buffer + currentIndex, oLength); //解析子嵌套TLV
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


