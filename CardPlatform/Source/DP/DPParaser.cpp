#include "stdafx.h"
#include "DPParaser.h"
#include <fstream>


int DPParser::Read(const string& filePath)
{
    return -1;
}

void DPParser::Save(const string& filePath)
{
	ofstream outputFile(filePath.c_str());
	if (!outputFile)
		return;
	for (auto iter : m_DpData)
	{
		outputFile << "[" << iter.DGIName << "]" << endl;
		for (auto tlvData : iter.vecItem)
		{
			outputFile << tlvData.strTag << "=" << tlvData.strTag + tlvData.strLen + tlvData.strValue << endl;
		}
	}
	outputFile.close();
	outputFile.clear();
}

INIParser DPParser::GetConfig(const string& filePath)
{
	INIParser iniFile(filePath);

	return iniFile;
}

