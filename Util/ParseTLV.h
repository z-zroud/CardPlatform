#pragma once


/**************************************************************
* TLV解析格式(用于DP数据文件的解析)
***************************************************************/
typedef struct ASCII_TLV
{
    unsigned char* tag;			//标签
    unsigned char* length;		//长度
    unsigned char* value;		//数据
    unsigned int tagSize;		//标签占用字节  一般两个字节
    unsigned int lengthSize;	//数据占用的字节
    bool         isTemplate;    //是否为模板
    ASCII_TLV* subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
    unsigned int subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
}*PASCII_TLV;


typedef struct BCD_TLV
{
    char*	tag;			//标签
    char*	length;			//长度
    char*	value;			//数据
    int	tagSize;		//标签占用字节  一般两个字节
    int	lenSize;		//数据占用的字节
    bool			isTemplate;		//是否为复合结构，也就是这个tag就是template
    BCD_TLV*			subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
    int	subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
} *PBCD_TLV;

bool ParseBcdTLV(char* buffer, PBCD_TLV pTlvs, unsigned int& count);

void ParseAsciiTLV(unsigned char *buffer, unsigned int bufferLength, PASCII_TLV PTlvEntity, unsigned int& entitySize);

bool IsAsciiTlvStruct(unsigned char* buffer, unsigned int bufferLength);
bool IsBcdTlvStruct(char* buffer, unsigned int bufferLength);