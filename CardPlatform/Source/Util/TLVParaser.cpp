#include "stdafx.h"
#include "TLVParaser.h"
#include <ctype.h>
#include <string>

using namespace std;


void TLVParaser::TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PBCD_TL pTlEntity, unsigned int& entityNum)
{
	unsigned int currentIndex = 0;		//用于标记buffer
	int currentTLVIndex = 0;			//当前TL结构标记
	int currentStatus = 'T';			//状态字符

	unsigned long valueSize = 0;		//数据长度

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			{
				valueSize = 0; //清零
				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//多字节
					int endTagIndex = tempIndex + 1;
					while (ctoi(buffer[endTagIndex]) & 0x08)
					{
						endTagIndex += 2;
					}
					int tagSize = endTagIndex - currentIndex + 2;

					pTlEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(pTlEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					pTlEntity[currentTLVIndex].tag[tagSize] = 0;

					currentIndex += tagSize;
				}
				else
				{
					//单字节
					pTlEntity[currentTLVIndex].tag = (unsigned char *)malloc(2);
					memcpy(pTlEntity[currentTLVIndex].tag, buffer + currentIndex, 2);
					pTlEntity[currentTLVIndex].tag[2] = 0;

					currentIndex += 2;
				}

				currentStatus = 'L';
				break;
			}
			
		case 'L':
			{
				//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
				if (ctoi(buffer[currentIndex]) & 0x08)
				{
					//最高位1
					unsigned int lengthSize = 2 * ((buffer[currentIndex] & 0x07) * 8 + (buffer[++currentIndex] & 0x0f));

					currentIndex += 1; //从下一个字节开始算Length域
					pTlEntity[currentTLVIndex].len = (unsigned char *)malloc(lengthSize);
					memcpy(pTlEntity[currentTLVIndex].len, buffer + currentIndex, lengthSize);
					pTlEntity[currentTLVIndex].len[lengthSize] = 0;

					valueSize = 2 * stoi((char*)pTlEntity[currentTLVIndex].len, 0, 16);

					currentIndex += lengthSize;

				}
				else
				{
					//最高位0
					pTlEntity[currentTLVIndex].len = (unsigned char *)malloc(2);
					memcpy(pTlEntity[currentTLVIndex].len, buffer + currentIndex, 2);
					pTlEntity[currentTLVIndex].len[2] = 0;

					valueSize = 2 * stoi((char*)pTlEntity[currentTLVIndex].len, 0, 16);

					currentIndex += 2;
				}
				currentTLVIndex++;
				currentStatus = 'T';
				break;
			}
		}			
	}
	entityNum = currentTLVIndex;
}

