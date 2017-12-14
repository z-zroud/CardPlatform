// DataParse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "IDataParse.h"

using namespace std;

//���ַ�����ʮ������תΪ����
int ctoi(unsigned char c)
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

void ParseTL(char* buffer, PTL pTls, unsigned int& count)
{
	unsigned int	currentIndex = 0;			//���ڱ��buffer
	unsigned long	valueSize = 0;				//���ݳ���
	int				currentTLVIndex = 0;		//��ǰTL�ṹ���
	int				currentStatus = 'T';		//״̬�ַ�

	unsigned int bufferLen = strlen(buffer);
	while (currentIndex < bufferLen)
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

				pTls[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
				memcpy(pTls[currentTLVIndex].tag, buffer + currentIndex, tagSize);
				pTls[currentTLVIndex].tag[tagSize] = 0;

				currentIndex += tagSize;
			}
			else
			{
				//���ֽ�
				pTls[currentTLVIndex].tag = (unsigned char *)malloc(2);
				memcpy(pTls[currentTLVIndex].tag, buffer + currentIndex, 2);
				pTls[currentTLVIndex].tag[2] = 0;

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
				pTls[currentTLVIndex].len = (unsigned char *)malloc(lengthSize);
				memcpy(pTls[currentTLVIndex].len, buffer + currentIndex, lengthSize);
				pTls[currentTLVIndex].len[lengthSize] = 0;

				valueSize = 2 * std::stoi((char*)pTls[currentTLVIndex].len, 0, 16);

				currentIndex += lengthSize;

			}
			else
			{
				//���λ0
				pTls[currentTLVIndex].len = (unsigned char *)malloc(2);
				memcpy(pTls[currentTLVIndex].len, buffer + currentIndex, 2);
				pTls[currentTLVIndex].len[2] = 0;

				valueSize = 2 * std::stoi((char*)pTls[currentTLVIndex].len, 0, 16);

				currentIndex += 2;
			}
			currentTLVIndex++;
			currentStatus = 'T';
			break;
		}
		}
	}
	count = currentTLVIndex;
}





