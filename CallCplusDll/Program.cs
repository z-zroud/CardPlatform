using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CallCplusDll
{
    class Program
    {
        [DllImport(@"F:\CardPlatform\bin\PCSC.dll", EntryPoint = "GetReaders", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        extern static  int GetReaders(IntPtr[] readers, ref int count);

        [DllImport(@"F:\CardPlatform\bin\PCSC.dll", EntryPoint = "OpenReader", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        extern static bool OpenReader(string reader);

        [DllImport(@"F:\CardPlatform\bin\PCSC.dll", EntryPoint = "GetATR", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        extern static void GetATR(StringBuilder atr, int len);

        [DllImport(@"F:\CardPlatform\bin\PCSC.dll", EntryPoint = "GetApduError", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        extern static IntPtr GetApduError();

        static void Main(string[] args)
        {
            int count1 = 3;
            IntPtr[] intPtr = new IntPtr[4];
            intPtr[0] = new IntPtr();
            intPtr[1] = new IntPtr();
            intPtr[2] = new IntPtr();
            intPtr[3] = new IntPtr();
            GetReaders(intPtr, ref count1);
            string xxx = Marshal.PtrToStringAnsi(intPtr[0]);
            string xxxx = Marshal.PtrToStringAnsi(intPtr[1]);
            string xxxx1 = Marshal.PtrToStringAnsi(intPtr[2]);
            string xxxx2 = Marshal.PtrToStringAnsi(intPtr[3]);
            //if(OpenReader(sr))
            //{
            //    StringBuilder atr = new StringBuilder(1024);
            //    int len = 1024;
            //    GetATR(atr,  len);
            //}

        }
    }
}
