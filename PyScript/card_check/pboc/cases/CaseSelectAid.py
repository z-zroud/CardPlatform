from card_check.util import ReturnRet
from card_check.util.TransInfo import GetTagFromTlv

def PBOC_sAID_SJHGX_001(tlvs):
    if len(tlvs) < 1:
        return ReturnRet.S_ERROR
    if tlvs[0].tag != "6F":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_002(tlvs):
    pass

def PBOC_sAID_SJHGX_003(tlvs):
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

def PBOC_sAID_SJHGX_004(tlvs):
    tag84 = GetTagFromTlv("84",tlvs)
    tagA5 = GetTagFromTlv("A5",tlvs)
    if tag84.length == 0 or tagA5.length == 0:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_005(tlvs):
    tag84 = GetTagFromTlv("84",tlvs)
    if tag84.length < 5 or tag84.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_006(tlvs):
    tag50 = GetTagFromTlv("50",tlvs)
    if tag50.length == 0:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_007(tlvs):
    for tlv in tlvs:
        if tlv.tag not in ("87","9F38","5F2D","9F11","9F12","BF0C"):
            return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_008(tlvs):
    tag87 = GetTagFromTlv(tlvs)
    if tag87.length != 1:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_009(tlvs):
    pass

def PBOC_sAID_SJHGX_010(tlvs):
    tag5F2D = GetTagFromTlv("5F2D",tlvs)
    if tag5F2D.length % 2 != 0:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_011(tlvs):
    tag5F2D = GetTagFromTlv("5F2D",tlvs)
    asc = Tool.BcdToAsc(tag5F2D)
    if asc.isalpha() is False:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_012(tlvs):
    tag9F11 = GetTagFromTlv("9F11",tlvs)
    if tag9F11.length != 2:
        return ReturnRet.S_ERROR
    value = int(tag9F11.value,16)
    if value > 0x10 or value < 0x01:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_sAID_SJHGX_013(tlvs):
    tag9F12 = GetTagFromTlv("9F12",tlvs)
    if tag9F12.length < 1 or tag9F12.length > 16:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK


def PBOC_sAID_SJHGX_014(tlvs):
    pass

def PBOC_sAID_SJHGX_015(tlvs):
    tag9F4D = GetTagFromTlv("9F4D",tlvs)
    tagDF4D = GetTagFromTlv("DF4D",tlvs)
    if tag9F4D.length != 2 or tagDF4D.length != 2:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

