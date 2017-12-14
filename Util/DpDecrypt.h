#pragma once
#include <string>
#include "Interface\CardBase.h"
using namespace std;

/**********************************************************
* ���ݽ������ͣ�������Կ���ͽ��н���
***********************************************************/
class DpDecrypt
{
public:
	static bool Decrypt(const string& key, const string& input, string& output, DECRYPT_TYPE type);
	static bool Decrypt(const string& key, const string& path, DECRYPT_TYPE type);
};
