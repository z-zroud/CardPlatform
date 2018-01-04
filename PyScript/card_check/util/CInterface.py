# This module define interface witch communicate with C++
# or dynamiclly generated some data
from card_check.util import param
import time
import random
import sys

############### This Function will used by C++ to set param ##################
def GetReaderName(name):
    param.readerName = name


def GetSMMark(bMark):
    if bMark is True:
        param.termParams["DF69"] = "01"
    else:
        param.termParams["DF69"] = "00"

def SetUdkAuthKey(key):
    param.udkAuthKey = key

def GetUdkAuthKey():
    return param.udkAuthKey

def SetUdkMacKey(key):
    param.udkMacKey = key

def GetUdkMacKey():
    return param.udkMacKey

def SetUdkEncKey(key):
    param.udkEncKey = key

def GetUdkEncKey():
    return param.udkEncKey
##############################################################################

def GetTerm9A():
    return time.strftime("%y%m%d",time.localtime(time.time()))

def GetTerm9F37():
    seed = ['0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F']
    snap = random.sample(seed,8)
    return ''.join(snap)

def GetTerm9F21():
    return time.strftime("%H%M%S",time.localtime(time.time()))

def GetTermTag(tag):
    if tag in param.termParams:
        return param.termParams.get(tag)
    else:
        funcName = "GetTerm" + tag
        pFunc = getattr(sys.modules[__name__],funcName)
        return pFunc()


if __name__ == '__main__':
    print(GetTerm9A())
    print(GetTerm9F37())
    print(GetTermTag("9F02"))
    print(GetTermTag("9F37"))
    print(GetTermTag("9F21"))
