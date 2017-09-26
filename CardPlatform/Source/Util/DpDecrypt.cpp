#include "DpDecrypt.h"
#include "Des0.h"

bool DpDecrypt::Decrypt(const string& key, const string& input, string& output, DECRYPT_TYPE type)
{
	if (type == DES3_ECB)
	{
		int inputLen = input.length();
		if (inputLen % 16 != 0)
		{
			return false;	//加密数据必须是8字节整数倍
		}
		char* result = new char[inputLen + 1];
		memset(result, 0, inputLen + 1);
		
		Des3_ECB(result, (char*)key.c_str(), (char*)input.c_str(), inputLen + 1);
		output = result;

		return true;
	}

	return false;
}

bool DpDecrypt::Decrypt(const string& key, const string& path, DECRYPT_TYPE type)
{
	FILE* iFile = NULL;
	FILE* oFile = NULL;

	fopen_s(&iFile, path.c_str(), "r");
	if (iFile == NULL)
	{
		return false;	//无法打开文件
	}
	fseek(iFile, 0, SEEK_END);
	long iFileSize = ftell(iFile);
	fseek(iFile, 0, SEEK_SET);
	if (iFileSize % 16 != 0)
	{
		return false;	//数据不是8字节整数倍
	}
	char* buff = new char[iFileSize + 1];
	memset(buff, 0, iFileSize + 1);
	fread_s(buff, iFileSize, iFileSize, 1, iFile);
	fclose(iFile);

	string output;
	if (!Decrypt(key, buff, output, type))
	{
		return false;	//解密失败
	}
	string newPath;
	int pos = path.find('.');
	if (pos == string::npos)
	{
		newPath = path + "_decrypt";
	}
	else {
		newPath = path.substr(0, pos) + "_decrypt" + path.substr(pos);
	}
	fopen_s(&oFile, newPath.c_str(), "w+");
	if (oFile)
	{
		fwrite(output.c_str(), output.length(), 1, oFile);
		fclose(oFile);
		return true;
	}

	return false;
}