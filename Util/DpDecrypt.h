#pragma once
#include <string>
#include "Interface\CardBase.h"
using namespace std;

/**********************************************************
* 数据解密类型，根据密钥类型进行解密
***********************************************************/
class DpDecrypt
{
public:
	static bool Decrypt(const string& key, const string& input, string& output, DECRYPT_TYPE type);
	static bool Decrypt(const string& key, const string& path, DECRYPT_TYPE type);
};
