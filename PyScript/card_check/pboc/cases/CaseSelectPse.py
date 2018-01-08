
def PBOC_sPSE_SJHGX_001(resp):
    if resp[0:2] != "6F":
        return False
    return True

def PBOC_sPSE_SJHGX_003(tlvs):
    subTag = 0
    for tlv in tlvs:
        if tlv.level == 1:
            subTag += 1
    if subTag != 2:
        return False
    if tlvs[1].tag != "84":
        return False
    if tlvs[2].tag != "A5":
        return False
    return True