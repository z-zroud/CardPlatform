using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CplusplusDll
{
    public class CDll
    {
        #region import PCSC.dll
        [DllImport(@"PCSC.dll", EntryPoint = "GetReaders", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static int GetReaders(IntPtr[] readers, ref int count);

        [DllImport(@"PCSC.dll", EntryPoint = "OpenReader", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool OpenReader(string reader);

        [DllImport(@"PCSC.dll", EntryPoint = "GetATR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static void GetATR(StringBuilder atr, int len);

        [DllImport(@"PCSC.dll", EntryPoint = "WarmReset", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool WarmReset();

        [DllImport(@"PCSC.dll", EntryPoint = "ColdReset", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool ColdReset();

        [DllImport(@"PCSC.dll", EntryPoint = "CloseReader", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static void CloseReader();

        [DllImport(@"PCSC.dll", EntryPoint = "GetCardStatus", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static int GetCardStatus();

        [DllImport(@"PCSC.dll", EntryPoint = "GetTransProtocol", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static int GetTransProtocol();

        [DllImport(@"PCSC.dll", EntryPoint = "SendApdu", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static int SendApdu(string cmd, StringBuilder output, int len);

        [DllImport(@"PCSC.dll", EntryPoint = "SendApdu2", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static int SendApdu2(string cmd);
        #endregion

        #region import CPS.dll
        [DllImport(@"CPS.dll", EntryPoint = "GenCpsFile", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool GenCpsFile(string dllName, string funcName, string dpFile, string ruleFile);

        [DllImport(@"CPS.dll", EntryPoint = "GetCpsFiles", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool GetCpsFiles(IntPtr[] readers, ref int count);

        [DllImport(@"CPS.dll", EntryPoint = "SetPersonlizationConfig", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static void SetPersonlizationConfig(string isd, string kmc, int divMethod, int secureLevel);

        [DllImport(@"CPS.dll", EntryPoint = "DoPersonlization", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public extern static bool DoPersonlization(string cpsFile, string iniConfigFile);
        #endregion
    }
}
