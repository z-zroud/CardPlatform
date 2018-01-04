import sys
from card_check.pboc.transaction import contactTrans
from card_check.util import PCSC
from card_check.util import param



def OpenCardReader(readerName):
    print(readerName)
    if PCSC.OpenReader(readerName) is True:
        print(PCSC.GetATR())

def DoPbocContactless():
    contactTrans.SelectPSE()
    contactTrans.SelectApp()
    contactTrans.InitApp()
    contactTrans.ReadRecord()
    contactTrans.OfflineAuth()
    contactTrans.TerminalActionAnalyse()
    contactTrans.IssuerAuthencation()
    contactTrans.EndTransaction()
    contactTrans.HandleIssuerScript()





if __name__ == '__main__':
    import os
    print(os.getcwd())
    from card_check.util import PCSC
    readers = PCSC.GetReaders()
    for reader in readers:
        print(reader)
    if PCSC.OpenReader(readers[1]) is True:
        print(PCSC.GetATR())
        DoPbocContactless()

