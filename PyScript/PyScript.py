import sys
from card_check.pboc.transaction import contactTrans as pbocContactTrans
from card_check.pboc.transaction import pse as pbocPse
from card_check.pboc.transaction import ec as pbocEc
from card_check.pboc.transaction import qPboc
from card_check.util import PCSC
from card_check.util import CInterface
from card_check.util import ApduCmd



def OpenCardReader(readerName):
    print(readerName)
    if PCSC.OpenReader(readerName) is True:
        print(PCSC.GetATR())

def DoPbocContact():
    pbocPse.SelectPSE()
    pbocContactTrans.SelectApp(CInterface.GetAid())
    pbocContactTrans.InitApp()
    pbocContactTrans.ReadRecord()
    pbocContactTrans.OfflineAuth()
    pbocContactTrans.TerminalActionAnalyse()
    pbocContactTrans.IssuerAuthencation()
    pbocContactTrans.EndTransaction()
    pbocContactTrans.HandleIssuerScript()

def DoPbocContactless():
    pbocPse.SelectPPSE()
    pbocContactTrans.SelectApp(CInterface.GetAid())
    pbocContactTrans.InitApp()
    pbocContactTrans.ReadRecord()
    pbocContactTrans.OfflineAuth()
    pbocContactTrans.TerminalActionAnalyse()
    pbocContactTrans.IssuerAuthencation()
    pbocContactTrans.EndTransaction()
    pbocContactTrans.HandleIssuerScript()

def DoPbocEC():
    pbocPse.SelectPSE()
    pbocEc.SelectApp(CInterface.GetAid())
    pbocEc.InitApp()
    pbocEc.ReadRecord()
    pbocEc.OfflineAuth()
    pbocEc.TerminalRiskManagement()
    pbocEc.TerminalActionAnalyse()

def DoPbocQPBOCTrans():
    pbocPse.SelectPPSE()
    qPboc.SelectApp(CInterface.GetAid())
    qPboc.InitApp()
    qPboc.ReadRecord()
    qPboc.OfflineAuth()


if __name__ == '__main__':
    import os
    print(os.getcwd())
    from card_check.util import PCSC
    readers = PCSC.GetReaders()
    for reader in readers:
        print(reader)
    if PCSC.OpenReader(readers[1]) is True:
        print(PCSC.GetATR())
        DoPbocContact()
        #DoPbocContactless()
        #DoPbocQPBOCTrans()

