#include "Interface\InterfaceInstance.h"
#include "Util\APDU.h"
#include "Util\PCSC.h"
#include "Util\KeyGenerator.h"

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