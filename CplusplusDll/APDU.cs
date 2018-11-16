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
            string p1 = Utils.IntToHexStr(recordNo, 2);
            string p2 = Utils.IntToHexStr(SFI, 2);

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

        /// <summary>
        /// 生成动态应用数据，也就是9F4B
        /// </summary>
        /// <param name="DDOLData"></param>
        /// <returns></returns>
        public static string GenDynamicDataCmd(string DDOLData)
        {
            string DDOLDataLen = Utils.GetBcdLen(DDOLData);
            string cmd = "00880000" + DDOLDataLen + DDOLData;

            var resp = SendApdu(cmd);
            if(resp.SW == 0x9000)
            {
                var tlvs = DataParse.ParseTLV(resp.Response);
                if(resp.Response.Substring(0,2) == "80")
                {
                    if (tlvs.Count == 1)
                    {
                        return tlvs[0].Value;   //Tag9F4B
                    }
                    return string.Empty;
                }else if(resp.Response.Substring(0, 2) == "77")
                {
                    if (tlvs.Count == 2)
                    {
                        return tlvs[1].Value;   //MC应用 发送内部认证返回带77模板的9F4B数据
                    }
                }
            }
            return string.Empty;
        }

        /// <summary>
        /// 生成应用密文
        /// </summary>
        /// <param name="acType">密文类型</param>
        /// <param name="cdol">由终端组成的CDOL 数据</param>
        /// <returns></returns>
        public static ApduResponse GACCmd(int acType, string cdol)
        {
            string type = Convert.ToString(acType, 16);
            string dataLen = Utils.GetBcdLen(cdol);
            string cmd = "80AE" + type + "00" + dataLen + cdol;

            return SendApdu(cmd);
        }

        /// <summary>
        /// 外部认证
        /// </summary>
        /// <param name="ARPC"></param>
        /// <param name="authCode"></param>
        /// <returns></returns>
        public static ApduResponse ExtAuthCmd(string ARPC, string authCode)
        {
            string extAuthData = ARPC + authCode;
            string dataLen = Utils.GetBcdLen(extAuthData);

            string cmd = "00820000" + dataLen + extAuthData;

            return SendApdu(cmd);
        }

        /// <summary>
        /// 向卡片设置tag值
        /// </summary>
        /// <param name="tag"></param>
        /// <param name="data"></param>
        /// <returns></returns>
        public static ApduResponse PutDataCmd(string tag, string data, string mac)
        {
            if(tag.Length == 2)
            {
                tag = "00" + tag;
            }
            string cmdData = data + mac;
            string cmdDataLen = Utils.GetBcdLen(cmdData);

            string cmd = "04DA" + tag + cmdDataLen + cmdData;

            return SendApdu(cmd);
        }

    }
}
