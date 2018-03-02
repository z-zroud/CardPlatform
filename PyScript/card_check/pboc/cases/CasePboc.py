import sys
from card_check.util import DataParse
from card_check.pboc.cases import CaseGPO
from card_check.pboc.cases import CaseSelectPse
from card_check.pboc.cases import CaseSelectAid
from card_check.util.Tool import SW,FormatTLV
from card_check.util.Tool import S


def LogCheck(checkFunc,cmd,sw,resp):
    names = checkFunc.split('.')
    moduleName = "card_check.pboc.cases." + names[0]
    funcName = names[1]
    pFunc = getattr(sys.modules[moduleName],funcName)
    tlvs = []
    DataParse.ParseTLV(resp,tlvs)
    strFormat = FormatTLV(tlvs)
    checkRet = pFunc(resp,tlvs)
    info = "Check Point: " + moduleName + "." + funcName + "\n" + \
        "I: " + cmd + "\n" + \
        "O: " + resp + "\n" + \
        strFormat + "\n" + \
        "S: " + SW(sw) + "\n" + \
        "Result: " + str(checkRet)
    print(info)



if __name__ == '__main__':
    hexStr = '0x{0:04X}'.format(0x6985)
    cmd = "00A40000"
    print(info)
    print(hexStr)