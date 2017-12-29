import param
import sys
print(sys.path)
print(param.readerName)
from card_check.pboc.transaction import contactTrans
print(param.readerName)
from card_check.util import PCSC
print(param.readerName)


def OpenCardReader(readerName):
    print(readerName)
    if PCSC.OpenReader(readerName) is True:
        print(PCSC.GetATR())

def DoPbocContact():
    contactTrans.SelectPSE()





if __name__ == '__main__':
    from card_check.util import PCSC
    if OpenCardReader() is True:
        print(PCSC.GetATR())
