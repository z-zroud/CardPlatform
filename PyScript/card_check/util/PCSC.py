import os
import sys
from ctypes import *
from .PCSCEnum import PCSCStatus
from .PCSCEnum import SCARD_PROTOCOL
from .Tool import SW

dllName = 'PCSC.dll'
dllPath = os.path.dirname(os.path.abspath(__file__))
#print(dllPath)
dirList = dllPath.split(os.path.sep)
#print(dirList)
dllPath = os.path.sep.join(dirList[0:len(dirList) - 1]) + os.path.sep + "dll" + os.path.sep + dllName

#print(dllPath)
ReaderLib = CDLL(dllPath)


# Get smart card readers
def GetReaders():
	readers = []
	charArrType = c_char_p * 10
	readerNameArr = charArrType()
	readerNum = c_int(10)
	ReaderLib.GetReaders(readerNameArr,byref(readerNum))
	for count in range(readerNum.value):
		readers.append(bytes.decode(readerNameArr[count]))
		#print(readerNameArr[count])
	return readers


# Open smart card reader by specified name
# Note: reader name must be bytes type
# if sucess, OpenReader return True, otherwise return false
def OpenReader(readerName):
    print('selected reader: ', readerName)
    name = str.encode(readerName)
    ReaderLib.OpenReader.restype = c_bool
    result = ReaderLib.OpenReader(name)
    return result

# Close reader and release relative resource
def CloseReader():
	ReaderLib.CloseReader()

# reset reader status using SCARD_LEAVE_CARD, we call it warm reset
def WarmReset():
	result = ReaderLib.WarmReset()
	return bool(result)

# reset reader status using SCARD_UNPOWER_CARD, we call it cold reset
def ColdReset():
	result = ReaderLib.ColdReset()
	return bool(result)

# get card status
def GetCardStatus():
	status = PCSCStatus(ReaderLib.GetCardStatus())
	return status

# get communication protocol between reader and smart card
def GetTransProtocol():
	status = SCARD_PROTOCOL(ReaderLib.GetTransProtocol())
	return status

# get smart card ATR
def GetATR():
	ATRLen = c_int(128)
	bytesATR = create_string_buffer(ATRLen.value)
	ReaderLib.GetATR(bytesATR,ATRLen)
	return bytes.decode(bytesATR.value)

# send apdu command, this function return a tuple eg. tuple(SW, data).
# var cmd is string type, not bytes type.
def SendApdu(cmd):
	#print('send apdu: ' + cmd)
	bytesCmd = str.encode(cmd)
	respLen = c_int(2048)
	respData = create_string_buffer(respLen.value)
	sw = ReaderLib.SendApdu(bytesCmd,respData,respLen)
	data = bytes.decode(respData.value)
	#print( 'apdu response:  ' + SW(sw) + '  ' + data)
	return sw,data

# send apdu command, this function return only sw
def SendApduSW(cmd):
	#print('send apdu: ' + cmd)
	bytesCmd = str.encode(cmd)
	respLen = c_int(2048)
	respData = create_string_buffer(respLen.value)
	sw = ReaderLib.SendApdu(bytesCmd,respData,respLen)
	#data = bytes.decode(respData.value)
	#print( 'apdu response:  ' + SW(sw) + '  ' + data)
	return sw


if __name__ == '__main__':
	readers = GetReaders()
	for reader in readers:
		print(reader)
	if OpenReader(readers[0]) is True:
		print(WarmReset())
		print(ColdReset())
		print(GetCardStatus())
		print(GetTransProtocol())
		print(GetATR())
		print(SendApdu('00a40400 00'))
	CloseReader()
