from card_check.util import ReturnRet

def PBOC_GPO_SJHGX_001(resp):
    if len(resp) < 2:
        return ReturnRet.S_ERROR
    if resp[0:2] != "80":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK

def PBOC_GPO_SJHGX_002(tlvs):
    pass

def PBOC_GPO_SJHGX_003(tlvs):
    if len(tlvs) != 1:
        return ReturnRet.S_ERROR
    gpoValue = tlvs[0].value
    if len(gpoValue) < 4:
        return ReturnRet.S_ERROR
    tag94 = gpoValue[4:len(gpoValue)]
    if tag94 % 4 != 0:
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK


        