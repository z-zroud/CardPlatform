from Base import HexStr
# b = b"example"
# s = "example"
# sb = bytes(s, encoding="utf8")
# bs = str(b,encoding="utf8")

# sb2 = str.encode(s)
# bs2 = bytes.decode(b)

# print(sb)
# print(bs)
# print(sb2)
# print(bs2)

# strlen = '{0:x}'.format(10)
# lenStr = len('xxxxwetwehruwehtuw')
# strlen = '{0:x}'.format(lenStr)	

# teststr = "ABCDEFG"
# x = teststr[1:6]
# print(x)

# print(strlen)

# temp = '6'
# print(int(temp,base=16) & 0x02 == 0x02)

def ReadRecord(p1, sfi):
	tmp = int(sfi,base=16)
	p2 = (tmp << 3) + 4
	p2Str = HexStr(p2)
	cmd = "00B2" + p1 + p2Str
	print(cmd)

if __name__ == '__main__':
	ReadRecord('01', '01')
	ReadRecord('02', '15')
	ReadRecord('01', '08')
	l = [1,2,3,4]
	print(l[2:2])
	print(l[2:4])
