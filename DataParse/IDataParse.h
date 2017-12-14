#pragma once

#ifdef DATAPARSE_EXPORTS
#define DATAPARSE_EXPORTS_API __declspec(dllexport)
#else
#define DATAPARSE_EXPORTS_API __declspec(dllimport)
#endif



typedef struct TLV
{
	unsigned char*	tag;			//标签
	unsigned char*	length;			//长度
	unsigned char*	value;			//数据
	unsigned int	tagSize;		//标签占用字节  一般两个字节
	unsigned int	lenSize;		//数据占用的字节
	bool			isTemplate;		//是否为复合结构，也就是这个tag就是template
	TLV*			subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
	unsigned int	subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
} *PTLV;

typedef struct TL
{
	unsigned char* tag;		//标签
	unsigned char* len;		//长度
}*PTL;

//应用文件定位器(AFL)
typedef struct AFL
{
	int		sfi;				//短文件标识符
	int		recordNumber;		//记录号
	bool	bSigStaticData;		//是否为签名的数据(表明哪些数据需要参与到签名中)
}*PAFL;

/**********************************************************************
* 解析DOL数据结构，即TL不包含value的TLV
* 参数： buffer 待解析的TL数据
* 参数： pTls 解析之后的TL数组
* 参数： count 包含TL的个数
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseTL(char* buffer, PTL pTls, unsigned int& count);

/**********************************************************************
* 解析TLV数据结构
* 参数： buffer 待解析的TLV数据
* 参数： pTlvs 解析之后的TLV数组
* 参数： count 包含TLV的个数
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseTLV(char* buffer, PTLV pTlvs, unsigned int& count);

/**********************************************************************
* 解析AFL结构，方便后续读取记录数据
* 参数： buffer 待解析的TLV数据
* 参数： pTlvs 解析之后的TLV数组
* 参数： count 包含TLV的个数
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseAFL(char* buffer, PAFL pAfls, unsigned int& count);