#pragma once

#include "Interface\CardBase.h"

class TLVParaser
{
public:
	void TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PBCD_TL pTlEntity, unsigned int& entityNum);
	void TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PTLV PTlvEntity, unsigned int& entityNum);
	void TLVConstruct(unsigned char* buffer, unsigned int bufferLength, PBCD_TLV pBCDTlvEntity, unsigned int& entityNum);
	bool TLVParseAndFindError(PTLV PTlvEntity, unsigned int entitySize, unsigned char* buffer, unsigned int& bufferLength);

private:
	int ctoi(unsigned char c);
};