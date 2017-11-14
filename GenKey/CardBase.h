#pragma once
/***********************************************
* ��ͷ�ļ������������뿨Ƭ�����йص���ؽṹ�塢ö�١�
* �����������Ϣ�����ǿ�Ƭ�����Ļ����ļ���
************************************************/
#include <string>
#include <vector>

using namespace std;

//�����ɢ����
#define NOT_DIV      _T("Not Div")
#define CPG202      _T("CPG2.0.2")
#define CPG212      _T("CPG2.1.2")

//���尲ȫ�ȼ�
#define NO_SECURE   _T("NO SECURE")
#define MAC         _T("MAC")

#define Status_IssuerSecurityDomain	0x80			//�����߰�ȫ��
#define Status_ApplicationAndSecurityDomain	0x40	//Ӧ�úͰ�ȫ��
#define Status_ExecutableLoadFile	0x20		//��ִ��װ���ļ�
#define Status_ExecutableModule	0x10		//��ִ��ģ��


//��Ƭ״̬
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
string CardStatusToString(CARD_STATUS status);	//enum to string
#define CARD_STATUS_TO_STRING(status)	CardStatusToString(status)

//�������뿨Ƭ�����ݴ���Э��
typedef enum tagCardTransmissonProtocol
{
	UNDEFINED = 0,
	T0,
	T1,
	RAW
}CARD_TRANSMISSION_PROTOCOL;
string CTPToString(CARD_TRANSMISSION_PROTOCOL CTP);	//enum to string
#define CARD_TRANSMISSION_PROTOCOL_TO_STRING(CTP)	CTPToString(CTP)

//��ɢ����
//enum DIV_METHOD_FLAG
//{
//	DIV_MIN = 0,
//	NO_DIV = 0,
//	DIV_CPG202,
//	DIV_CPG212,
//	DIV_MAX
//};
//string DivToString(DIV_METHOD_FLAG flag);	//enum to string
//#define DIV_METHOD_FLAG_TO_STRING(flag)	DivToString(flag)

//��ȫ�ȼ�
enum SECURE_LEVEL
{
	SL_MIN = 0,
	SL_NO_SECURE = 0,
	SL_MAC,
	SL_MAC_ENC,
	SL_MAX
};
string SecureLevelToString(SECURE_LEVEL level);	//enum to string
#define SECURE_LEVEL_TO_STRING(level)	SecureLevelToString(level)

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

//GAC ���ÿ��Ʋ���
enum GACControlParam
{
	AAC,
	ARQC,
	TC,
	CDA
};


//Ӧ���ļ���λ��(AFL)
struct AFL
{
	int SFI;                    //���ļ���ʶ��
	int nRecordNo;              //��¼��
	bool bAcceptAuthencation;   //�Ƿ���Ҫ�����ѻ�������֤
};


struct CVMLIST
{
	string code;
	string condition;
};

//CVM�б�
struct CVM
{
	string X;
	string Y;
	vector<CVMLIST> cvmList;
};

//�ն���֤���
struct TVR
{
	bool OfflineDataValidationIsDone;			//δ�����ѻ�������֤
	bool SDAFailed;								//�ѻ���̬������֤ʧ��
	bool ICCDataabsence;						//IC������ȱʧ
	bool CardInTermExceptionFile;				//��Ƭ�������ն��쳣�ļ���
	bool DDAFailed;								//�ѻ���̬������֤ʧ��
	bool CDAFailed;								//CDA��֤ʧ��
	bool ApplicationVersionFailed;				//IC�����ն�Ӧ�ð汾��һ��
	bool ApplicationExpired;					//Ӧ���ѹ���
	bool ApplicationNotEffect;					//Ӧ����δ��Ч
	bool ServiceDenyByCard;						//��Ƭ������������ķ���
	bool IsNewCard;								//�Ƿ�Ϊ�¿�
	bool CardOnwerValidationFailed;				//�ֿ�����֤ʧ��
	bool UnknownCVM;							//δ֪��CVM
	bool PinRetryExceedLimitation;				//PIN���Դ�������
	bool RequirePinWithoutKeyborad;				//Ҫ������PIN,��������̲�����
	bool RequirePinButNotInput;					//Ҫ������PIN,����δ����
	bool RequirePin;							//��������PIN
	bool TransExceedFloorLimitation;			//���׳�������޶�
	bool ExceedContinuedTransFloorLimitation;	//���������ѻ���������
	bool ExceedContinuedTransUpLimitation;		//���������ѻ���������
	bool TransUseRandomOnlineHandle;			//���ױ����ѡ����������
	bool CustomRequireOnlineTrans;				//�̻�Ҫ����������
	bool UseDefaultTDOL;						//ʹ��ȱʡTDOL
	bool IssuerValidationFailed;				//��������֤ʧ��
	bool HandlePSFailedBeforeLastGAC;			//���һ��GAC����֮ǰ�ű�����ʧ��
	bool HandlePSFailedAfterLastGAC;			//���һ��GAC����֮��ű�����ʧ��
};

