#pragma once
#include "DP\DPParaser.h"

#define MIC_CARD_SURFACE_TAG    _T("000PRN")    //卡面数据标识
#define MIC_TRIP_DATA_TAG       _T("000MAG")    //磁条数据
#define MIC_CARD_DATA_TAG       _T("000EMV")    //IC数据

#define MIC_CARD_SURFACE    0
#define MIC_TRIP_DATA       1
#define MIC_CARD_DATA       2

struct MIC_DATA
{
    string  tags;       //数据标识
    int     dataLen;    //数据长度
    string  data;       //数据
};

class HTDPParaser : public DPParser
{
public:
    HTDPParaser();
    ~HTDPParaser() {}
    int Read(const string& filePath);

private:
    void ReadCardSeq(ifstream &dpFile, streamoff offset);
    void ReadCardCateGory(ifstream &dpFile, streamoff offset);
    void ReadCardType(ifstream &dpFile, streamoff offset);
    int ReadCardDataLen(ifstream &dpFile, streamoff offset);
    int ReadCardData(ifstream &dpFile, streamoff offset);
    void ReadHash(ifstream &dpFile, streamoff offset);
private:
    int     m_nCardSeq;
    string  m_sCardCategory;
    string  m_sCardType;
    int     m_nCardDataLen;
    MIC_DATA m_micData[3];
    string  m_sHash;
};