void ParseTLV(char* buffer, PTLV pTlvs, unsigned int& count)
{
	unsigned int	currentIndex = 0;				//���ڱ��buffer
	unsigned long	dataSize = 0;					//���ݳ���
	int				currentTLVIndex = 0;			//��ǰTLV�ṹ���
	int				currentStatus = 'T';			//״̬�ַ�
	unsigned int bufferLen = strlen(buffer);

	while (currentIndex < bufferLen)
	{
		switch (currentStatus)
		{
		case 'T':
			dataSize = 0; //����			
			if (ctoi(buffer[currentIndex]) & 0x02)	//�ж�TLV�Ƿ�Ϊ��һ�ṹ,�ֽڵĵ�6λ�Ƿ�Ϊ1
			{//���Ͻṹ ˵����ģ��				
				pTlvs[currentTLVIndex].isTemplate = true;				
				unsigned int tempIndex = currentIndex;
				if ((ctoi(buffer[tempIndex]) & 0x01) && (ctoi(buffer[++tempIndex]) == 0x0F))	//�ж�tag�Ƿ�Ϊ���ֽ�
				{//tagΪ���ֽ�					
					int endTagIndex = tempIndex + 1;
					while (ctoi(buffer[endTagIndex]) & 0x08)
					{
						endTagIndex += 2;
					}
					int tagSize = endTagIndex - currentIndex + 2;

					pTlvs[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					pTlvs[currentTLVIndex].tagSize = tagSize;
					currentIndex += tagSize;
				}
				else
				{ //tagΪ���ֽ�
					pTlvs[currentTLVIndex].tag = (unsigned char *)malloc(2);
					memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, 2);
					pTlvs[currentTLVIndex].tag[2] = 0;
					pTlvs[currentTLVIndex].tagSize = 2;
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
					pTlvs[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
					memcpy(pTlvs[currentTLVIndex].length, buffer + nStartIndex, lengthSize);
					pTlvs[currentTLVIndex].length[lengthSize] = 0;
					pTlvs[currentTLVIndex].lenSize = lengthSize;

					subTlvLength = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

					//����һ��subTlvlength��С���ڴ��Ÿ�TLV������
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + nStartIndex + lengthSize, subTlvLength);
				}
				else
				{
					//���λΪ0
					char subTlvLen[3] = { 0 };
					memcpy(subTlvLen, &buffer[currentIndex], 2);
					subTlvLength = 2 * std::stoi(subTlvLen, 0, 16);
					temp = (unsigned char *)malloc(subTlvLength);
					memset(temp, 0, subTlvLength);
					memcpy(temp, buffer + currentIndex + 2, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//����ж��ٸ�ͬ�ȼ�����TLV������ʱҲӦ���õ�

				//�������TLVͬ�ȼ���TLV�ж��ٸ�������32TLV��С���ڴ�϶�����
				pTlvs[currentTLVIndex].subTLVEntity = (PTLV)malloc(sizeof(TLV[32]));
				ParseTLV((char*)temp, pTlvs[currentTLVIndex].subTLVEntity, oSize);

				pTlvs[currentTLVIndex].subTLVnum = oSize; //������TLV������
			}
			else
			{
				//��һ�ṹ
				pTlvs[currentTLVIndex].isTemplate = false;
				pTlvs[currentTLVIndex].subTLVEntity = NULL;
				pTlvs[currentTLVIndex].subTLVnum = 0;

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

					pTlvs[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					pTlvs[currentTLVIndex].tag[tagSize] = 0;

					pTlvs[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//���ֽ�
					pTlvs[currentTLVIndex].tag = (unsigned char *)malloc(2);
					memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, 2);
					pTlvs[currentTLVIndex].tag[2] = 0;

					pTlvs[currentTLVIndex].tagSize = 2;

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
				pTlvs[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
				memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, lengthSize);
				pTlvs[currentTLVIndex].length[lengthSize] = 0;
				pTlvs[currentTLVIndex].lenSize = lengthSize;
				dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

				currentIndex += lengthSize;

			}
			else
			{
				//���λ0
				pTlvs[currentTLVIndex].length = (unsigned char *)malloc(2);
				memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, 2);
				pTlvs[currentTLVIndex].length[2] = 0;
				pTlvs[currentTLVIndex].lenSize = 2;

				dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

				currentIndex += 2;
			}
			currentStatus = 'V';
			break;
		case 'V':
			pTlvs[currentTLVIndex].value = (unsigned char *)malloc(dataSize);
			memset(pTlvs[currentTLVIndex].value, 0, dataSize);
			memcpy(pTlvs[currentTLVIndex].value, buffer + currentIndex, dataSize);
			pTlvs[currentTLVIndex].value[dataSize] = 0;

			currentIndex += dataSize;

			//������һ��TLV����ѭ��
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	count = currentTLVIndex;
}

void ParseAFL(char* buffer, PAFL pAfls, unsigned int& count)
{
	unsigned int bufferLen = strlen(buffer);
	string aflBuffer = buffer;
	int index = 0;
	for (unsigned int i = 0; i < bufferLen; i += 8)
	{
		AFL afl;
		afl.sfi = ((ctoi(aflBuffer[i]) & 0x0F) << 1) + (ctoi(aflBuffer[i + 1]) && 0x08);
		string strStartRecord	= aflBuffer.substr(i + 2, 2);
		string strEndRecord		= aflBuffer.substr(i + 4, 2);
		string strNeedValidate	= aflBuffer.substr(i + 6, 2);

		int startRecord			= stoi(strStartRecord, 0, 16);
		int endRecord			= stoi(strEndRecord, 0, 16);
		int nAcceptAuthencation	= stoi(strNeedValidate, 0, 16);

		for (int k = startRecord; k <= endRecord; k++)
		{
			afl.recordNumber = k;
			if (nAcceptAuthencation >= k)
			{
				afl.bSigStaticData = true;
			}
			else {
				afl.bSigStaticData = false;
			}
			pAfls[index] = afl;
			index++;
		}
	}
	count = index;
}