#pragma once
/***********************************************
* 该头文件定义了所有与卡片操作有关的相关结构体、枚举、
* 常量等相关信息，它是卡片操作的基础文件。
************************************************/
#include <string>
#include <vector>

using namespace std;

//定义分散方法
#define NOT_DIV      _T("Not Div")
#define CPG202      _T("CPG2.0.2")
#define CPG212      _T("CPG2.1.2")

//定义安全等级
#define NO_SECURE   _T("NO SECURE")
#define MAC         _T("MAC")

#define Status_IssuerSecurityDomain	0x80			//发行者安全域
#define Status_ApplicationAndSecurityDomain	0x40	//应用和安全域
#define Status_ExecutableLoadFile	0x20		//可执行装载文件
#define Status_ExecutableModule	0x10		//可执行模块


//卡片状态
typedef enum tagCardStatus
{
	UNKNOWN = 0,
	ABSENT,
	PRESENT,
	SWALLOWED,
	POWERED,
	NEGOTIABLE,
	SPECIFIC,
}CARD_STATUS;


//读卡器与卡片的数据传输协议
typedef enum tagCardTransmissonProtocol
{
	UNDEFINED = 0,
	T0,
	T1,
	RAW
}CARD_TRANSMISSION_PROTOCOL;


//分散方法
enum DIV_METHOD_FLAG
{
	DIV_MIN = 0,
	NO_DIV = 0,
	DIV_CPG202,
	DIV_CPG212,
	DIV_MAX
};


//安全等级
enum SECURE_LEVEL
{
	SL_MIN = 0,
	SL_NO_SECURE = 0,
	SL_MAC,
	SL_MAC_ENC,
	SL_MAX
};


#define MAX_DATA_LEN 2048
struct APDU_REQUEST
{
	unsigned char CLA;
	unsigned char INS;
	unsigned char P1;
	unsigned char P2;
	unsigned char Lc;
	unsigned char Le;
	unsigned char data[MAX_DATA_LEN];
};

struct APDU_RESPONSE
{
	unsigned char SW1;
	unsigned char SW2;
	char data[MAX_DATA_LEN];
};

struct SESSION_KEY
{
	string sessionAuthKey;
	string sessionMacKey;
	string sessionEncKey;
};

struct SUB_KEY
{
	string sAuthKey;
	string sMacKey;
	string sEncKey;
};

struct CARD_KEY
{
	SESSION_KEY sessionKey;
	SUB_KEY staticKey;
};

enum STATICKEY
{
	AuthKey,
	MacKey,
	EncKey,
};

enum SESSIONKEY
{
	SessionAuthKey,
	SessionMacKey,
	SessionEncKey,
};

//GAC 引用控制参数
enum GACControlParam
{
	AAC,
	ARQC,
	TC,
	CDA
};


//应用文件定位器(AFL)
struct AFL
{
	int SFI;                    //短文件标识符
	int nRecordNo;              //记录号
	bool bAcceptAuthencation;   //是否需要参与脱机数据认证
};


struct CVMLIST
{
	string code;
	string condition;
};

//CVM列表
struct CVM
{
	string X;
	string Y;
	vector<CVMLIST> cvmList;
};

