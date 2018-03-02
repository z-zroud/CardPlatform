from enum import Enum
from card_check.util.DataParse import TL,TLV,AFL,ParseTLV

sigStaticData = ""
transAid = "A000000333010101"   #交易时使用的应用AID，由界面传入

def SaveTlv(tlvs, tags):
    for tlv in tlvs:
        if tlv.isTemplate is False:
            tags.append(TV(tlv.tag,tlv.value))
            print("Tag: ", tlv.tag, "Value:",tlv.value)

def GetTags(tlvs):
    tags = []
    for tlv in tlvs:
        if tlv.isTemplate is False:
            tags.append(TV(tlv.tag,tlv.value))
    return tags


def GetTagFromTlv(tag,tlvs):
    ret = TLV()
    for tlv in tlvs:
        if tlv.tag == tag:
            return tlv
    return ret

def GetTagValue(tag,tags):
    for item in tags:
        if tag == item.tag:
            return item.value
    return ''

def HasDuplicateTag(tag,tlvs):
    if tlvs.count(tag) > 1:
        return True
    return False

#交易阶段
class TransStep(Enum):
    STEP_SELECT_PSE = 0
    STEP_SELECT_PSE_DIR = 1
    STEP_SELECT_AID = 2
    STEP_GPO = 3
    STEP_READ_RECORD = 4
    STEP_OFFLINE_AUTH = 5
    STEP_GAC_FIRST = 6
    STEP_GAC_SECOND = 7
    STEP_ISSUER_AUTH = 8


#存储每个交易阶段的Tag值
TransTags = {TransStep.STEP_SELECT_PSE:{},
             TransStep.STEP_SELECT_PSE_DIR:{},
             TransStep.STEP_SELECT_AID:{},
             TransStep.STEP_GPO:{},
             TransStep.STEP_READ_RECORD:{},
             TransStep.STEP_OFFLINE_AUTH:{},
             TransStep.STEP_GAC_FIRST:{},
             TransStep.STEP_GAC_SECOND:{},
             TransStep.STEP_ISSUER_AUTH:{}
    }

def SetTransTags(transStep,tlvs):
    for tlv in tlvs:
        TransTags[transStep][tlv.tag] = tlv

def GetTransTags(transStep,tag):
    return TransTags[transStep][tag]

def ParseAndSaveResp(resp,transStep):
    tlvs = []
    if ParseTLV(resp,tlvs) is False:
        return False
    SetTransTags(transStep,tlvs)
    return True

def ParseGPOAndSave(resp):
    if len(resp) < 4:
        return False
    gpoTlvs = []
    if ParseTLV(resp,gpoTlvs) is False:
        return False
    if len(gpoTlvs) != 1:
        return False
    tag80 = TLV()
    tag80.isTemplate = True
    tag80.level = 0
    tag80.tag = "80"
    tag82 = TLV()
    tag82.isTemplate = False
    tag82.length = 2
    tag82.level = 1
    tag82.tag = "82"
    tag82.value = gpoTlvs[0].value[0:4]
    tag94 = TLV()
    tag94.isTemplate = False
    tag94.level = 1
    tag94.tag = "94"
    tag94.value = gpoTlvs[0].value[4:len(gpoTlvs[0].value)]
    tag94.length = len(tag94.value) // 2
    tlvs = [tag80,tag82,tag94]
    SetTransTags(TransStep.STEP_GPO,tlvs)
    return True



if __name__ == '__main__':
    tlv = TLV()
    tlv.tag = "4F"
    tlv.length=2
    tlvs=[]
    tlvs.append(tlv)
    SetTransTags(TransStep.STEP_ISSUER_AUTH,tlvs)
    ret = GetTransTags(TransStep.STEP_ISSUER_AUTH,"4F")