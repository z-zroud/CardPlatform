from card_check.util import ApduCmd
from card_check.util import DataParse
from card_check.util import Authencation
from card_check.util import CInterface
from card_check.util import TransInfo
from card_check.util.TransInfo import TransStep,GetTransTags
from card_check.pboc.cases.CasePboc import LogCheck
from card_check.util.PCSC import GetLastApduCmd

def SelectApp(aid):    
    sw,resp = ApduCmd.SelectAppCmd(aid)
    if sw != 0x9000:
        return False
    cmd = GetLastApduCmd()
    TransInfo.ParseAndSaveResp(resp,TransStep.STEP_SELECT_AID)
    LogCheck("CaseSelectAid",cmd,sw,resp)
    return True

def InitApp():
    tag9F38 = TransInfo.GetTransTags(TransStep.STEP_SELECT_AID,"9F38")
    tls = []
    DataParse.ParseTL(tag9F38,tls)
    gpoData = ""
    for tl in tls:
        gpoData += CInterface.GetTermTag(tl.tag)
    sw,resp = ApduCmd.GPOCmd(gpoData)
    if sw != 0x9000:
        return False
    cmd = GetLastApduCmd()
    TransInfo.ParseGPOAndSave(resp)
    LogCheck("CaseGPO",cmd,sw,resp)
    return True

def ReadRecord():
    tag94 = TransInfo.GetTransTags(TransStep.STEP_GPO,"94")
    afls = []
    DataParse.ParseAFL(tag94,afls)
    for afl in afls:
        sw,resp = ApduCmd.ReadRecordCmd(afl.sfi,afl.recordNumber)
        TransInfo.ParseAndSaveResp(resp,TransStep.STEP_READ_RECORD)
        if afl.bSigStaticData is True:
            TransInfo.sigStaticData += resp[6:]

def OfflineAuth():
    caIndex = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"8F")
    caPublicKey = Authencation.GenCAPublicKey(caIndex,"A000000333")
    print("CA Public Key=",caPublicKey)
    issuerPublicCert = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"90")
    ipkRemainder = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"92")
    issuerExponent = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F32")
    issuerPublicKey = Authencation.GenDesIssuerPublicKey(caPublicKey,issuerPublicCert,ipkRemainder,issuerExponent)
    print("issuer Public Key=",issuerPublicKey)
    tag93 = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"93")
    tag82 = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"82")
    print("tag93 = ",tag93)
    print("sig data = ",sigStaticData)
    ret = Authencation.DES_SDA(issuerPublicKey,issuerExponent,tag93,sigStaticData,tag82)
    if ret != 0:
        return False
    iccPublicCert = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F46")
    iccRemainder = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F48")
    iccExponent = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F47")
    iccPublicKey = Authencation.GenDesICCPublicKey(issuerPublicKey,iccPublicCert,iccRemainder,sigStaticData,iccExponent,tag82)
    ddol = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F49")
    tls = []
    DataParse.ParseTL(ddol,tls)
    ddolData = ""
    for tl in tls:
        ddolData += CInterface.GetTermTag(tl.tag)
    tag9F4B = ApduCmd.GenDynamicData(ddolData)
    ret = Authencation.DES_DDA(iccPublicKey,iccExponent,tag9F4B,ddolData)
    if ret != 0:
        return False


def TerminalActionAnalyse():
    cdol1 = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"8C")
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

def IssuerAuthencation():
    atc = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F36")
    udkSessionKey = Authencation.GenUdkSessionKey(CInterface.GetUdkAuthKey(),atc)
    authCode = "3030"
    ac = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F26")
    arpc = Authencation.GenArpc(udkSessionKey,ac,authCode)
    sw,resp = ApduCmd.ExternalAuthencationCmd(arpc,authCode)
    if sw != 0x9000:
        return False

def EndTransaction():
    cdol2 = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"8D")
    tls = []
    DataParse.ParseTL(cdol2,tls)
    cdol2Data = ""
    for tl in tls:
        cdol2Data += CInterface.GetTermTag(tl.tag)
    sw,resp = ApduCmd.GACCmd(ApduCmd.TC,cdol2Data)
    tag9F27 = resp[4:6]
    tag9F36 = resp[6:10]
    tag9F10 = resp[26:]
    tags.append(DataParse.TV("9F27",tag9F27))
    tags.append(DataParse.TV("9F36",tag9F36))
    tags.append(DataParse.TV("9F10",tag9F10))

def HandleIssuerScript():
    atc = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F36")
    ac = TransInfo.GetTransTags(TransStep.STEP_READ_RECORD,"9F26")
    udkMacSessionKey = Authencation.GenUdkSessionKey(CInterface.GetUdkMacKey(),atc)
    scriptData = "04DA9F790A" + atc + ac + "000000050000"
    mac = Authencation.GenIssuerScriptMac(udkMacSessionKey,scriptData)
    sw,resp = ApduCmd.PutDataCmd("9F79","000000050000",mac)
    if sw != 0x9000:
        return False

