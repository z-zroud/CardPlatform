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

class TLVEntity(Structure):
    pass

TLVEntity._fields_ = [("tag",c_char_p),
                ("length",c_char_p),
                ("value",c_char_p),
                ("tagSize",c_uint),
                ("lenSize",c_uint),
                ("isTemplate",c_bool),
                ("subTLVEntity",POINTER(TLVEntity)),
                ("subTLVnum",c_uint)
        ]

class _TLV(Structure):
    _fields_ = [("isTemplate",c_bool),
                ("level",c_int),
                ("tag",c_char_p),
                ("length",c_uint),
                ("value",c_char_p)
        ]

class TLV:
    def __init__(self):
        self.isTemplate = False
        self.level = 0
        self.tag = ''
        self.length = 0
        self.value = ''

def ParseTL(buffer):
    bytesBuffer = str.encode(buffer)
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

def ParseTLV(buffer, tlvList):
    bytesBuffer = str.encode(buffer)
    TLVArr = _TLV * 30
    tlvs = TLVArr()
    tlvCount = c_uint(30)
    DpLib.ParseTLV.restype = c_bool
    ret = DpLib.ParseTLV(bytesBuffer,tlvs,byref(tlvCount))
    if ret is False:
        return ret
    for index in range(tlvCount.value):
        _tlv = TLV()
        _tlv.isTemplate = tlvs[index].isTemplate
        _tlv.length = tlvs[index].length
        _tlv.level = tlvs[index].level
        _tlv.tag = bytes.decode(tlvs[index].tag)
        _tlv.value = bytes.decode(tlvs[index].value)
        tlvList.append(_tlv)
    return ret

def FormatTlv(tlvs):
    formatStr = ""
    for tlv in tlvs:
        formatStr += '    ' * tlv.level
        if tlv.isTemplate is True:
            formatStr += "<" + tlv.tag + ">\n"
        else:
            formatStr += "[" + tlv.tag + "]=" + tlv.value + '\n'
    return formatStr


if __name__ == '__main__':
    print(ParseTL("9F1F055F2D04"))
    tlvs = []
    ParseTLV("6F27840E315041592E5359532E4444463031A5158801015F2D027A68BF0C0A9F4D020B0ADF4D020C0A",tlvs)
    for tlv in tlvs:
        print("tag=",tlv.tag,"len=",tlv.length,"value=",tlv.value,"level=",tlv.level,"isTemplate=",tlv.isTemplate)
    print(FormatTlv(tlvs))

