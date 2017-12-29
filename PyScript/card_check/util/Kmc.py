# This module generate various of sub key and session key.
# eg. kmc sub key and session key
# eg. udk sub key and session key
from PCSCEnum import DIV_METHOD
import Des

# This method generate three kmc sub key tuple
# tuple(auth,mac,enc)
def GenKmcSubKey(kmc, divMethod, divData):
	leftDivData = ""
	rightDivData = ""
	if divMethod == DIV_METHOD.DIV_CPG202:
		leftDivData = divData[0:4] + divData[8:16]
		rightDivData = leftDivData
	elif divMethod == DIV_METHOD.DIV_CPG212:
		leftDivData = divData[8:20]
		rightDivData = leftDivData
	else:
		return kmc,kmc,kmc
	kmcAuthKey = _GenKmcAuthKey(kmc, leftDivData, rightDivData)
	kmcMacKey = _GenKmcMacKey(kmc, leftDivData, rightDivData)
	kmcEncKey = _GenKmcEncKey(kmc, leftDivData, rightDivData)
	return kmcAuthKey,kmcMacKey,kmcEncKey

def GenSecureChannelSessionKey(kmc,divMethod,termialRandom,initializeUpdateResp):
	sessionAuthKey = ""
	sessionMacKey = ""
	sessionEncKey = ""
	kekKey = ""
	divData = initializeUpdateResp[0:20]
	keyVersion = initializeUpdateResp[20:22]
	scp = initializeUpdateResp[22:24]
	cardChallenge = initializeUpdateResp[24:40]
	cardCryptogram = initializeUpdateResp[40:56]
	kmcAuthKey,kmcMacKey,kmcEncKey = GenKmcSubKey(kmc, divMethod, divData)
	if scp == 1:
		leftDivData = cardChallenge[8:16] + termialRandom[0:8]
		rightDivData = cardChallenge[0:8] + termialRandom[8:16]
		divData = leftDivData + rightDivData
		sessionAuthKey = Des.Des3ECBEncrypt(kmcAuthKey, divData)
		sessionMacKey = Des.Des3ECBEncrypt(kmcMacKey, divData)
		sessionEncKey = Des.Des3ECBEncrypt(kmcEncKey, divData)
		kekKey = kmcEncKey
	else:	#scp == 2
		seqNo = cardChallenge[0:4]
		sessionAuthKey = _GenSecureChannelSessionAuthKey(seqNo, kmcAuthKey)
		sessionMacKey = _GenSecureChannelSessionMacKey(seqNo, kmcMacKey)
		sessionEncKey = _GenSecureChannelSessionEncKey(seqNo, kmcEncKey)
		kekKey = sessionEncKey

	return sessionAuthKey,sessionMacKey,sessionEncKey,kekKey


def _GenSecureChannelSessionAuthKey(seqNo,authKey):
	leftDivData = '0182' + seqNo + '00000000'
	rightDivData = '0000000000000000'
	return _GenSecureChannelSessionKey(authKey, leftDivData, rightDivData)

def _GenSecureChannelSessionMacKey(seqNo,macKey):
	leftDivData = '0101' + seqNo + '00000000'
	rightDivData = '0000000000000000'
	return _GenSecureChannelSessionKey(macKey, leftDivData, rightDivData)

def _GenSecureChannelSessionEncKey(seqNo,encKey):
	leftDivData = '0181' + seqNo + '00000000'
	rightDivData = '0000000000000000'
	return _GenSecureChannelSessionKey(encKey, leftDivData, rightDivData)


def _GenSecureChannelSessionKey(key,leftDivData,rightDivData):
	leftKey = Des.Des3Encrypt(key, leftDivData)
	tmp = Des.Xor(leftKey, rightDivData)
	rightKey = Des.Des3Encrypt(key, tmp)
	return leftKey + rightKey


def _GenKmcAuthKey(kmc,leftDivData,rightDivData):
	leftDivData += 'F001'
	rightDivData += '0F01'
	return _GenKmcSubKey(kmc, leftDivData, rightDivData)

def _GenKmcMacKey(kmc,leftDivData,rightDivData):
	leftDivData += 'F002'
	rightDivData += '0F02'
	return _GenKmcSubKey(kmc, leftDivData, rightDivData)

def _GenKmcEncKey(kmc,leftDivData,rightDivData):
	leftDivData += 'F003'
	rightDivData += '0F03'
	return _GenKmcSubKey(kmc, leftDivData, rightDivData)

def _GenKmcSubKey(kmc,leftDivData,rightDivData):
	leftKey = Des.Des3Encrypt(kmc, leftDivData)
	rightKey = Des.Des3Encrypt(kmc, rightDivData)
	return leftKey + rightKey
