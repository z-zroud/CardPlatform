#pragma once
#ifdef PCSC_EXPORTS
#define PCSC_API __declspec(dllexport)
#else
#define PCSC_API __declspec(dllimport)
#endif

/******************************************************
* ����̬�ⶨ������PCSC��������صĽӿ�
*******************************************************/

/*****************************************************
* ���ܣ� ��ȡ��������������
* ������ readers   �洢��������������
* ������ count �������ӿڸ���
******************************************************/
extern "C" void GetReaders(char** readers, int count);

/****************************************************
* ���ܣ� �ȸ�λ��SCARD_LEAVE_CARD
*****************************************************/
extern "C" void WarmReset();

/****************************************************
* ���ܣ� �临λ��SCARD_UNPOWER_CARD �ϵ縴λ
*****************************************************/
extern "C" void ColdReset();

/***************************************************
****************************************************/
extern "C" bool OpenReader(const char* reader);
extern "C" void CloseReader();
extern "C" void GetATR(char* atr, int len);
extern "C" int  GetCardStatus();
extern "C" int  GetTransProtocol();

