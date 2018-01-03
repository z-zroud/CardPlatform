# This module define interface witch communicate with C++
# or dynamiclly generated some data
from . import param
import time
import random

def GetReaderName(name):
    param.readerName = name

def GetTerm95():
    return time.strftime("%y%m%d",time.localtime(time.time()))

def GetTerm9F37():
    seed = ['0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F']
    snap = random.sample(seed,8)
    return ''.join(snap)


if __name__ == '__main__':
    print(GetTerm95())
    print(GetTerm9F37())
