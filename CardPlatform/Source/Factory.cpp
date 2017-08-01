#include "Interface\InterfaceInstance.h"
#include "Util\APDU.h"
#include "Util\PCSC.h"
#include "Util\KeyGenerator.h"

/******************************************************
* ���ͨ���鿴InterfaceInstance.hͷ�ļ��鿴���Ե��õĽӿ�
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