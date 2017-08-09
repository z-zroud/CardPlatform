#pragma once
#include <winscard.h>
#include "IAPDU.h"
#include "IDPParser.h"
#include "ICommTransaction.h"
#include "IKeyGenerator.h"
#include "IPCSC.h"
#include "Util\Log.h"



#define DP_YL  0
#define DP_GEER     1
#define DP_HUATENG  2

IAPDU*              GetAPDUInterface(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol);
ICommTransaction*   GetTransInterface(TRANS_TYPE type, IPCSC* pReader, COMMUNICATION_TYPE commType = COMM_TOUCH);
IKeyGenerator*      GetKeyGeneratorInterface();
IPCSC*              GetPCSCInterface();
IPCSC*              GetPCSCInterface(const char* szReaderName);