//�ն˽��׽��
enum TERM_TRANS_TYPE
{
	TERM_AAC,
	TERM_ARQC,
	TERM_TC
};

typedef struct TLVEntity
{
	unsigned char* Tag;			//��ǩ
	unsigned char* Length;		//����
	unsigned char* Value;		//����
	unsigned int TagSize;		//��ǩռ���ֽ�  һ�������ֽ�
	unsigned int LengthSize;	//����ռ�õ��ֽ�
	TLVEntity* subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
	unsigned int subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
}TLV, *PTLV;

typedef struct BCD_TLVEntity
{
	unsigned char* Tag;			//��ǩ
	unsigned char* Length;		//����
	unsigned char* Value;		//����
	unsigned int TagSize;		//��ǩռ���ֽ�  һ�������ֽ�
	unsigned int LengthSize;	//����ռ�õ��ֽ�
	bool isTemplate;		//�Ƿ�Ϊ���Ͻṹ��Ҳ�������tag����template
	BCD_TLVEntity* subTLVEntity;	//Ƕ�׵���TLV�ṹ��,����еĻ�����Ҫ�ݹ�
	unsigned int subTLVnum;		//��һ����TLV���������������¼���TLV����¼subTLVEntity[]�Ĵ�С��
}BCD_TLV, *PBCD_TLV;

typedef struct BCD_TLEntity
{
	unsigned char* tag;		//��ǩ
	unsigned char* len;		//����
}BCD_TL, *PBCD_TL;


struct APP_STATUS
{
	string strAID;			//AID
	string lifeCycleStatus;	//��������״̬
	string privilege;		//Ӧ��Ȩ��
};

//Ӧ��ѡ�������
enum APP_TYPE
{
    APP_PSE,
    APP_PPSE,
    APP_AID
};

//��ƬͨѶ����
enum COMMUNICATION_TYPE
{
    COMM_TOUCH,
    COMM_UNTOUCH
};

//��װӦ������
enum INSTALL_TYPE
{
    INSTALL_MIN = 0,
	INSTALL_APP = 0,
	INSTALL_PSE,
	INSTALL_PPSE,
	INSTALL_MAX
};

//Ӧ�ð�װ�����ṹ��
struct INSTALL_PARAM
{
	string strExeLoadFileAID;
	string strExeModuleAID;
	string strApplicationAID;
	string strPrivilege;
	string strInstallParam;
	string strToken;
};

////�洢����������
//enum STORE_DATA_TYPE
//{
//	STORE_DATA_COMMON,		//��ͨ����
//	STORE_DATA_ENCRYPT,		//��������
//	STORE_DATA_LAST			//���һ������
//};

//��Ƭ�����㷨����
enum ENCRYPT_TYPE
{
	ENCRYPT_DES,       //ʹ��DES����
	ENCRYPT_SM         //ʹ��SM����
};

//�ѻ�������֤����
enum OFFLINE_AUTHENCATE_TYPE
{          
    AUTH_DDA,        
    AUTH_CDA,
    AUTH_SDA,
};

//��Կ����
enum KEY_TYPE
{
    KEY_UDK,
    KEY_MDK 
};

//��������
enum TRANS_TYPE
{
    TRANS_PBOC,
    TRANS_QPBOC,
    TRANS_EC
};

enum DECRYPT_TYPE
{
	DES3_ECB,
};