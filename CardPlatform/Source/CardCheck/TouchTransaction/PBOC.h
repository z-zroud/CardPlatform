#pragma once
#include "EMV.h"
#include "Util\PCSC.h"

class PBOC : public EMV
{
public:
    PBOC(IPCSC* pPCSC);
};
