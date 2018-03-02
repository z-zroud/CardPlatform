from card_check.util.TransInfo import GetTagFromTlv
from card_check.util.TransInfo import GetTags
from card_check.util import ReturnRet
from card_check.util import Tool

#扩展，如果必要，可以将详细的错误判断信息一并返回，这里暂
#不做具体判断。只返回检查点是否成功

def PBOC_sPSE_SJHGX_001(resp,tlvs):
    if len(resp) < 2:
        return ReturnRet.S_ERROR
    if resp[0:2] != "6F":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_003(resp,tlvs):
    subTag = 0
    for tlv in tlvs:
        if tlv.level == 1:
            subTag += 1
    if subTag != 2:
        return ReturnRet.S_ERROR
    if tlvs[1].tag != "84":
        return ReturnRet.S_ERROR
    if tlvs[2].tag != "A5":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_004(resp,tlvs):
    return PBOC_sPSE_SJHGX_003(resp,tlvs)

def PBOC_sPSE_SJHGX_005(resp,tlvs):
    tag84 = GetTagFromTlv("84",tlvs)
    if tag84.value != "315041592E5359532E4444463031":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_006(resp,tlvs):
    for tlv in tlvs:
        if tlv.level == 2:
            if tlv.tag not in ("88","5F2D","9F11","BF0C"):
                return ReturnRet.S_ERROR
    tag88 = GetTagFromTlv("88",tlvs)
    if tag88.level != 2:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_007(resp,tlvs):
    tag88 = GetTagFromTlv("88",tlvs)
    if tag88.value != "01":
        return ReturnRet.S_WARN
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_009(resp,tlvs):
    tag5F2D = GetTagFromTlv("5F2D",tlvs)
    if tag5F2D.length % 2 != 0:
        return ReturnRet.S_ERROR
    if tag5F2D.length < 4 or tag5F2D.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_010(resp,tlvs):
    tag5F2D = GetTagFromTlv("5F2D",tlvs)
    asc = Tool.BcdToAsc(tag5F2D)
    if asc.isalpha() is False:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_011(resp,tlvs):
    tag9F11 = GetTagFromTlv("9F11",tlvs)
    if tag9F11.length != 2:
        return ReturnRet.S_ERROR
    value = int(tag9F11.value,16)
    if value > 0x10 or value < 0x01:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSE_SJHGX_012(resp,tlvs):
    tags = GetTags(tlvs)
    for tag in tags:
        if len(tag.value) == 0:
            return ReturnRet.S_WARN
    return ReturnRet.S_OK

def PBOC_sPSE_ZQX_002_01(resp,tlvs):
    tag9F4D = GetTagFromTlv("9F4D",tlvs)
    if tag9F4D != "0B0A":
        return ReturnRet.S_WARN
    return ReturnRet.S_OK

def PBOC_sPSE_ZQX_002_02(resp,tlvs):
    tagDF4D = GetTagFromTlv("DF4D",tlvs)
    if tag9F4D != "0C0A":
        return ReturnRet.S_WARN
    return ReturnRet.S_OK

def PBOC_sPSE_ZQX_002_04(resp,tlvs):
    tag5F2D = GetTagFromTlv("5F2D",tlvs)
    if tag5F2D != "7A68":
        return ReturnRet.S_WARN
    return ReturnRet.S_OK

def PBOC_sPSE_ZQX_002_05(resp,tlvs):
    tag9F11 = GetTagFromTlv("9F11",tlvs)
    if tag9F11 != "01":
        return ReturnRet.S_WARN
    return ReturnRet.S_OK


def PBOC_sPSE_CFX_001(resp,tlvs):
    tags = []
    for tlv in tlvs:
        tags.append(tlv.tag)
    if tags.count("A5") > 1:
        return ReturnRet.S_ERROR
    if tags.count("6F") > 1:
        return ReturnRet.S_ERROR
    if tags.count("BF0C") > 1:
        return ReturnRet.S_ERROR
    if tags.count("9F11") > 1:
        return ReturnRet.S_ERROR
    if tags.count("5F2D") > 1:
        return ReturnRet.S_ERROR
    if tags.count("88") > 1:
        return ReturnRet.S_ERROR
    if tags.count("9F4D") > 1:
        return ReturnRet.S_ERROR
    if tags.count("DF4D") > 1:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_001(resp,tlvs):
    if tlvs[0].value != "70":
        return ReturnRet.S_ERROR
    return True

def PBOC_sPSEDIR_SJHGX_002(resp,tlvs):
    pass

def PBOC_sPSEDIR_SJHGX_003(resp,tlvs):
    template61 = GetTagFromTlv("61",tlvs)
    if template61.level != 1:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_004(resp,tlvs):
    pass

def PBOC_sPSEDIR_SJHGX_005(resp,tlvs):
    tag4F = GetTagFromTlv("4F",tlvs)
    if tag4F.level != 2 or len(tag4F.value) == 0:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_006(resp,tlvs):
    tag4F = GetTagFromTlv("4F",tlvs)
    if tag4F.length < 5 or tag4F.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_007(resp,tlvs):
    tag50 = GetTagFromTlv("50",tlvs)
    if tag50.length < 1 or tag50.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_008(resp,tlvs):
    for tlv in tlvs:
        if tlv.isTemplate is False:
            if tlv.tag not in ("9F12","87","73"):
                return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_009(resp,tlvs):
    tag9F12 = GetTagFromTlv("9F12",tlvs)
    if tag9F12.length < 1 or tag9F12.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_010(resp,tlvs):
    tag87 = GetTagFromTlv("87",tlvs)
    if tag87.length != 1:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sPSEDIR_SJHGX_011(resp,tlvs):
    pass

def PBOC_sPSEDIR_SJHGX_012(resp,tlvs):
    pass

def PBOC_sPSEDIR_SJHGX_013(resp,tlvs):
    pass



                