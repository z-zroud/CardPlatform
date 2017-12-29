# This module define some hex int number to string type

def HexStr(hexNum):
	num = int(hexNum / 2)
	strNum = '{0:X}'.format(num)
	if len(strNum) % 2 != 0:
		strNum = '0' + strNum
	return strNum


def SW(sw):
	strNum = '{0:X}'.format(sw)
	if len(strNum) % 2 != 0:
		strNum = '0' + strNum
	return strNum






if __name__ == '__main__':
	print(HexStr(4))
	print(HexStr(26))