using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CplusplusDll
{
    public class SCReader : ICDll
    {
        public void CloseReader()
        {
            CDll.CloseReader();
        }

        public bool ColdReset()
        {
            return CDll.ColdReset();
        }

        public string GetATR()
        {
            int len = 128;
            StringBuilder atr = new StringBuilder(len);
            CDll.GetATR(atr, len);
            return atr.ToString();
        }

        public int GetCardStatus()
        {
            return CDll.GetCardStatus();
        }

        public List<string> GetReaders()
        {
            var readers = new List<string>();
            int count = 12;
            IntPtr[] intPtrs = new IntPtr[count];
            for(int i = 0; i < count; i++)
            {
                intPtrs[i] = new IntPtr();
            }
            CDll.GetReaders(intPtrs, ref count);
            for(int j = 0; j < count; j++)
            {
                string reader = Marshal.PtrToStringAnsi(intPtrs[j]);
                readers.Add(reader);
            }
            return readers;
        }

        public int GetTransProtocol()
        {
            return CDll.GetTransProtocol();
        }

        public bool OpenReader(string reader)
        {
            return CDll.OpenReader(reader);
        }

        public int SendApdu(string cmd, ref string result)
        {
            int resultLen = 2048;
            StringBuilder apduRet = new StringBuilder(resultLen);
            int sw = CDll.SendApdu(cmd, apduRet, resultLen);
            result = apduRet.ToString();

            return sw;
        }

        public int SendApdu(string cmd)
        {
            return CDll.SendApdu2(cmd);
        }

        public bool WarmReset()
        {
            return CDll.WarmReset();
        }
    }
}
