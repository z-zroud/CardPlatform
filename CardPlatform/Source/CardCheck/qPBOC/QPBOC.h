#pragma once
#include "CardCheck\PBOC\PBOC.h"

class QPBOC : public PBOC
{
public:
    QPBOC(IPCSC* pReader);
};