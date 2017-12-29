# This module define some hex int number to string type

# convert int value to hex string type
def IntToStr(intValue):
    strNum = '{0:X}'.format(intValue)
    if len(strNum) % 2 != 0:
        strNum = '0' + strNum
    return strNum

# get bcd data hex str len
def GetBcdDataLen(bcdData):
    bcdLen = int(len(bcdData) / 2)
    return IntToStr(bcdLen)


# convert int sw to string sw
def SW(sw):
	strNum = '{0:X}'.format(sw)
	if len(strNum) % 2 != 0:
		strNum = '0' + strNum
	return strNum






if __name__ == '__main__':
    print(IntToStr(4))
    print(IntToStr(26))
    GetBcdDataLen("3F00")
    GetBcdDataLen("32149895898392844")