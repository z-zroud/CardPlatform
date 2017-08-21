#include "StdAfx.h"
#include "HTDPParser.h"
#include "Util\Log.h"


HTDPParaser::HTDPParaser()
{
    m_micData[MIC_CARD_SURFACE].tags = MIC_CARD_DATA_TAG;
    m_micData[MIC_TRIP_DATA].tags = MIC_TRIP_DATA_TAG;
    m_micData[MIC_CARD_DATA].tags = MIC_CARD_DATA_TAG;
}

int HTDPParaser::Read(const string& filePath)
{
    ifstream dpFile(filePath.c_str(), ios::binary);
    if (!dpFile)
        return -1;

    //��һ������ȡ�����
    ReadCardSeq(dpFile, dpFile.tellg());

    //�ڶ�������ȡ����
    ReadCardCateGory(dpFile, dpFile.tellg());

    //�������� ��ȡ�ƿ�����
    ReadCardType(dpFile, dpFile.tellg());

    //���Ĳ��� ��ȡ�����ݳ���
    ReadCardDataLen(dpFile, dpFile.tellg());

    //���岽�� ��ȡIC������
    ReadCardData(dpFile, dpFile.tellg());

    //�������� ��ȡHASHֵ
    ReadHash(dpFile, dpFile.tellg());

    return 0;
}

void HTDPParaser::ReadCardSeq(ifstream &dpFile, streamoff offset)
{
    char szCardSeq[5] = { 0 };
    dpFile.read(szCardSeq, 4);
    
    m_nCardSeq = stoi(szCardSeq, 0, 16);
}

void HTDPParaser::ReadCardCateGory(ifstream &dpFile, streamoff offset)
{
    char szCardCategory[11] = { 0 };
    dpFile.read(szCardCategory, 10);

    m_sCardCategory = string(szCardCategory);
}

void HTDPParaser::ReadCardType(ifstream &dpFile, streamoff offset)
{
    char szCardType[2] = { 0 };
    dpFile.read(szCardType, 1);

    m_sCardType = string(szCardType);
}

int HTDPParaser::ReadCardDataLen(ifstream &dpFile, streamoff offset)
{
    char szCardDataLen[5] = { 0 };
    dpFile.read(szCardDataLen, 4);

    int nDataLen = stoi(szCardDataLen, 0, 16);

    return nDataLen;
}

void HTDPParaser::ReadHash(ifstream &dpFile, streamoff offset)
{
    char szHash[41] = { 0 };
    dpFile.read(szHash, 40);

    m_sHash = string(szHash);
}


int HTDPParaser::ReadCardData(ifstream &dpFile, streamoff offset)
{
    for (int i = MIC_CARD_SURFACE; i < MIC_CARD_DATA; i++)
    {
        char tag[7] = { 0 };
        dpFile.read(tag, m_micData[i].tags.length());
        if (string(tag) != m_micData[i].tags)
        {
            Log->Error("�޷���������IC����");
            return -1;
        }
        char len[5] = { 0 };
        dpFile.read(len, 4);
        int dataLen = stoi(len, 0, 16);

        m_micData[i].dataLen = dataLen;

        char* sData = new char[dataLen + 1];
        memset(sData, 0, dataLen + 1);

        dpFile.read(sData, dataLen);
        m_micData[i].data = string(sData);

        delete sData;
    }

    return 0;
}