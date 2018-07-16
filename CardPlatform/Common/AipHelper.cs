using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Common
{
    public class AipHelper
    {
        private string aip;

        public AipHelper(string aip)
        {
            this.aip = aip;
        }

        //检查AIP支持的功能
        private int GetFirstByteOfAIP()
        {
            if (aip.Length != 4)
            {
                return 0;
            }
            return Convert.ToInt32(aip.Substring(0, 2), 16);
        }

        public bool IsSupportSDA()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x40) == 0x40)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportDDA()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x20) == 0x20)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportCardHolderVerify()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x10) == 0x10)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportTerminalRiskManagement()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x08) == 0x08)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportIssuerAuth()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x04) == 0x04)
            {
                return true;
            }
            return false;
        }

        public bool IsSupportCDA()
        {
            var firstByte = GetFirstByteOfAIP();
            if ((firstByte & 0x01) == 0x01)
            {
                return true;
            }
            return false;
        }
    }
}