//终端验证结果
struct TVR
{
	bool OfflineDataValidationIsDone;			//未进行脱机数据认证
	bool SDAFailed;								//脱机静态数据认证失败
	bool ICCDataabsence;						//IC卡数据缺失
	bool CardInTermExceptionFile;				//卡片出现在终端异常文件中
	bool DDAFailed;								//脱机动态数据认证失败
	bool CDAFailed;								//CDA验证失败
	bool ApplicationVersionFailed;				//IC卡和终端应用版本不一致
	bool ApplicationExpired;					//应用已过期
	bool ApplicationNotEffect;					//应用尚未生效
	bool ServiceDenyByCard;						//卡片不允许所请求的服务
	bool IsNewCard;								//是否为新卡
	bool CardOnwerValidationFailed;				//持卡人验证失败
	bool UnknownCVM;							//未知的CVM
	bool PinRetryExceedLimitation;				//PIN重试次数超限
	bool RequirePinWithoutKeyborad;				//要求输入PIN,但密码键盘不存在
	bool RequirePinButNotInput;					//要求输入PIN,但尚未输入
	bool RequirePin;							//输入联机PIN
	bool TransExceedFloorLimitation;			//交易超过最低限额
	bool ExceedContinuedTransFloorLimitation;	//超过连续脱机交易下线
	bool ExceedContinuedTransUpLimitation;		//超过连续脱机交易上限
	bool TransUseRandomOnlineHandle;			//交易被随机选择联机处理
	bool CustomRequireOnlineTrans;				//商户要求联机交易
	bool UseDefaultTDOL;						//使用缺省TDOL
	bool IssuerValidationFailed;				//发卡行认证失败
	bool HandlePSFailedBeforeLastGAC;			//最后一次GAC命令之前脚本处理失败
	bool HandlePSFailedAfterLastGAC;			//最后一次GAC命令之后脚本处理失败
};

//终端交易结果
enum TERM_TRANS_TYPE
{
	TERM_AAC,
	TERM_ARQC,
	TERM_TC
};

typedef struct TLVEntity
{
	unsigned char* Tag;			//标签
	unsigned char* Length;		//长度
	unsigned char* Value;		//数据
	unsigned int TagSize;		//标签占用字节  一般两个字节
	unsigned int LengthSize;	//数据占用的字节
	TLVEntity* subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
	unsigned int subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
}TLV, *PTLV;

typedef struct BCD_TLVEntity
{
	unsigned char* Tag;			//标签
	unsigned char* Length;		//长度
	unsigned char* Value;		//数据
	unsigned int TagSize;		//标签占用字节  一般两个字节
	unsigned int LengthSize;	//数据占用的字节
	bool isTemplate;		//是否为复合结构，也就是这个tag就是template
	BCD_TLVEntity* subTLVEntity;	//嵌套的子TLV结构体,如果有的话，需要递归
	unsigned int subTLVnum;		//下一级的TLV数量，不包括下下级的TLV（记录subTLVEntity[]的大小）
}BCD_TLV, *PBCD_TLV;

typedef struct BCD_TLEntity
{
	unsigned char* tag;		//标签
	unsigned char* len;		//长度
}BCD_TL, *PBCD_TL;


struct APP_STATUS
{
	string strAID;			//AID
	string lifeCycleStatus;	//生命周期状态
	string privilege;		//应用权限
};

//应用选择的类型
enum APP_TYPE
{
    APP_PSE,
    APP_PPSE,
    APP_AID
};

//卡片通讯类型
enum COMMUNICATION_TYPE
{
    COMM_TOUCH,
    COMM_UNTOUCH
};

//安装应用类型
enum INSTALL_TYPE
{
    INSTALL_MIN = 0,
	INSTALL_APP = 0,
	INSTALL_PSE,
	INSTALL_PPSE,
	INSTALL_MAX
};

//应用安装参数结构体
struct INSTALL_PARAM
{
	string strExeLoadFileAID;
	string strExeModuleAID;
	string strApplicationAID;
	string strPrivilege;
	string strInstallParam;
	string strToken;
};

//存储的数据类型
enum STORE_DATA_TYPE
{
	STORE_DATA_COMMON,		//普通数据
	STORE_DATA_ENCRYPT,		//加密数据
	STORE_DATA_LAST			//最后一块数据
};

//卡片加密算法类型
enum ENCRYPT_TYPE
{
	ENCRYPT_DES,       //使用DES加密
	ENCRYPT_SM         //使用SM加密
};

//脱机数据认证类型
enum AUTHENCATE_TYPE
{          
    AUTH_DDA,        
    AUTH_CDA,
    AUTH_SDA,
};

//密钥类型
enum KEY_TYPE
{
    KEY_UDK,
    KEY_MDK 
};

//交易类型
enum TRANS_TYPE
{
    TRANS_PBOC,
    TRANS_QPBOC,
    TRANS_EC
};