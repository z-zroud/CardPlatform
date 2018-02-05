from card_check.util import ReturnRet

def PBOC_GPO_SJHGX_001(resp):
    if len(resp) < 2:
        return ReturnRet.S_ERROR
    if resp[0:2] != "80":
        return ReturnRet.S_ERROR
    return ReturnRet.S_OK
        