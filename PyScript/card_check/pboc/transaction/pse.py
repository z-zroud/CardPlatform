from card_check.util import ApduCmd
from card_check.util.PCSC import GetLastApduCmd
from card_check.util import DataParse
from card_check.util import TransInfo
from card_check.pboc.cases import CaseSelectPse
from card_check.pboc.cases.CasePboc import LogCheck

tags = []

def SelectPSE():
    sw,resp = ApduCmd.SelectAppCmd("315041592E5359532E4444463031")
    if sw != 0x9000:
        return False
    cmd = GetLastApduCmd()
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    #TransInfo.SaveTlv(tlvs,tags)
    #print(TransInfo.FormatTlv(tlvs))
    LogCheck("CaseSelectPse.PBOC_sPSE_SJHGX_001",cmd,sw,resp)
    CaseSelectPse.PBOC_sPSE_SJHGX_001(resp)
    CaseSelectPse.PBOC_sPSE_SJHGX_003(tlvs)
    tag88 = TransInfo.GetTagValue("88",tags)
    sfi = int(tag88,base=16)
    recordNumber = 1
    while True:
        sw,resp = ApduCmd.ReadRecordCmd(sfi,recordNumber)
        if sw == 0x6A83:
            break
        recordNumber += 1
        tlvs = []
        if DataParse.ParseTLV(resp,tlvs) is False:
            print("TLV format is not correct!")
            return False
        TransInfo.SaveTlv(tlvs,tags)
        print(TransInfo.FormatTlv(tlvs))
        CaseSelectPse.PBOC_sPSE_SJHGX_001(resp)
        CaseSelectPse.PBOC_sPSE_SJHGX_003(tlvs)

def SelectPPSE():
    sw,resp = ApduCmd.SelectAppCmd("325041592E5359532E4444463031")
    if sw != 0x9000:
        return
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)
    CaseSelectPse.PBOC_sPSE_SJHGX_001(resp)
    CaseSelectPse.PBOC_sPSE_SJHGX_003(tlvs)
