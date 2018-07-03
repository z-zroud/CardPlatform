#pragma once

#ifdef READCARDNO_EXPORTS
#define READCARDNO_API __declspec(dllexport)
#else
#define READCARDNO_API __declspec(dllimport)
#endif



/**********************************************************
* ���ܣ���ȡ������Ϣ
* ������aid	Ӧ��ʵ������ǿ�:A000000333010101 ���ǿ�:A000000333010102
sfi ���ļ���ʶ�� ָ�����������ļ��� һ��д 2
recordNo ��¼�ţ�ָ���������ڼ�¼��һ���� 1
cardNo �洢�ӿڷ��صĿ�Ƭ��Ϣ
cardNoLen cardNo����
* ���أ�0��ʾ�ɹ�����0��ʾʧ��
***********************************************************/
extern "C" READCARDNO_API int ReadCardNo(const char* aid, int sfi, int recordNo, char* cardNo, int cardNoLen);

/**********************************************************
* ���ܣ���ȡ�������б�
* ������readers �洢�ӿڷ��صĶ������б�
*		count ���յĶ���������
* ���أ�0��ʾ�ɹ�����0��ʾʧ��
***********************************************************/
extern "C" READCARDNO_API int GetReadersEx(char** readers, int& count);

/**********************************************************
* ���ܣ��򿪶�����
* ������readerName ����������
* ���أ�0��ʾ�ɹ�����0��ʾʧ��
***********************************************************/
extern "C" READCARDNO_API int OpenReaderEx(char* readerName);