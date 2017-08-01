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

IAPDU* GetAPDUInterface(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol);
//IDPParser* GetDPParserInterface(int dpType);
//IEMV* GetEMVInterface(IPCSC* pPCSC);
//IEMV* GetEMVInterfaceEx();
IKeyGenerator* GetKeyGeneratorInterface();
IPCSC* GetPCSCInterface();
