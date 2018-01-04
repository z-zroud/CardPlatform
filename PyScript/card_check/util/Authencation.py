import os
import sys
from ctypes import *

dllName = "Authencation.dll"
dllPath = os.path.dirname(os.path.abspath(__file__))
#print(dllPath)
dirList = dllPath.split(os.path.sep)
#print(dirList)
dllPath = os.path.sep.join(dirList[0:len(dirList) - 1]) + os.path.sep + "dll" + os.path.sep + dllName

AuthLib = CDLL(dllPath)

def GenCAPublicKey(index,rid):
    bytesIndex = str.encode(index)
    bytesRid = str.encode(rid)
    publicKey = create_string_buffer(2048)
    AuthLib.GenCAPublicKey(bytesIndex,bytesRid,publicKey)
    return bytes.decode(publicKey.value)

def GenDesIssuerPublicKey(caPublicKey,issuerPublicCert,ipkRemainder,issuerExponent):
    bytesCaPublicKey = str.encode(caPublicKey)
    bytesIssuerPublicCert = str.encode(issuerPublicCert)
    bytesIpkRemainder = str.encode(ipkRemainder)
    bytesIssuerExponent = str.encode(issuerExponent)
    issuerPublicKey = create_string_buffer(2048)
    AuthLib.GenDesIssuerPublicKey(bytesCaPublicKey,bytesIssuerPublicCert,bytesIpkRemainder,bytesIssuerExponent,issuerPublicKey)
    return bytes.decode(issuerPublicKey.value)

def GenDesICCPublicKey(issuerPublicKey,iccPublicCert,iccRemainder,sigStaticData,iccExponent,tag82):
    bytesIssuerPublicKey = str.encode(issuerPublicKey)
    bytesIccPublicCert = str.encode(iccPublicCert)
    bytesIccRemainder = str.encode(iccRemainder)
    bytesSigStaticData = str.encode(sigStaticData)
    bytesIccExponent = str.encode(iccExponent)
    bytesTag82 = str.encode(tag82)
    iccPublicKey = create_string_buffer(2048)
    AuthLib.GenDesICCPublicKey(bytesIssuerPublicKey,bytesIccPublicCert,bytesIccRemainder,bytesSigStaticData,bytesIccExponent,bytesTag82,iccPublicKey)
    return bytes.decode(iccPublicKey.value)

def GenSMIssuerPublicKey(caPublicKey,issuerPublicCert):
    bytesCaPublicKey = str.encode(caPublicKey)
    bytesIssuerPublicCert = str.encode(issuerPublicCert)
    issuerPublicKey = create_string_buffer(2048)
    AuthLib.GenSMIssuerPublicKey(bytesCaPublicKey,bytesIssuerPublicCert,issuerPublicKey)
    return bytes.decode(issuerPublicKey)

def GenSMICCPublicKey(issuerPublicKey,iccPublicCert,needAuthStaticData):
    bytesIssuerPublicKey = str.encode(issuerPublicKey)
    bytesIccPublicCert = str.encode(iccPublicCert)
    bytesNeedAuthStaticData = str.encode(needAuthStaticData)
    iccPublicKey = create_string_buffer(2048)
    AuthLib.GenSMICCPublicKey(bytesIssuerPublicKey,bytesIccPublicCert,bytesNeedAuthStaticData,iccPublicKey)
    return bytes.decode(iccPublicKey.value)

def DES_SDA(issuerPublicKey,ipkExponent,tag93,sigStaticData,tag82):
    bytesIssuerPublicKey = str.encode(issuerPublicKey)
    bytesIpkExponent = str.encode(ipkExponent)
    bytesTag93 = str.encode(tag93)
    bytesSigStaticData = str.encode(sigStaticData)
    bytesTag82 = str.encode(tag82)
    return AuthLib.DES_SDA(bytesIssuerPublicKey,bytesIpkExponent,bytesTag93,bytesSigStaticData,bytesTag82)

def SM_SDA(issuerPublicKey,ipkExponent,sigStaticData,tag93,tag82):
    bytesIssuerPublicKey = str.encode(issuerPublicKey)
    bytesIpkExponent = str.encode(ipkExponent)
    bytesSigStaticData = str.encode(sigStaticData)
    bytesTag93 = str.encode(tag93)
    bytesTag82 = str.encoe(tag82)
    return AuthLib.SM_SDA(bytesIssuerPublicKey,bytesIpkExponent,bytesSigStaticData,bytesTag93,bytesTag82)

def DES_DDA(iccPublicKey,iccExponent,tag9F4B,dynamicData):
    bytesIccPublicKey = str.encode(iccPublicKey)
    bytesIccExponent = str.encode(iccExponent)
    bytesTag9F4B = str.encode(tag9F4B)
    bytesDynamicData = str.encode(dynamicData)
    return AuthLib.DES_DDA(bytesIccPublicKey,bytesIccExponent,bytesTag9F4B,bytesDynamicData)

def SM_DDA(iccPublicKey,dynamicData):
    bytesIccPublicKey = str.encode(iccPublicKey)
    bytesDynamicData = str.encode(dynamicData)
    return AuthLib.SM_DDA(bytesIccPublicKey,bytesDynamicData)

def GenUdkSessionKey(udkSubKey,atc):
    udkSessionKey = create_string_buffer(33)
    bytesUdkSubKey = str.encode(udkSubKey)
    bytesAtc = str.encode(atc)
    AuthLib.GenUdkSessionKey(bytesUdkSubKey,bytesAtc,udkSessionKey)
    return bytes.decode(udkSessionKey.value)

def GenUdk(mdk,cardNo,cardSequence):
    bytesMdk = str.encode(mdk)
    bytesCardNo = str.encode(cardNo)
    bytesCardSequence = str.encode(cardSequence)
    udk = create_string_buffer(33)
    AuthLib.GenUdk(bytesMdk,bytesCardNo,bytesCardSequence,udk)
    return bytes.decode(udk.value)

def GenArpc(udkAuthSessionKey,ac,authCode):
    bytesUdkAuthSessionKey = str.encode(udkAuthSessionKey)
    bytesAc = str.encode(ac)
    bytesAuthCode = str.encode(authCode)
    arpc = create_string_buffer(128)
    AuthLib.GenArpc(bytesUdkAuthSessionKey,bytesAc,bytesAuthCode,arpc)
    return bytes.decode(arpc.value)

def GenIssuerScriptMac(udkMacSessionKey,data):
    bytesUdkMacSessionKey = str.encode(udkMacSessionKey)
    bytesData = str.encode(data)
    mac = create_string_buffer(32)
    AuthLib.GenIssuerScriptMac(bytesUdkMacSessionKey,bytesData,mac)
    return bytes.decode(mac.value)

