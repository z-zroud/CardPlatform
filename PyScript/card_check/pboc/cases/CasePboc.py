import sys
from card_check.util import DataParse
from card_check.pboc.cases import CaseGPO
from card_check.pboc.cases import CaseSelectPse
from card_check.pboc.cases import CaseSelectAid


def LogCheck(checkFunc,cmd,sw,resp):
    names = checkFunc.split('.')
    moduleName = "card_check.pboc.cases." + names[0]
    funcName = names[1]
    pFunc = getattr(sys.modules[moduleName],funcName)
    tlvs = []
    DataParse.ParseTLV(resp,tlvs)
    checkRet = pFunc(resp,tlvs)
    info = "I: " + cmd + "\n" + "O: " + resp + "\n" + "S: " + str(sw)
    print(info)