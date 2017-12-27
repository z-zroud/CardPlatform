# This module define a collection of apdu command and some
# basic function reletive to apdu command.
from .PCSC import SendApdu
from .Tool import HexDataLen,HexStr
from .PCSCEnum import DIV_METHOD,SECURE_LEVEL
from . import Kmc
from . import Des
from ctypes import *

dllName = 'ApduCmd.dll'
dllPath = os.path.dirname(os.path.abspath(__file__))
#print(dllPath)
dirList = dllPath.split(os.path.sep)
#print(dirList)
dllPath = os.path.sep.join(dirList[0:len(dirList) - 1]) + os.path.sep + "dll" + os.path.sep + dllName

ApduLib = CDLL(dllPath)

resp_len = 2048

#########################################################################
#           Wrap ApduCmd.dll function
#########################################################################
def GPOCmd(terminalData):
    resp = create_string_buffer(resp_len)
    btyesTerminalData = str.encode(terminalData)
    sw = ApduLib.GPOCmd(btyesTerminalData,resp)
    return sw,bytes.decode(resp.value)


def PutDataCmd(tag,value,mac):
    bytesTag = str.encode(tag)
    bytesValue = str.encode(value)
    bytesMac = str.encode(mac)
    sw = ApduLib.PutDataCmd(bytesTag,bytesValue,bytesMac)
    return sw,""

def DeleteAppCmd(aid):
    bytesAid = str.encode(aid)
    sw = ApduLib.DeleteAppCmd(bytesAid)
    return sw,""

def InternalAuthencationCmd(ddolData):
    bytesDdolData = str.encode(ddolData)
    resp = create_string_buffer(resp_len)
    sw = ApduLib.InternalAuthencationCmd(bytesDdolData,resp)
    return sw,bytes.decode(resp.value)

def ExternalAuthencationCmd(arpc,authCode):
    bytesArpc = str.encode(arpc)
    bytesAuthCode = str.encode(authCode)
    resp = create_string_buffer(resp_len)
    sw = ApduLib.ExternalAuthencationCmd(bytesArpc,bytesAuthCode,resp)
    return sw,bytes.decode(resp.value)

def GACCmd(terminalCryptogramType,cdolData):
    cTerminalCryptogramType = c_int(terminalCryptogramType)
    bytesCdolData = str.encode(cdolData)
    resp = create_string_buffer(resp_len)
    sw = ApduLib.GACCmd(cTerminalCryptogramType,bytesCdolData,resp)
    return sw,bytes.decode(resp.value)

def GenDynamicData(ddolData):
    bytesDdolData = str.encode(ddolData)
    resp = create_string_buffer(resp_len)
    return bytes.decode(resp.value)


# Used for selecting an application
def Select(aid):
	cmd = '00A40400' + HexDataLen(len(aid)) + aid
	return SendApdu(cmd)

# Use fid to select an application
def SelectByName(name):
	cmd = '00A40000' + HexDataLen(len(name)) + name
	return SendApdu(cmd)

# Read record, p1 and sfi are string type
def ReadRecordCmd(p1, sfi):
	tmp = int(sfi,base=16)
	p2 = (tmp << 3) + 4
	p2Str = HexStr(p2)
	cmd = "00B2" + p1 + p2Str
	return SendApdu(cmd)

# This cmd is used to transmit card and Secure Channel Session data
# between the card and the host. This cmd initiates the initiation of
# a Secure Channel Session
def InitializeUpdate(terminalRandom, keyVersionNum='00', keyIdentifier='00'):	
	cmd = '8050' + keyVersionNum + keyIdentifier
	cmd += HexDataLen(len(terminalRandom)) + terminalRandom
	return SendApdu(cmd)
	
def ExternalAuthenticate(kmc, divMethod,terminalRandom, secureLevel, InitializeUpdateResp):
	cmd = ''
	hostCryptogramAndMacLen = '10'
	mac = ''
	if secureLevel == SECURE_LEVEL.SL_MAC:
		cmd = '84820100'
	elif secureLevel == SECURE_LEVEL.SL_MAC_ENC:
		cmd = '84820300'
	else:
		cmd = '84820000'
	scp = InitializeUpdateResp[22:24]
	cardChallenge = InitializeUpdateResp[24:40]
	cardCryptogram = InitializeUpdateResp[40:56]
	hostChallengeInput = cardChallenge + terminalRandom
	sessionAuthKey, sessionMacKey, *_ = KeyGenerator.GenSecureChannelSessionKey(kmc, divMethod, terminalRandom, InitializeUpdateResp)
	validateMacInput = terminalRandom + cardChallenge
	validateMac = Des.Des3FullMac(sessionAuthKey, '0000000000000000', validateMacInput)
	if validateMac != cardCryptogram:
		return (0x6300,'')
	hostChallenge = Des.Des3FullMac(sessionAuthKey, '0000000000000000', hostChallengeInput)
	cmd += hostCryptogramAndMacLen + hostChallenge
	if scp == '01':
		mac = Des.Des3FullMac(sessionAuthKey, '0000000000000000', cmd)
	else:	#scp == '02'
		mac = Des.Des3Mac(sessionMacKey, '0000000000000000', cmd)
	cmd += mac
	return SendApdu(cmd)

def OpenSecureChannel(kmc, divMethod=DIV_METHOD.NO_DIV, secureLevel=SECURE_LEVEL.SL_NO_SECURE):
	terminalRandom = '1122334455667788'
	sw, InitializeUpdateResp = InitializeUpdate(terminalRandom)
	print(InitializeUpdateResp)
	if sw != 0x9000:
		return False
	sw, *_ = ExternalAuthenticate(kmc, divMethod, terminalRandom, secureLevel, InitializeUpdateResp)
	if sw != 0x9000:
		return False
	return True




if __name__ == '__main__':
	import PCSC
	readers = PCSC.GetReaders()
	PCSC.OpenReader(readers[0])
	print(PCSC.GetATR())
	Select('A000000003000000')
	result = OpenSecureChannel('404142434445464748494A4B4C4D4E4F',DIV_METHOD.DIV_CPG202)
	print(result)
	PCSC.CloseReader()


