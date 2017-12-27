#This module define command relative with MOT applicaiton

from .PCSC import SendApdu
from .Tool import HexDataLen
from . import Kmc
from . import Des

# This function is used in period of persionlization,
# and EC has been installed in advance. if you want to 
# make the local ADF share with EC about balance, you 
# need to call this func after create the ADF file.
def ShareInstance(aid='A000000632010106'):
	cmd = '00D10000' + HexDataLen(aid) + aid
	return SendApdu(cmd)
	
def GetChanllenge():
	cmd = '00840000 04'
	return SendApdu(cmd)

# Used for initialize card
def InitializeCard(initializeKey):
	cardChallenge = GetChanllenge()
	initCardInput = cardChallenge[0:8] + '000000000000000000000000'
	authencationCryptogram = Des.Des3ECBEncrypt(initializeKey, initCardInput)
	cmd = '84010000 08' + authencationCryptogram[0:16]
	return SendApdu(cmd)

# This function is uesed to check the validation of external device.
# and authorize access right for device to manipulate card.
def ExternalAuthenticate(keyId, exAuthKey):
	cardChallenge = GetChanllenge()
	initCardInput = cardChallenge[0:8] + '000000000000000000000000'
	authencationCryptogram = Des.Des3ECBEncrypt(exAuthKey, initCardInput)
	cmd = '008200' + keyId + '08' + authencationCryptogram[0:16]
	return SendApdu(cmd)

