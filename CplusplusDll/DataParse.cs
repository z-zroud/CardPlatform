using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace CplusplusDll
{
    public class TLV
    {
        public bool IsTemplate { get; set; }
        public int Level { get; set; }
        public string Tag { get; set; }
        public int Len { get; set; }
        public string Value { get; set; }
    }

    public class TL
    {
        public string Tag { get; set; }
        public int Len { get; set; }
    }

    public class AFL
    {
        public int SFI { get; set; }
        public int RecordNo { get; set; }
        public bool IsSignedRecordNo { get; set; }
    }

    public static class DataParse
    {
        public static List<TLV> ParseTLV(string bcdBuffer)
        {
            var tlvs = new List<TLV>();
            
            int count = 32;
            int size = Marshal.SizeOf(typeof(CDll.TLVStruct)) * count;
            IntPtr intPtrs = Marshal.AllocHGlobal(size);

            CDll.ParseTLV(bcdBuffer, intPtrs, ref count);
            CDll.TLVStruct[] arrTLV = new CDll.TLVStruct[count];
            for (int i = 0; i < count; i++)
            {
                IntPtr ptr = new IntPtr(intPtrs.ToInt32() + Marshal.SizeOf(typeof(CDll.TLVStruct)) * i);
                arrTLV[i] = (CDll.TLVStruct)Marshal.PtrToStructure(ptr, typeof(CDll.TLVStruct));

                TLV tlv = new TLV();
                tlv.IsTemplate = arrTLV[i].IsTemplate;
                tlv.Len = arrTLV[i].Length;
                tlv.Level = arrTLV[i].Level;
                tlv.Tag = Marshal.PtrToStringAnsi(arrTLV[i].Tag);
                tlv.Value = Marshal.PtrToStringAnsi(arrTLV[i].Value);
                tlvs.Add(tlv);
            }

            return tlvs;
        }

        public static List<TL> ParseTL(string bcdBuffer)
        {
            var tls = new List<TL>();
            int count = 32;
            int size = Marshal.SizeOf(typeof(CDll.TLStruct)) * count;
            IntPtr pTL = Marshal.AllocHGlobal(size);
            CDll.ParseTL(bcdBuffer, pTL, ref count);
            for(int i = 0; i < count; i++)
            {
                IntPtr intPtr = new IntPtr(pTL.ToInt32() + Marshal.SizeOf(typeof(CDll.TLStruct)) * i);
                CDll.TLStruct tlStruct = (CDll.TLStruct)Marshal.PtrToStructure(intPtr, typeof(CDll.TLStruct));
                TL tl = new TL();
                tl.Tag = Marshal.PtrToStringAnsi(tlStruct.Tag);
                tl.Len = tlStruct.Length;
                tls.Add(tl);
            }
            return tls;
        }

        public static List<AFL> ParseAFL(string bcdBuffer)
        {
            var tls = new List<AFL>();
            int count = 32;
            int size = Marshal.SizeOf(typeof(CDll.AFLStruct)) * count;
            IntPtr pAFL = Marshal.AllocHGlobal(size);
            CDll.ParseAFL(bcdBuffer, pAFL, ref count);
            for (int i = 0; i < count; i++)
            {
                IntPtr intPtr = new IntPtr(pAFL.ToInt32() + Marshal.SizeOf(typeof(CDll.AFLStruct)) * i);
                CDll.AFLStruct aflStruct = (CDll.AFLStruct)Marshal.PtrToStructure(intPtr, typeof(CDll.AFLStruct));
                AFL afl = new AFL();
                afl.SFI = aflStruct.sfi;
                afl.RecordNo = aflStruct.recordNumber;
                afl.IsSignedRecordNo = aflStruct.bSigStaticData;
                tls.Add(afl);
            }
            return tls;
        }
    }
}
