#include "stdafx.h"
#include "TLVParaser.h"
#include <ctype.h>
#include <string>

using namespace std;


void TLVParaser::TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PBCD_TL pTlEntity, unsigned int& entityNum)
{
	unsigned int currentIndex = 0;		//���ڱ��buffer
	int currentTLVIndex = 0;			//��ǰTL�ṹ���
	int currentStatus = 'T';			//״̬�ַ�

	unsigned long valueSize = 0;		//���ݳ���

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			{
				valueSize = 0; //����
				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//���ֽ�
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
					//���ֽ�
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
				//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
				if (ctoi(buffer[currentIndex]) & 0x08)
				{
					//���λ1
					unsigned int lengthSize = 2 * ((buffer[currentIndex] & 0x07) * 8 + (buffer[++currentIndex] & 0x0f));

					currentIndex += 1; //����һ���ֽڿ�ʼ��Length��
					pTlEntity[currentTLVIndex].len = (unsigned char *)malloc(lengthSize);
					memcpy(pTlEntity[currentTLVIndex].len, buffer + currentIndex, lengthSize);
					pTlEntity[currentTLVIndex].len[lengthSize] = 0;

					valueSize = 2 * stoi((char*)pTlEntity[currentTLVIndex].len, 0, 16);

					currentIndex += lengthSize;

				}
				else
				{
					//���λ0
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
	unsigned int currentIndex = 0;		//���ڱ��buffer
	int currentTLVIndex = 0;			//��ǰTLV�ṹ���
	int currentStatus = 'T';			//״̬�ַ�

	unsigned long valueSize = 0;		//���ݳ���

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //����

			//�ж�TLV�Ƿ�Ϊ��һ�ṹ,�ֽڵĵ�6λ�Ƿ�Ϊ1
			if (ctoi(buffer[currentIndex]) & 0x02)
			{
				//���Ͻṹ
				pBCDTlvEntity[currentTLVIndex].isTemplate = true;
				//�ж��Ƿ�Ϊ���ֽ�
				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//���ֽ�
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
					//���ֽ�
					pBCDTlvEntity[currentTLVIndex].Tag = (unsigned char *)malloc(2);
					memcpy(pBCDTlvEntity[currentTLVIndex].Tag, buffer + currentIndex, 2);
					pBCDTlvEntity[currentTLVIndex].Tag[2] = 0;

					pBCDTlvEntity[currentTLVIndex].TagSize = 2;

					currentIndex += 2;
				}

				//������TLV�е�Tag
				int subTlvLength = 0;			//��TLV����
				unsigned char * temp;			//��TLV������������
				//���ж�length��ĳ���,length���ֽ�������λΪ1�������ֽڴ����ȣ�Ϊ0��1--7λ�������ݳ���
				unsigned int nStartIndex = currentIndex;
				if (ctoi(buffer[nStartIndex]) & 0x08)
				{
					//���λ1
					int height = (ctoi(buffer[nStartIndex]) & 0x07) * 8;
					int low = ctoi(buffer[++nStartIndex]) & 0x0f;
					unsigned int lengthSize = 2 * (height + low);

					nStartIndex += 1; //����һ���ֽڿ�ʼ��Length��
					pBCDTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
					memcpy(pBCDTlvEntity[currentTLVIndex].Length, buffer + nStartIndex, lengthSize);
					pBCDTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
					pBCDTlvEntity[currentTLVIndex].LengthSize = lengthSize;

					subTlvLength = 2 * stoi((char*)pBCDTlvEntity[currentTLVIndex].Length, 0, 16);

					//����һ��subTlvlength��С���ڴ��Ÿ�TLV������
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + nStartIndex + lengthSize, subTlvLength);
				}
				else
				{
					//���λΪ0
					char subTlvLen[3] = { 0 };
					memcpy(subTlvLen, &buffer[currentIndex], 2);
					subTlvLength = 2 * stoi(subTlvLen, 0, 16);	
					temp = (unsigned char *)malloc(subTlvLength);
					memset(temp, 0, subTlvLength);
					memcpy(temp, buffer + currentIndex + 2, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//����ж��ٸ�ͬ�ȼ�����TLV������ʱҲӦ���õ�

				//�������TLVͬ�ȼ���TLV�ж��ٸ�������32TLV��С���ڴ�϶�����
				pBCDTlvEntity[currentTLVIndex].subTLVEntity = (PBCD_TLV)malloc(sizeof(BCD_TLV[32]));
				TLVConstruct(temp, subTlvLength, pBCDTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				pBCDTlvEntity[currentTLVIndex].subTLVnum = oSize; //������TLV������
			}
			else
			{
				//��һ�ṹ
				pBCDTlvEntity[currentTLVIndex].isTemplate = false;
				pBCDTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				pBCDTlvEntity[currentTLVIndex].subTLVnum = 0;

				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))
				{
					//���ֽ�
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
					//���ֽ�
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
			//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
			if (ctoi(buffer[currentIndex]) & 0x08)
			{
				//���λ1
				int height = (ctoi(buffer[currentIndex]) & 0x07) * 8;
				int low = ctoi(buffer[++currentIndex]) & 0x0f;
				unsigned int lengthSize = 2 * (height + low);

				currentIndex += 1; //����һ���ֽڿ�ʼ��Length��
				pBCDTlvEntity[currentTLVIndex].Length = (unsigned char *)malloc(lengthSize);
				memcpy(pBCDTlvEntity[currentTLVIndex].Length, buffer + currentIndex, lengthSize);
				pBCDTlvEntity[currentTLVIndex].Length[lengthSize] = 0;
				pBCDTlvEntity[currentTLVIndex].LengthSize = lengthSize;
				valueSize = 2 * stoi((char*)pBCDTlvEntity[currentTLVIndex].Length, 0, 16);

				currentIndex += lengthSize;
				
			}
			else
			{
				//���λ0
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

			//������һ��TLV����ѭ��
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}


void TLVParaser::TLVConstruct(unsigned char *buffer,			//TLV�ַ���	
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

bool TLVParaser::TLVParseAndFindError(
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

//���ַ�����ʮ������תΪ����
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