# This module define some hex int number to string type

from card_check.util import DataParse
# convert int value to hex string type
def IntToHexStr(intValue):
    strNum = '{0:X}'.format(intValue)
    if len(strNum) % 2 != 0:
        strNum = '0' + strNum
    return strNum

# get bcd data hex str len
def GetBcdDataLen(bcdData):
    bcdLen = int(len(bcdData) / 2)
    return IntToHexStr(bcdLen)


# convert int sw to string sw
def SW(sw):
	strNum = '{0:X}'.format(sw)
	if len(strNum) % 2 != 0:
		strNum = '0' + strNum
	return strNum

def BcdToAsc(bcd):
    result = ""
    if len(bcd) % 2 != 0:
        return result
    for index in range(0, len(bcd), 2):
        temp = bcd[index:index + 2]
        asc = chr(int(temp,16))
        result += asc
    return result




if __name__ == '__main__':
    print(BcdToAsc("FF68"))
    print(IntToHexStr(4))
    print(IntToHexStr(26))
    GetBcdDataLen("3F00")
    GetBcdDataLen("32149895898392844")