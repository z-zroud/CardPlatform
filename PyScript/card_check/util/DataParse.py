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


class _TL(Structure):
    _fields_ = [("tag",c_char_p),("len",c_uint)]

class TL:
    def __init__(self):
        self.tag = ''
        self.len = 0

class _AFL(Structure):
    _fields_ = [("sfi",c_int),
                ("recordNumber",c_int),
                ("bSigStaticData",c_bool)
        ]

class AFL:
    def __init__(self):
        self.sfi = 0
        self.recordNumber = 0
        self.bSigStaticData = False

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

class TV:
    def __init__(self,tag,value):
        self.tag = tag
        self.value = value

def ParseTL(buffer,tls):
    bytesBuffer = str.encode(buffer)
    TLArr = _TL * 30
    _tls = TLArr()
    tlsCount = c_int(30)
    DpLib.ParseTL(bytesBuffer,_tls,byref(tlsCount))
    for index in range(tlsCount.value):
        tl = TL()
        tl.tag = bytes.decode(_tls[index].tag)
        tl.len = _tls[index].len
        tls.append(tl)
        print("TL tag=",tl.tag," len=",tl.len)

def ParseAFL(buffer,afls):
    bytesBuffer = str.encode(buffer)
    AFLArr = _AFL * 30
    _afls = AFLArr()
    aflCount = c_int(30)
    DpLib.ParseAFL(bytesBuffer,_afls,byref(aflCount))
    for index in range(aflCount.value):
        afl = AFL()
        afl.bSigStaticData = _afls[index].bSigStaticData
        afl.recordNumber = _afls[index].recordNumber
        afl.sfi = _afls[index].sfi
        afls.append(afl)

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


def SaveTlv(tlvs, tags):
    for tlv in tlvs:
        if tlv.isTemplate is False:
            tags.append(TV(tlv.tag,tlv.value))
            print("Save Tag: ", tlv.tag)

def GetTagValue(tag,tags):
    for item in tags:
        if tag == item.tag:
            return item.value
    return ''


if __name__ == '__main__':
    afls = []
    print(ParseAFL("08010100100103011010100018010400",afls))
    tls = []
    print(ParseTL("9F1F055F2D04",tls))
    for i in range(100):
        tlvs = []
        ParseTLV("6F27840E315041592E5359532E4444463031A5158801015F2D027A68BF0C0A9F4D020B0ADF4D020C0A",tlvs)
        for tlv in tlvs:
            print("tag=",tlv.tag,"len=",tlv.length,"value=",tlv.value,"level=",tlv.level,"isTemplate=",tlv.isTemplate)
    print(FormatTlv(tlvs))

