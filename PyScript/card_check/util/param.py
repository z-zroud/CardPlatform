# This module define some fixed data which used by PBOC test
from enum import Enum
readerName = ""

udkAuthKey = "8A58D9DCD5520DC494C40761CDA82F7F"
udkMacKey = "BC0D7915A4C245B01A2919F1751CC449"
udkEncKey = "680BC8619283F86DC4A12573297C0270"

aid = "A000000333010101"

termParams = {"9F02":"000000000100", #授权金额 JR13
             "9F03":"000000000000", #其他金额 JR13
             "9F04":"000000000000", #与交易相关的第2金额，表示返现金额(二进制) JR6
             "9F1A":"0156",     #终端国家代码 JR6
             "5F2A":"0156",     #交易货币代码 JR6
             "95":"0000000000", #终端验证结果TVR，初始值为0 JR6
             "9C":"00",     #交易类型
             "DF60":"00",   #CAPP交易指示位 JR14
             "DF69":"00",   #SM2算法支持指示器
             "9F7A":"00",   #电子现金终端支持指示器 JR13 pass
             "9F66":"26000080",     #终端交易属性 JR12
             "9F4E":"1234567899876543210012345678900987654321",#终端商户名称
             "8A":"3030"}#for test, need set by C++




