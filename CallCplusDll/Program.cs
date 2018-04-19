using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Data;
using UtilLib;
using CplusplusDll;

namespace CallCplusDll
{
    class Program
    {
        static void Main(string[] args)
        {


            string ret;
            ret = Utils.StrToBcd("cn");
            ret = Utils.BcdToStr(ret);
            ret = Convert.ToString(0x80, 16);
            ret = Authencation.GenDesKcv("12345678900987765434123456789761");
            ret = Authencation.GenSmKcv("12345678900987765434123456789761");
            ret = Authencation.GenCAPublicKey("03", "A000000333");
            string data;
            data = Algorithm.Des3Encrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            data = Algorithm.Des3Decrypt("12345678900987765434123456789761", "4EAE6AB88A307FA94EAE6AB88A307FA9");
            data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "0000000000000000000000000000000000");

            data = Algorithm.AscToBcd("4858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345");
            //data = Algorithm.BcdToAsc(data);
            ret = Utils.IntToString(1, 2);
            ret = Utils.IntToString(15, 2);
            ret = Utils.IntToString(20, 2);

            string bcdBuffer = "6F688408A000000333010101A55C500E556E696F6E5061792044656269748701019F381BDF60019F66049F02069F03069F1A0295055F2A029A039C019F37045F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0E9F4D020B0ADF4D020C0ADF610142";
            List<TLV> result = DataParse.ParseTLV(bcdBuffer);

            string tlBuffer = "9F7A019F02065F2A02DF6901";
            List<TL> tlList = DataParse.ParseTL(tlBuffer);

            string aflBuffer = "08010100100103011010100018010400";
            List<AFL> aflList = DataParse.ParseAFL(aflBuffer);

            var readers = SCReader.GetReaders();
            if(APDU.RegisterReader(readers[0]))
            {
                //APDU.SelectCmd(Constant.CreditAid);
            }
        }
    }
}
