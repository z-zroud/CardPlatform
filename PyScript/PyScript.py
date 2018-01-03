import sys
from card_check.pboc.transaction import contactTrans
from card_check.util import PCSC
from card_check.util import param



def OpenCardReader(readerName):
    print(readerName)
    if PCSC.OpenReader(readerName) is True:
        print(PCSC.GetATR())

def DoPbocContactless():
    contactTrans.SelectPPSE()





if __name__ == '__main__':
    from card_check.util import PCSC
    readers = PCSC.GetReaders()
    for reader in readers:
        print(reader)
    if PCSC.OpenReader(readers[0]) is True:
        print(PCSC.GetATR())
        DoPbocContactless()

