#pragma once

#ifdef DATAPARSE_EXPORTS
#define DATAPARSE_EXPORTS_API __declspec(dllexport)
#else
#define DATAPARSE_EXPORTS_API __declspec(dllimport)
#endif

/*****************************************************
* 定义此TLV结构，有利于阅读及解析
******************************************************/
typedef struct TLV
{
    bool            isTemplate;     //是否是模板
    int             level;          //层级
    char*           tag;            //标签
    unsigned int    length;         //长度
    char*           value;          //值
}*PTLV;

typedef struct TL
{
	unsigned char* tag;		//标签
	unsigned int len;		//长度
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
extern "C" DATAPARSE_EXPORTS_API void ParseTL(char* bcdBuffer, PTL pTls, unsigned int& count);

/**********************************************************************
* 解析TLV数据结构
* 参数： buffer 待解析的TLV数据
* 参数： pTlvs 解析之后的TLV数组
* 参数： count 包含TLV的个数
* 返回： true 说明解析成功，false TLV格式不正确
**********************************************************************/
//extern "C" DATAPARSE_EXPORTS_API bool ParseTLV(char* bcdBuffer, PTLV pTlvs, unsigned int& count);


/*********************************************************************
* 解析TLV数据结构
*
* 返回： true 说明解析成功，false TLV格式不正确
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API bool ParseTLV(char* bcdBuffer, PTLV pTlv, unsigned int& count);


extern "C" DATAPARSE_EXPORTS_API bool IsTLV(char* bcdBuffer, int buffLen);



/**********************************************************************
* 解析AFL结构，方便后续读取记录数据
* 参数： buffer 待解析的TLV数据
* 参数： pTlvs 解析之后的TLV数组
* 参数： count 包含TLV的个数
**********************************************************************/
extern "C" DATAPARSE_EXPORTS_API void ParseAFL(char* bcdBuffer, PAFL pAfls, unsigned int& count);