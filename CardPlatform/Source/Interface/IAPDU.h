#pragma once
#include "CardBase.h"
/****************************************************
* 该接口定义了所有APDU指令集，符合PBOC3.0规范
*****************************************************/
struct IAPDU
{
	virtual bool SelectAppCmd(const string& aid, APDU_RESPONSE &response) = 0;
	virtual bool InitUpdateCmd(const string& randomNum, APDU_RESPONSE &response) = 0;	//个人化 更新初始化命令
	virtual bool DeleteAppCmd(const string& aid) = 0;	//删除命令

	virtual bool StoreDataCmd(const string& dgi, 
							const string& dgiData, 
							STORE_DATA_TYPE type, 
							bool reset,
							APDU_RESPONSE& response) = 0;	//加载数据

	virtual bool InstallAppCmd(const string& package, 
						const string& applet, 
						const string& instance,
						const string& privilege,
						const string& installParam,
						const string& token,
						APDU_RESPONSE& response) = 0;

	virtual bool ReadTagCmd(const string &tag, APDU_RESPONSE &response) = 0;	//取数据命令 获取标签
	virtual bool GetAppStatusCmd(vector<APP_STATUS> &status, APDU_RESPONSE& reponse) = 0;		//获取状态命令



	virtual bool ExternalAuthCommand(const string cardRandomNum,		//外部认证命令
		const string termRandomNum,
		const string sessionAuthKey,
		const string seesionMacKey,
		SECURE_LEVEL nSecureLevel,
		int nSCP,
		APDU_RESPONSE &strOutputData) = 0;
	
	virtual bool ExternalAuthcateCommand(const string ARPC, const string authCode, APDU_RESPONSE &response) = 0;	//外部认证命令
	virtual bool ReadRecordCommand(const string &strCommand, const string strRecordNumber, APDU_RESPONSE &response) = 0;	//读记录 命令
	virtual bool GPOCommand(const string &strCommand, APDU_RESPONSE &response) = 0;		//获取处理选项(GPO)命令
	virtual bool InternalAuthCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool PutDataCommand(const string &tag, const string &value, const string &mac) = 0;
    virtual bool PutKeyCommand(const string keyVersion,
        const string authKeyWithKcv,
        const string macKeyWithKcv,
        const string encKeyWithKcv) = 0;
	virtual bool UpdateRecordCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool GACCommand(GACControlParam p1, const string terminalData, APDU_RESPONSE &response) = 0;
	virtual bool WriteDataCommand(const string &strCommand, APDU_RESPONSE &response) = 0;
	virtual bool SendAPDU(string &strCommand, APDU_RESPONSE &response) = 0;	//发送APDU命令	
	virtual string GetAPDUError() = 0;	//获取命令执行失败原因
};