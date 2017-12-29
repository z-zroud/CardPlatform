from card_check.util import ApduCmd
from card_check.util import DataParse
from card_check.util import Authencation

def SelectPSE():
    sw,resp = ApduCmd.SelectAppCmd("315041592E5359532E4444463031")
    print("SW=",sw,"resp=",resp)
