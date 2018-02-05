from card_check.util import ApduCmd
from card_check.util import DataParse
from card_check.pboc.cases import CaseSelectPse

tags = []

def SelectPSE():
    sw,resp = ApduCmd.SelectAppCmd("315041592E5359532E4444463031")
    if sw != 0x9000:
        return False
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)
    print(DataParse.FormatTlv(tlvs))
    CaseSelectPse.PBOC_sPSE_SJHGX_001(resp)
    CaseSelectPse.PBOC_sPSE_SJHGX_003(tlvs)
    tag88 = DataParse.GetTagValue("88",tags)
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
        DataParse.SaveTlv(tlvs,tags)
        print(DataParse.FormatTlv(tlvs))
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
