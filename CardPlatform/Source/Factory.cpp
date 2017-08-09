#include "Interface\InterfaceInstance.h"
#include "Util\APDU.h"
#include "Util\PCSC.h"
#include "Util\KeyGenerator.h"
#include "CardCheck\PBOC\PBOC.h"
#include "CardCheck\EC\EC.h"
#include "CardCheck\qPBOC\QPBOC.h"

/******************************************************
* 外界通过查看InterfaceInstance.h头文件查看可以调用的接口
*******************************************************/

IAPDU* GetAPDUInterface(SCARDHANDLE scardHandle, CARD_TRANSMISSION_PROTOCOL protocol)
{
	return new APDU(scardHandle, protocol);
}

IKeyGenerator* GetKeyGeneratorInterface()
{
	return new KeyGenerator;
}

IPCSC* GetPCSCInterface()
{
	return new PCSC;
}

IPCSC* GetPCSCInterface(const char* szReaderName)
{
    IPCSC* pPCSC = new PCSC;
    if (pPCSC->OpenReader(szReaderName))
    {
        return pPCSC;
    }

    delete pPCSC;
    return NULL;
}

ICommTransaction* GetTransInterface(TRANS_TYPE type, IPCSC* pReader, COMMUNICATION_TYPE commType)
{
    switch (type)
    {
    case TRANS_PBOC:    return new PBOC(commType, pReader);
    case TRANS_QPBOC:   return new QPBOC(pReader);
    case TRANS_EC:      return new EC(pReader);
        
    }

    return NULL;
}