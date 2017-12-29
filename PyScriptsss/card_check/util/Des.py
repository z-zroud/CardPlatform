# This module define the algorithm relative with card bussiness.
# it include basic algorithm like des 3des and sh1.
import os
import sys
from ctypes import *

dllName = 'Des0.dll'
dllPath = os.path.dirname(os.path.abspath(__file__))
#print(dllPath)
dirList = dllPath.split(os.path.sep)
#print(dirList)
dllPath = os.path.sep.join(dirList[0:len(dirList) - 1]) + os.path.sep + "dll" + os.path.sep + dllName

DesLib = CDLL(dllPath)

def DesEncrypt(key, data):
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib.Des(output,bytesKey,bytesData)
	return bytes.decode(output.value)

def DesDecrypt(key,data):
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib._Des(output,bytesKey,bytesData)
	return bytes.decode(output.value)

def Des3Encrypt(key,data):
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib.Des3(output,bytesKey,bytesData)
	return bytes.decode(output.value)

def Des3Decrypt(key,data):
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib._Des3(output,bytesKey,bytesData)
	return bytes.decode(output.value)

# algorithm description: 
# step 1. 8 bytes block + 3Des encrypt ==> result1
# step 2. result1 xor next 8 bytes block ==> result2
# step 3. recursive 1 to 2 until the last block
# stpe 4. result = result1 + result2 + ... + resultn
def Des3CBCEncrypt(key,data):
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib.Des3_CBC(output,bytesKey,bytesData,dataLen)
	return bytes.decode(output.value)

# algorithm description:
# step1. 8 bytes block + 3Des encrypt ==> result1
# step2. next 8 bytes block + 3Des encrypt ==> result2
# step3. recursive... step2
# step4. result = result1 + result2 + ... + resultn
def Des3ECBEncrypt(key,data):	
	dataLen = len(data)
	output = create_string_buffer(dataLen + 1)
	bytesKey = str.encode(key)
	bytesData = str.encode(data)
	DesLib.Des3_ECB(output,bytesKey,bytesData,dataLen)
	return bytes.decode(output.value)

# algorithm description:
# step1. initData xor block1  + key(L) ==> result1
# step2. result1 xor next block + key(L) ==> result2
# step3. recursive step2 until the last block ==> resultn
# step4. resultn + key(R) decrypt ==> ret1
# step5. ret1 + key(L) encrypt ==> mac
def Des3Mac(key, initData, inData):
	bytesKey = str.encode(key)
	bytesInitData = str.encode(initData)
	bytesInData = str.encode(inData)
	output = create_string_buffer(17)
	DesLib.DES_3DES_CBC_MAC(bytesInData,bytesKey,bytesInitData,output)
	return bytes.decode(output.value)

#algorithm description:
# step1. initData + 8 bytes block + 3Des ==> result1
# step2. result1 + next block + 3Des ==> result2
# step3. recursive step2 until the last block ==> mac
def Des3FullMac(key, initData, inData):
	bytesKey = str.encode(key)
	bytesInitData = str.encode(initData)
	bytesInData = str.encode(inData)
	output = create_string_buffer(17)
	DesLib.Full_3DES_CBC_MAC(bytesInData,bytesKey,bytesInitData,output)
	return bytes.decode(output.value)

# xor two string. note data1 and data2 must have the same length.
def Xor(data1, data2):
	bytesData1 = str.encode(data1)
	bytesData2 = str.encode(data2)
	dataLen = len(data1)
	output = create_string_buffer(dataLen + 1)
	output.value = bytesData1
	DesLib.str_xor(output,bytesData2,dataLen)
	return bytes.decode(output.value)



if __name__ == '__main__':
	print(DesEncrypt('1122334455667788','1111111111111111'))
	print(DesDecrypt('1122334455667788','1111111111111111'))
	print(Des3Encrypt('00112233445566778899AABBCCDDEEFF','1111111111111111'))
	print(Des3Decrypt('00112233445566778899AABBCCDDEEFF','1111111111111111'))
	print(Des3CBCEncrypt('00112233445566778899AABBCCDDEEFF','11111111111111111333333333333333'))
	print(Des3ECBEncrypt('00112233445566778899AABBCCDDEEFF','11111111111111111333333333333333'))
	print(Des3Mac('00112233445566778899AABBCCDDEEFF','00000000000000000000000000000000','00000000000000000000000000000000000'))
	print(Des3FullMac('00112233445566778899AABBCCDDEEFF','00000000000000000000000000000000','00000000000000000000000000000000000'))
	print(Xor('1234567890','9876543210'))