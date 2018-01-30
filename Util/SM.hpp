#pragma once
#include <Windows.h>

#define SM_OK 0
//*******************************
//椭圆曲线签名算法
//函数名: dllSM2_Sign
//参数1 : pPrivateKey 私钥
//参数2 : pMSG 签名信息
//参数3 : pSignData(输出参数) 签名数据(R|S)
//返回值: 0 - 签名成功
//      : <0 - 签名失败
//*******************************
typedef int(*PDllSM2_Sign)(char *pPrivateKey, char *pMSG, char *pSignData);

//*******************************
//椭圆曲线验证签名算法
//函数名: dllSM2_Verify
//参数1 : pPublicKey 公钥
//参数2 : pMSG 签名信息
//参数3 : pSignData 验签数据(R|S)
//返回值: 0 - 验签成功
//      : <0 - 验签失败
//*******************************
typedef int(*PDllSM2_Verify)(char *pPublicKey, char *pMSG, char *pSignData);

//*******************************
//生成一对非对称密钥对
//函数名: dllSM2_GenKey
//参数1 : pPrivateKey 私钥值
//参数1 : pPublicKey 公钥值
//返回值: 0 - 生成成功
//      : <0 - 生成失败
//*******************************
typedef int(*PDllSM2_GenKey)(char *pPrivateKey, char *pPublicKey);

//*******************************
//使用SM3杂凑算法计算哈希值
//函数名: dllSM3_HASH
//参数1 : input 计算HASH值的输入数据
//参数2 : ilen 输入数据的长度，单位(字节)
//参数3 : output(输出参数) 哈希值,32字节,64位
//返回值: void
//*******************************
typedef void(*PDllSM3_HASH)(char *input, int inlen, char *output);

//*******************************
//使用SM4算法加密数据,算法为ECB
//函数名: dllSM4_ECB_ENC
//参数1 : sm4Key 解密KEY
//参数2 : input 加密明文数据
//参数3 : output(输出参数) 密文数据
//返回值: void
//*******************************
typedef void(*PDllSM4_ECB_ENC)(char *sm4Key, char *input, char *output);

//*******************************
//使用SM4算法解密数据,算法为ECB
//函数名: dllSM4_ECB_DEC
//参数1 : sm4Key 解密KEY
//参数2 : input 密文数据
//参数3 : output(输出参数) 解密后的明文数据
//返回值: void
//*******************************
typedef void(*PDllSM4_ECB_DEC)(char *sm4Key, char *input, char *output);

//*******************************
//使用SM4算法加密数据,算法为CBC
//函数名: dllSM4_CBC_ENC
//参数1 : sm4Key 解密KEY
//参数2 : input 加密明文数据
//参数3 : output(输出参数) 密文数据
//返回值: void
//*******************************
typedef void(*PDllSM4_CBC_ENC)(char *sm4Key, char *input, char *output);

//*******************************
//使用SM4算法解密数据,算法为CBC
//函数名: dllSM4_CBC_DEC
//参数1 : sm4Key 解密KEY
//参数2 : input 密文数据
//参数3 : output(输出参数) 解密后的明文数据
//返回值: void
//*******************************
typedef void(*PDllSM4_CBC_DEC)(char *sm4Key, char *input, char *output);

//*******************************
//使用SM4算法计算PBOC MAC
//函数名: dllSM4_CBC_MAC
//参数1 : sm4Key 解密KEY
//参数2 : input 加密明文数据
//参数3 : InitVec 初始向量
//参数4 : output(输出参数) 密文数据
//返回值: void
//*******************************
typedef void(*PDllSM4_CBC_MAC)(char *sm4Key, char *input, char *InitVec, char *output);

//*******************************
//使用SM3算法计算PBOC3.0信息摘要Hash值,h = SM3(Za|MSG)
//函数名: dllGET_PBOCMSG_HASH
//参数1 : 信息摘要
//参数2 : ilen 输入数据的长度，单位(字节)
//参数3 : output(输出参数) Hash值
//返回值: void
//*******************************
typedef void(*PDllPBOC_GETMSG_HASH)(char *pMSG, char *pPublicKey, char *output);

//*******************************
//PBOC3.0国密证书签名函数
//函数名: dllPBOC_SM2_Sign
//参数1 : pPrivateKey 私钥
//参数2 : pPublicKey 公钥
//参数3 : pMSG 签名信息
//参数4 : pSignData(输出参数) 签名数据(R|S)
//返回值: 0 - 签名成功
//      : <0 - 签名失败
//*******************************
typedef int(*PDllPBOC_SM2_Sign)(char *pPrivateKey, char *pPublicKey, char *pMSG, char *pSignData);

//*******************************
//PBOC3.0国密证书验签函数
//函数名: dllPBOC_SM2_Verify
//参数1 : pPublicKey 公钥
//参数2 : pMSG 签名信息
//参数3 : pSignData 验签数据(R|S)
//返回值: 0 - 验签成功
//      : <0 - 验签失败
//*******************************
typedef int(*PDllPBOC_SM2_Verify)(char *pPublicKey, char *pMSG, char *pSignData);


template <typename T>
T GetSMFunc(const char* funcName)
{
    static bool isLoaded = false;
    HINSTANCE hInst = NULL;
    T ret = NULL;
    if (!isLoaded)
    {
		char appPath[256] = { 0 };
		GetModuleFileName(NULL, appPath, 256);
		string temp = appPath;
		int pos = temp.find_last_of('\\');
		string dbPath = temp.substr(0, pos) + "\\ChineseSM.dll";
        hInst = LoadLibrary(dbPath.c_str());
        if (hInst == NULL)
        {
            return ret;
        }
        isLoaded = true;       
    }
    if(isLoaded)
        ret = (T)GetProcAddress(hInst, funcName);
    return ret;
}
