import os
import sys
from ctypes import *

dllName = 'DataParse.dll'
dllPath = os.path.dirname(os.path.abspath(__file__))
#print(dllPath)
dirList = dllPath.split(os.path.sep)
#print(dirList)
dllPath = os.path.sep.join(dirList[0:len(dirList) - 1]) + os.path.sep + "dll" + os.path.sep + dllName

DpLib = CDLL(dllPath)


class TL(Structure):
    _fields_ = [("tag",c_char_p),("len",c_char_p)]

class AFL(Structure):
    _fields_ = [("sfi",c_int),
                ("recordNumber",c_int),
                ("bSigStaticData",c_bool)
        ]

class TLV(Structure):
    pass

TLV._fields_ = [("tag",c_char_p),
                ("length",c_char_p),
                ("value",c_char_p),
                ("tagSize",c_uint),
                ("lenSize",c_uint),
                ("isTemplate",c_bool),
                ("subTLVEntity",POINTER(TLV)),
                ("subTLVnum",c_uint)
        ]

def ParseTL(buffer):
    bytesBuffer = str.encode(buffer)
    #tls = []
    TLArr = TL * 10
    tls = TLArr()
    tlsCount = c_int(10)
    DpLib.ParseTL(bytesBuffer,tls,byref(tlsCount))
    return tlsCount.value

def ParseAFL(buffer):
    bytesBuffer = str.encode(buffer)
    AFLArr = AFL * 20
    afls = AFLArr()
    aflCount = c_int(20)
    DpLib.ParseAFL(bytesBuffer,afls,byref(aflCount))
    return aflCount.value

def ParseTLV(buffer):
    bytesBuffer = str.encode(buffer)
    TLVArr = TLV * 20
    tlvs = TLVArr()
    tlvCount = c_int(20)
    DpLib.ParseTLV(bytesBuffer,tlvs,byref(tlvCount))
    return tlvCount.value

if __name__ == '__main__':
    print(ParseTL("9F1F055F2D04"))
    ParseTLV("6F558408A000000333010101A549500E556E696F6E5061792044656269748701019F380C9F7A019F02065F2A02DF69015F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0A9F4D020B0ADF4D020C0A")

