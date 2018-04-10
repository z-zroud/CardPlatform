using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CplusplusDll
{
    public static class SCReader
    {
        public static void CloseReader()
        {
            CDll.CloseReader();
        }

        public static bool ColdReset()
        {
            return CDll.ColdReset();
        }

        public static string GetATR()
        {
            int len = 128;
            StringBuilder atr = new StringBuilder(len);
            CDll.GetATR(atr, len);
            return atr.ToString();
        }

        public static int GetCardStatus()
        {
            return CDll.GetCardStatus();
        }

        public static List<string> GetReaders()
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

        public static int GetTransProtocol()
        {
            return CDll.GetTransProtocol();
        }

        public static bool OpenReader(string reader)
        {
            if (string.IsNullOrWhiteSpace(reader))
                return false;
            return CDll.OpenReader(reader);
        }

        public static int SendApdu(string cmd, ref string result)
        {
            int resultLen = 2048;
            StringBuilder apduRet = new StringBuilder(resultLen);
            int sw = CDll.SendApdu(cmd, apduRet, resultLen);
            result = apduRet.ToString();

            return sw;
        }

        public static int SendApdu(string cmd)
        {
            return CDll.SendApdu2(cmd);
        }

        public static bool WarmReset()
        {
            return CDll.WarmReset();
        }
    }
}
