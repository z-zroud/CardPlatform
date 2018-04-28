// DataParse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "IDataParse.h"
#include "../Util/ParseTLV.h"
#include "../Util/Tool.h"
using namespace std;
using namespace Tool;
//���ַ�����ʮ������תΪ����


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
                char szLen[5] = { 0 };
				memcpy(szLen, buffer + currentIndex, lengthSize);
                pTls[currentTLVIndex].len = std::stoi(szLen, 0, 16);
				currentIndex += lengthSize;

			}
			else
			{
				//���λ0
                char szLen[5] = { 0 };
				memcpy(szLen, buffer + currentIndex, 2);
                pTls[currentTLVIndex].len = std::stoi(szLen, 0, 16);
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


void _ParseTLVEx(PBCD_TLV ptlvs, unsigned int tlvCount, int level, vector<TLV>& vecTlv)
{
    for (int i = 0; i < tlvCount; i++)
    {
        int count = 0;
        if (ptlvs[i].isTemplate)
        {
            TLV tlv;
            tlv.isTemplate = true;
            tlv.level = level;
            tlv.tag = ptlvs[i].tag;
            tlv.value = ptlvs[i].value;
            tlv.length = stoi((char*)ptlvs[i].length, 0, 16);
            vecTlv.push_back(tlv);

            int subLevel = level + 1;
            _ParseTLVEx(ptlvs[i].subTLVEntity, ptlvs[i].subTLVnum, subLevel, vecTlv);
        }
        else {
           // Log->Info("[%s]=%s", entities[i].Tag, entities[i].Value);
            TLV tlv;
            tlv.isTemplate = false;
            tlv.level = level;
            tlv.tag = ptlvs[i].tag;
            tlv.value = ptlvs[i].value;
            tlv.length = stoi((char*)ptlvs[i].length, 0, 16);
            vecTlv.push_back(tlv);
        }
    }
}

bool IsTLV(char* bcdBuffer, int buffLen)
{
    return IsBcdTlvStruct(bcdBuffer, buffLen);
}

bool ParseTLV(char* buffer, PTLV pTlvs, unsigned int& count)
{
    BCD_TLV tlvs[32] = { 0 };
    unsigned int tlvCount = 32;
    if (!ParseBcdTLV(buffer, tlvs, tlvCount)) {
        return false;
    }
    vector<TLV> vecTlvs;
    _ParseTLVEx(tlvs, tlvCount, 0, vecTlvs);
    int tlvExSize = vecTlvs.size();
    for (int i = 0; i < tlvExSize; i++) {
        if (i >= count) {
            return false;
        }
        pTlvs[i] = vecTlvs[i];
        //printf("tag=%s,len=%d,value=%s,level=%d,isTemplate=%d\n", pTlvs[i].tag, pTlvs[i].length, pTlvs[i].value, pTlvs[i].level, pTlvs[i].isTemplate);
    }
    count = tlvExSize;  
    return true;
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

        int needSigRecord = 0;
		for (int k = startRecord; k <= endRecord; k++)
		{
			afl.recordNumber = k;
			if (nAcceptAuthencation > 0 && needSigRecord < nAcceptAuthencation)
			{
				afl.bSigStaticData = true;
                needSigRecord++;
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