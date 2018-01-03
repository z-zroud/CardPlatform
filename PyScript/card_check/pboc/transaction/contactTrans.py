from card_check.util import ApduCmd
from card_check.util import DataParse
from card_check.util import Authencation
from card_check.pboc.cases import CaseSelectApp
from card_check import param

tags = []   #store TV key-value pair

def SelectPSE():
    sw,resp = ApduCmd.SelectAppCmd("315041592E5359532E4444463031")
    print("SW=",sw,"resp=",resp)
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)
    CaseSelectApp.PBOC_sPSE_SJHGX_001(resp)
    CaseSelectApp.PBOC_sPSE_SJHGX_003(tlvs)

def SelectPPSE():
    sw,resp = ApduCmd.SelectAppCmd("325041592E5359532E4444463031")
    print("SW=",sw,"resp=",resp)
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)
    CaseSelectApp.PBOC_sPSE_SJHGX_001(resp)
    CaseSelectApp.PBOC_sPSE_SJHGX_003(tlvs)

def SelectApp(aid):
    sw,resp = ApduCmd.SelectAppCmd(aid)
    tlvs = []
    if DataParse.ParseTLV(resp,tlvs) is False:
        print("TLV format is not correct!")
        return False
    DataParse.SaveTlv(tlvs,tags)

def InitApp():
    tag9F38 = DataParse.GetTagValue("9F38",tags)
    tls = []
    DataParse.ParseTL(tag9F38,tls)
    gpoData = ""
    for tl in tls:
        gpoData += tl.tag
    sw,resp = ApduCmd.GPOCmd(gpoData)



