from card_check.util import ApduCmd
from card_check.util.PCSC import GetLastApduCmd
from card_check.util import DataParse
from card_check.util import TransInfo
from card_check.util.TransInfo import TransStep
from card_check.pboc.cases import CaseSelectPse
from card_check.pboc.cases.CasePboc import LogCheck

def SelectPSE():
    sw,resp = ApduCmd.SelectAppCmd("315041592E5359532E4444463031")
    if sw != 0x9000:
        return False
    cmd = GetLastApduCmd()
    TransInfo.ParseAndSaveResp(resp,TransStep.STEP_SELECT_PSE)
    LogCheck("CaseSelectPse",cmd,sw,resp)
    tag88 = TransInfo.GetTransTags(TransStep.STEP_SELECT_PSE,"88")
    sfi = int(tag88,base=16)
    recordNumber = 1
    while True:
        sw,resp = ApduCmd.ReadRecordCmd(sfi,recordNumber)
        if sw == 0x6A83:
            break
        cmd = GetLastApduCmd()
        TransInfo.ParseAndSaveResp(resp,TransStep.STEP_SELECT_PSE_DIR) #默认只有一个应用，暂不实现多个应用
        LogCheck("CaseSelectPseDir",cmd,sw,resp)
        recordNumber += 1
        

def SelectPPSE():
    sw,resp = ApduCmd.SelectAppCmd("325041592E5359532E4444463031")
    if sw != 0x9000:
        return
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)
    #CaseSelectPse.PBOC_sPSE_SJHGX_001(resp)
    #CaseSelectPse.PBOC_sPSE_SJHGX_003(tlvs)
