
class TLVEx:
	def __init__(self):
		self.isTemplate = False
		self.level = 0
		self.tag = ''
		self.length = 0
		self.value = ''

# define TL struct
class TL:
	def __init__(self):
		self.tag = ''
		self.length = 0

# used to parse TL struct. eg. PDOL data
def ParseTL(tlData):
	tlList = []	
	index = 0
	parseType = 'T' #'T' 'L'
	dataLen = len(tlData)
	while index < dataLen:
		if parseType == 'T':
			tl = TL()
			tagLen = 2 #default tag len
			# this means tag contain multi bytes
			if int(tlData[index],base=16) & 0x01 == 0x01 and int(tlData[index + 1] == 'F'):
				tagLen = 4
			tl.tag = tlData[index: index + tagLen]
			index += tagLen
			parseType = 'L'
			#print("Tag: ",tl.tag)
			continue #finish parse tag attribute
		elif parseType == 'L':
			if int(tlData[index],base=16) & 0x08 == 0x08:	#len is biger than FF, just treat len has two bytes
				tl.length = int(tlData[index + 2 : index + 4],base=16)
				index += 6
			else:
				#print('index: ',index)
				#print(tlData[index : index + 2])
				tl.length = int(tlData[index : index + 2],base=16)
				index += 2
			continue
		else:
			break

	return tlList

# there are not two same level template		
def ParseTLV(tlvData):
	result = 0
	tlvList = []	
	index = 0
	parseType = 'T' #'T' 'L' 'V'
	dataLen = len(tlvData)
	curLevel = 0
	lastTagIsTemplate = False
	while index < dataLen:
		if parseType == 'T':
			tlv = TLVEx()
			tagLen = 2 #default tag len
			if int(tlvData[index],base=16) & 0x02 == 0x02:	# is template
				tlv.isTemplate = True		
								
				# this means tag contain multi bytes
			if int(tlvData[index],base=16) & 0x01 == 0x01 and int(tlvData[index + 1] == 'F'):
				tagLen = 4
			tlv.tag = tlvData[index: index + tagLen]
			tlv.level = curLevel
			index += tagLen
			parseType = 'L'
			if index == dataLen:
				result = 1 # just has tag but not has length, this is not correct TLV struct.
			#print("Tag: ",tlv.tag)
			continue #finish parse tag attribute
		elif parseType == 'L':
			if int(tlvData[index],base=16) & 0x08 == 0x08:	#len is biger than FF, just treat len has two bytes
				tlv.length = int(tlvData[index + 2 : index + 4],base=16)
				index += 6
				if tlv.isTemplate is True:
					if len(tlvData[index,dataLen]) != tlv.length * 2:
						result = 2

			else:
				#print('index: ',index)
				#print(tlvData[index : index + 2])
				tlv.length = int(tlvData[index : index + 2],base=16)
				index += 2
			if tlv.isTemplate is True:
				tlvList.append(tlv)
				lastTagIsTemplate = True
				curLevel += 1	
				parseType = 'T' 
			else:
				parseType = 'V'
				lastTagIsTemplate = False
			continue
		elif parseType == 'V':
			tlv.value = tlvData[index : index + tlv.length * 2]
			parseType = 'T'
			index += tlv.length * 2
			tlvList.append(tlv)
			#print('value: ', tlv.value)
			continue
		else:
			break

	return result, tlvList

def GetTLVError(result):
	if result == 1:		return "it's not correct TLV format."
	elif result == 2: 	return "some exception data behind template."





if __name__ == "__main__":
	# result,ret = ParseTLV("6F1E840E315041592E5359532E4444463031A50C8801015F2D027A689F11010102")
	# if result != 0:
	# 	print(GetTLVError(result))
	# for item in ret:
	# 	print('tag=',item.tag, 'isTemplate=', item.isTemplate,'level=',item.level, 'len=', item.length, 'value=',item.value)
	result, ret1 = ParseTLV("6F558408A000000333010101A549500E556E696F6E5061792044656269748701019F380C9F7A019F02065F2A02DF69015F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0A9F4D020B0ADF4D020C0A")
	for item in ret1:
		print('tag=',item.tag, 'isTemplate=', item.isTemplate,'level=',item.level, 'len=', item.length, 'value=',item.value)