void TLVParaser::TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PBCD_TLV pBCDTlvEntity, unsigned int& entitySize)
{
	unsigned int currentIndex = 0;		//用于标记buffer
	int currentTLVIndex = 0;			//当前TLV结构标记
	int currentStatus = 'T';			//状态字符

	unsigned long valueSize = 0;		//数据长度

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //清零

			//判断TLV是否为单一结构,字节的第6位是否为1
			if (ctoi(buffer[currentIndex]) & 0x02)
			{
				//复合结构
				pBCDTlvEntity[currentTLVIndex].isTemplate = true;
				//判断是否为多字节
				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//多字节
					int endTagIndex = tempIndex + 1;
					while (ctoi(buffer[endTagIndex]) & 0x08)
					{
						endTagIndex += 2;
					}
					int tagSize = endTagIndex - currentIndex + 2;

					pBCDTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(tagSize);
					memcpy(pBCDTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					pBCDTlvEntity[currentTLVIndex].Tag[tagSize] = 0;

					pBCDTlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//单字节
					pBCDTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(2);
					memcpy(pBCDTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 2);
					pBCDTlvEntity[currentTLVIndex].Tag[2] = 0;

					pBCDTlvEntity[currentTLVIndex].TagSize = 2;

					currentIndex += 2;
				}

				//分析子TLV中的Tag
				int subTlvLength = 0;			//子TLV长度
				unsigned char * temp;			//子TLV所包含的数据
				//先判断length域的长度,length域字节如果最高位为1，后续字节代表长度，为0，1--7位代表数据长度
				unsigned int nStartIndex = currentIndex;
				if (ctoi(buffer[nStartIndex]) & 0x08)
				{
					//最高位1
					int height = (ctoi(buffer[nStartIndex]) & 0x07) * 8;
					int low = ctoi(buffer[++nStartIndex]) & 0x0f;
					unsigned int lengthSize = 2 * (height + low);

					nStartIndex += 1; //从下一个字节开始算Length域
					pBCDTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
					memcpy(pBCDTlvEntity[currentTLVIndex].Length, buffer + nStartIndex, lengthSize);
					pBCDTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
					pBCDTlvEntity[currentTLVIndex].LengthSize = lengthSize;

					subTlvLength = 2 * stoi((char*)pBCDTlvEntity[currentTLVIndex].Length, 0, 16);

					//申请一段subTlvlength大小的内存存放该TLV的内容
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + nStartIndex + lengthSize, subTlvLength);
				}
				else
				{
					//最高位为0
					char subTlvLen[3] = { 0 };
					memcpy(subTlvLen, &buffer[currentIndex], 2);
					subTlvLength = 2 * stoi(subTlvLen, 0, 16);	
					temp = (unsigned char *)malloc(subTlvLength);
					memset(temp, 0, subTlvLength);
					memcpy(temp, buffer + currentIndex + 2, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//输出有多少个同等级的子TLV，解析时也应该用到

				//不清楚子TLV同等级的TLV有多少个，申请32TLV大小的内存肯定够用
				pBCDTlvEntity[currentTLVIndex].subTLVEntity = (PBCD_TLV)malloc(sizeof(BCD_TLV[32]));
				TLVConstruct(temp, subTlvLength, pBCDTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				pBCDTlvEntity[currentTLVIndex].subTLVnum = oSize; //填入子TLV的数量
			}
			else
			{
				//单一结构
				pBCDTlvEntity[currentTLVIndex].isTemplate = false;
				pBCDTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				pBCDTlvEntity[currentTLVIndex].subTLVnum = 0;

				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//多字节
					int endTagIndex = tempIndex + 1;
					while (ctoi(buffer[endTagIndex]) & 0x08)
					{
						endTagIndex += 2;
					}
					int tagSize = endTagIndex - currentIndex + 2;

					pBCDTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(tagSize);
					memcpy(pBCDTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, tagSize);
					pBCDTlvEntity[currentTLVIndex].Tag[tagSize] = 0;

					pBCDTlvEntity[currentTLVIndex].TagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//单字节
					pBCDTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(2);
					memcpy(pBCDTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 2);
					pBCDTlvEntity[currentTLVIndex].Tag[2] = 0;

					pBCDTlvEntity[currentTLVIndex].TagSize = 2;

					currentIndex += 2;
				}				
			}
			currentStatus = 'L';
			break;
		case 'L':
			//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
			if (ctoi(buffer[currentIndex]) & 0x08)
			{
				//最高位1
				int height = (ctoi(buffer[currentIndex]) & 0x07) * 8;
				int low = ctoi(buffer[++currentIndex]) & 0x0f;
				unsigned int lengthSize = 2 * (height + low);

				currentIndex += 1; //从下一个字节开始算Length域
				pBCDTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
				memcpy(pBCDTlvEntity[currentTLVIndex].Length, buffer + currentIndex, lengthSize);
				pBCDTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
				pBCDTlvEntity[currentTLVIndex].LengthSize = lengthSize;
				valueSize = 2 * stoi((char*)pBCDTlvEntity[currentTLVIndex].Length, 0, 16);

				currentIndex += lengthSize;
				
			}
			else
			{
				//最高位0
				pBCDTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(2);
				memcpy(pBCDTlvEntity[currentTLVIndex].Length, buffer + currentIndex, 2);
				pBCDTlvEntity[currentTLVIndex].Length[2] = 0;
				pBCDTlvEntity[currentTLVIndex].LengthSize = 2;

				valueSize = 2 * stoi((char*)pBCDTlvEntity[currentTLVIndex].Length, 0, 16);

				currentIndex += 2;
			}
			currentStatus = 'V';
			break;
		case 'V':
			pBCDTlvEntity[currentTLVIndex].Value = (unsigned char *)malloc(valueSize);
			memset(pBCDTlvEntity[currentTLVIndex].Value, 0, valueSize);
			memcpy(pBCDTlvEntity[currentTLVIndex].Value, buffer + currentIndex, valueSize);
			pBCDTlvEntity[currentTLVIndex].Value[valueSize] = 0;

			currentIndex += valueSize;

			//进入下一个TLV构造循环
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}


void TLVParaser::TLVConstruct(unsigned char *buffer,			//TLV字符串	
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

bool TLVParaser::TLVParseAndFindError(
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

//将字符串型十六进制转为整形
int TLVParaser::ctoi(unsigned char c)
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