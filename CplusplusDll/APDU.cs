using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UtilLib;

namespace CplusplusDll
{
    public class ApduResponse
    {
        public int SW { get; set; }
        public string Request { get; set; }
        public string Response { get; set; }
    }

    public static class APDU
    {
        public static bool RegisterReader(string readerName)
        {
            return SCReader.OpenReader(readerName); 
        }

        public static ApduResponse SendApdu(string cmd)
        {
            ApduResponse apduResponse = new ApduResponse();
            string response = string.Empty;
            apduResponse.SW = SCReader.SendApdu(cmd, ref response);
            apduResponse.Response = response;
            apduResponse.Request = cmd;
            return apduResponse;
        }
        /// <summary>
        /// 选择应用
        /// </summary>
        /// <param name="aid"></param>
        /// <returns></returns>
        public static ApduResponse SelectCmd(string aid)
        {
            string cmd = "00A40400" + Utils.GetBcdLen(aid) + aid;

            return SendApdu(cmd);
        }

        /// <summary>
        /// 读取记录
        /// </summary>
        /// <param name="SFI"></param>
        /// <param name="recordNo"></param>
        /// <returns></returns>
        public static ApduResponse ReadRecordCmd(int SFI, int recordNo)
        {
            SFI = (SFI << 3) + 4;
            string p1 = Utils.IntToString(recordNo, 2);
            string p2 = Utils.IntToString(SFI, 2);

            string cmd = "00B2" + p1 + p2;

            return SendApdu(cmd);
        }

        /// <summary>
        /// 获取tag命令
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        public static ApduResponse GetDataCmd(string tag)
        {
            if(tag.Length == 2)
            {
                tag = "00" + tag;
            }
            if(tag.Length != 4)
            {
                return new ApduResponse();
            }
            string cmd = "80CA" + tag;
            return SendApdu(cmd);
        }

        /// <summary>
        /// GPO命令
        /// </summary>
        /// <param name="PDOLData"></param>
        /// <returns></returns>
        public static ApduResponse GPOCmd(string PDOLData)
        {
            string PDOLDataLen = Utils.GetBcdLen(PDOLData);
            string data = "83" + PDOLDataLen + PDOLData;
            string dataLen = Utils.GetBcdLen(data);
            string cmd = "80A80000" + dataLen + data;

            return SendApdu(cmd);
        }

    }
}
