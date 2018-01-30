#pragma once
#include <Windows.h>

#define SM_OK 0
//*******************************
//��Բ����ǩ���㷨
//������: dllSM2_Sign
//����1 : pPrivateKey ˽Կ
//����2 : pMSG ǩ����Ϣ
//����3 : pSignData(�������) ǩ������(R|S)
//����ֵ: 0 - ǩ���ɹ�
//      : <0 - ǩ��ʧ��
//*******************************
typedef int(*PDllSM2_Sign)(char *pPrivateKey, char *pMSG, char *pSignData);

//*******************************
//��Բ������֤ǩ���㷨
//������: dllSM2_Verify
//����1 : pPublicKey ��Կ
//����2 : pMSG ǩ����Ϣ
//����3 : pSignData ��ǩ����(R|S)
//����ֵ: 0 - ��ǩ�ɹ�
//      : <0 - ��ǩʧ��
//*******************************
typedef int(*PDllSM2_Verify)(char *pPublicKey, char *pMSG, char *pSignData);

//*******************************
//����һ�ԷǶԳ���Կ��
//������: dllSM2_GenKey
//����1 : pPrivateKey ˽Կֵ
//����1 : pPublicKey ��Կֵ
//����ֵ: 0 - ���ɳɹ�
//      : <0 - ����ʧ��
//*******************************
typedef int(*PDllSM2_GenKey)(char *pPrivateKey, char *pPublicKey);

//*******************************
//ʹ��SM3�Ӵ��㷨�����ϣֵ
//������: dllSM3_HASH
//����1 : input ����HASHֵ����������
//����2 : ilen �������ݵĳ��ȣ���λ(�ֽ�)
//����3 : output(�������) ��ϣֵ,32�ֽ�,64λ
//����ֵ: void
//*******************************
typedef void(*PDllSM3_HASH)(char *input, int inlen, char *output);

//*******************************
//ʹ��SM4�㷨��������,�㷨ΪECB
//������: dllSM4_ECB_ENC
//����1 : sm4Key ����KEY
//����2 : input ������������
//����3 : output(�������) ��������
//����ֵ: void
//*******************************
typedef void(*PDllSM4_ECB_ENC)(char *sm4Key, char *input, char *output);

//*******************************
//ʹ��SM4�㷨��������,�㷨ΪECB
//������: dllSM4_ECB_DEC
//����1 : sm4Key ����KEY
//����2 : input ��������
//����3 : output(�������) ���ܺ����������
//����ֵ: void
//*******************************
typedef void(*PDllSM4_ECB_DEC)(char *sm4Key, char *input, char *output);

//*******************************
//ʹ��SM4�㷨��������,�㷨ΪCBC
//������: dllSM4_CBC_ENC
//����1 : sm4Key ����KEY
//����2 : input ������������
//����3 : output(�������) ��������
//����ֵ: void
//*******************************
typedef void(*PDllSM4_CBC_ENC)(char *sm4Key, char *input, char *output);

//*******************************
//ʹ��SM4�㷨��������,�㷨ΪCBC
//������: dllSM4_CBC_DEC
//����1 : sm4Key ����KEY
//����2 : input ��������
//����3 : output(�������) ���ܺ����������
//����ֵ: void
//*******************************
typedef void(*PDllSM4_CBC_DEC)(char *sm4Key, char *input, char *output);

//*******************************
//ʹ��SM4�㷨����PBOC MAC
//������: dllSM4_CBC_MAC
//����1 : sm4Key ����KEY
//����2 : input ������������
//����3 : InitVec ��ʼ����
//����4 : output(�������) ��������
//����ֵ: void
//*******************************
typedef void(*PDllSM4_CBC_MAC)(char *sm4Key, char *input, char *InitVec, char *output);

//*******************************
//ʹ��SM3�㷨����PBOC3.0��ϢժҪHashֵ,h = SM3(Za|MSG)
//������: dllGET_PBOCMSG_HASH
//����1 : ��ϢժҪ
//����2 : ilen �������ݵĳ��ȣ���λ(�ֽ�)
//����3 : output(�������) Hashֵ
//����ֵ: void
//*******************************
typedef void(*PDllPBOC_GETMSG_HASH)(char *pMSG, char *pPublicKey, char *output);

//*******************************
//PBOC3.0����֤��ǩ������
//������: dllPBOC_SM2_Sign
//����1 : pPrivateKey ˽Կ
//����2 : pPublicKey ��Կ
//����3 : pMSG ǩ����Ϣ
//����4 : pSignData(�������) ǩ������(R|S)
//����ֵ: 0 - ǩ���ɹ�
//      : <0 - ǩ��ʧ��
//*******************************
typedef int(*PDllPBOC_SM2_Sign)(char *pPrivateKey, char *pPublicKey, char *pMSG, char *pSignData);

//*******************************
//PBOC3.0����֤����ǩ����
//������: dllPBOC_SM2_Verify
//����1 : pPublicKey ��Կ
//����2 : pMSG ǩ����Ϣ
//����3 : pSignData ��ǩ����(R|S)
//����ֵ: 0 - ��ǩ�ɹ�
//      : <0 - ��ǩʧ��
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
