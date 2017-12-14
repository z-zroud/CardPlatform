#include "DpDecrypt.h"
#include "Des0.h"

bool DpDecrypt::Decrypt(const string& key, const string& input, string& output, DECRYPT_TYPE type)
{
	if (type == DES3_ECB)
	{
		int inputLen = input.length();
		if (inputLen % 16 != 0)
		{
			return false;	//�������ݱ�����8�ֽ�������
		}
		char* result = new char[inputLen + 1];
		memset(result, 0, inputLen + 1);
		
        //for (int i = 0; i < inputLen; i += 16)
        //{
        //    char tmp[17] = { 0 };
        //    _Des3(tmp, (char*)key.c_str(), (char*)input.substr(i, 16).c_str());
        //    output += tmp;
        //}
        for (int i = 0; i < inputLen; i += 16)
        {
            char tmp[17] = { 0 };
            Des3(tmp, (char*)key.c_str(), (char*)input.substr(i, 16).c_str());
            output += tmp;
        }
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
		return false;	//�޷����ļ�
	}
	fseek(iFile, 0, SEEK_END);
	long iFileSize = ftell(iFile);
	fseek(iFile, 0, SEEK_SET);
	if (iFileSize % 16 != 0)
	{
		return false;	//���ݲ���8�ֽ�������
	}
	char* buff = new char[iFileSize + 1];
	memset(buff, 0, iFileSize + 1);
	fread_s(buff, iFileSize, iFileSize, 1, iFile);
	fclose(iFile);

	string output;
	if (!Decrypt(key, buff, output, type))
	{
		return false;	//����ʧ��
	}
	string newPath;
	int pos = path.find_last_of('.');
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