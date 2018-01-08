

from card_check.util import ApduCmd
from card_check.util import DataParse
from card_check.util import Authencation
from card_check.pboc.cases import CaseSelectApp
from card_check.util import CInterface

tags = []   #store TV key-value pair
sigStaticData = ""
terminalRandomNum = ""  # generated by InitApp and re_used by validate fDDA


def SelectApp(aid):
    #aid = DataParse.GetTagValue("4F",tags)
    tags.append(DataParse.TV("4F",aid))
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
        if tl.tag == "9F37":
            global terminalRandomNum
            terminalRandomNum = CInterface.GetTermTag(tl.tag)
            gpoData += terminalRandomNum
            print(gpoData)
        else:
            gpoData += CInterface.GetTermTag(tl.tag)
    sw,resp = ApduCmd.GPOCmd(gpoData)
    if sw != 0x9000:
        return False
    tlvs = []
    DataParse.ParseTLV(resp,tlvs)
    DataParse.SaveTlv(tlvs,tags)
    #tag82 = resp[4:8]
    #tag94 = resp[8:]
    #tags.append(DataParse.TV("82",tag82))
    #tags.append(DataParse.TV("94",tag94))
    return True

def ReadRecord():
    tag94 = DataParse.GetTagValue("94",tags)
    afls = []
    DataParse.ParseAFL(tag94,afls)
    global sigStaticData
    for afl in afls:
        sw,resp = ApduCmd.ReadRecordCmd(afl.sfi,afl.recordNumber)
        tlvs = []
        DataParse.ParseTLV(resp,tlvs)
        DataParse.SaveTlv(tlvs,tags)
        if afl.bSigStaticData is True:
            sigStaticData += resp[4:]

def OfflineAuth():
    caIndex = DataParse.GetTagValue("8F",tags)
    caPublicKey = Authencation.GenCAPublicKey(caIndex,"A000000333")
    print("CA Public Key=",caPublicKey)
    issuerPublicCert = DataParse.GetTagValue("90",tags)
    ipkRemainder = DataParse.GetTagValue("92",tags)
    issuerExponent = DataParse.GetTagValue("9F32",tags)
    issuerPublicKey = Authencation.GenDesIssuerPublicKey(caPublicKey,issuerPublicCert,ipkRemainder,issuerExponent)
    print("issuer Public Key=",issuerPublicKey)
    tag93 = DataParse.GetTagValue("93",tags)
    tag82 = DataParse.GetTagValue("82",tags)
    print("tag93 = ",tag93)
    print("sig data = ",sigStaticData)
    ret = Authencation.DES_SDA(issuerPublicKey,issuerExponent,tag93,sigStaticData,tag82)
    if ret != 0:
        return False
    iccPublicCert = DataParse.GetTagValue("9F46",tags)
    iccRemainder = DataParse.GetTagValue("9F48",tags)
    iccExponent = DataParse.GetTagValue("9F47",tags)
    iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey,iccPublicCert,iccRemainder,sigStaticData,iccExponent,tag82)
    
    ddolData = terminalRandomNum + CInterface.GetTermTag("9F02") + CInterface.GetTermTag("5F2A") + DataParse.GetTagValue("9F69",tags) # + DataParse.GetTagValue("9F36",tags)
    print(ddolData)
    tag9F4B = DataParse.GetTagValue("9F4B",tags)
    ret = Authencation.DES_DDA(iccPublicKey,iccExponent,tag9F4B,ddolData)
    if ret != 0:
        return False


def TerminalRiskManagement():
    pass


def TerminalActionAnalyse():
    cdol1 = DataParse.GetTagValue("8C",tags)
    tls = []
    DataParse.ParseTL(cdol1,tls)
    cdol1Data = ""
    for tl in tls:
        cdol1Data += CInterface.GetTermTag(tl.tag)
    sw,resp = ApduCmd.GACCmd(ApduCmd.ARQC,cdol1Data)
    tag9F27 = resp[4:6]
    tag9F36 = resp[6:10]
    tag9F26 = resp[10:26]
    tag9F10 = resp[26:]
    tags.append(DataParse.TV("9F27",tag9F27))
    tags.append(DataParse.TV("9F36",tag9F36))
    tags.append(DataParse.TV("9F26",tag9F26))
    tags.append(DataParse.TV("9F10",tag9F10))


