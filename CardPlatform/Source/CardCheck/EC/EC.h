#pragma once
#include "CardCheck\PBOC\PBOC.h"

class EC : public PBOC
{
public:
    EC(IPCSC* pReader);
};
