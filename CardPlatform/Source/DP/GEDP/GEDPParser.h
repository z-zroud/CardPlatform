#pragma once
#include "../DPParaser.h"

struct DPGeErData
{
	void Clear() { strBussinessType.clear(); vecData.clear(); }

	string strBussinessType;
	vector<TLVItem> vecData;
};

class GeErDPParser : public DPParser
{
public:
	int Read(const string& filePath);

private:
	vector<DPGeErData> m_vecGeErData;
};

