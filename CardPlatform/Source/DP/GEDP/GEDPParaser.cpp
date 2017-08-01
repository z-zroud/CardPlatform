#include "stdafx.h"
#include "GEDPParser.h"
#include "Util\StringParaser.h"
#include "Util\Log.h"
#include <fstream>


int GeErDPParser::Read(const string& filePath)
{
	ifstream dpFile(filePath.c_str());
	if (!dpFile)
		return -1;
	string strLine;
	DPGeErData dpData;
	while (getline(dpFile, strLine))
	{
		string::size_type leftPos = 0;
		string::size_type rightPos = 0;

		//是否为root
		if ((strLine.npos != (leftPos = strLine.find("["))) &&
			(strLine.npos != (rightPos = strLine.find("]"))) &&
			leftPos == 0)
		{
			if (!dpData.strBussinessType.empty())
			{
				m_vecGeErData.push_back(dpData);
			}
			dpData.Clear();
			dpData.strBussinessType = strLine.substr(leftPos + 1, rightPos - 1);
		}
		else {
			vector<string> splitItems;
			Tool::Stringparser::SplitString(strLine, splitItems, "|");
			if (splitItems.size() != 3)
			{
				Log->Error("格尔DP数据[%s]中TLV格式不正确", strLine);
				return -1;
			}
			int nLen = atoi(splitItems[1].c_str());
			char temp[5] = { 0 };
			_itoa_s(nLen, temp, 16); 
			string strLen = Tool::Converter::StrUpper(temp);
			TLVItem tlvItem(splitItems[0], strLen, splitItems[2]);
			dpData.vecData.push_back(tlvItem);
		}
	}
	//插入最后一条数据
	m_vecGeErData.push_back(dpData);

	return 0;
}

