from enum import Enum

# define smart card status
class PCSCStatus(Enum):
	SCARD_UNKNOWN = 0
	SCARD_ABSENT = 1
	SCARD_PRESENT = 2
	SCARD_SWALLOWED = 3
	SCARD_POWERED = 4
	SCARD_NEGOTIABLE = 5
	SCARD_SPECIFIC = 6

# define communication protocol between reader and smart card
class SCARD_PROTOCOL(Enum):
	SCARD_PROTOCOL_UNDEFINED = 0
	SCARD_PROTOCOL_T0 = 1
	SCARD_PROTOCOL_T1 = 2
	SCARD_PROTOCOL_RAW = 0x10000

class DIV_METHOD(Enum):
	NO_DIV = 0
	DIV_CPG202 = 1
	DIV_CPG212 = 2

class SECURE_LEVEL(Enum):
	SL_NO_SECURE = 0
	SL_MAC = 1
	SL_MAC_ENC = 2