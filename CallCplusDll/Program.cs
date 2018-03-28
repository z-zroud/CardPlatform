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
            string data;
            data = Algorithm.Des3Encrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            data = Algorithm.Des3Decrypt("12345678900987765434123456789761", "4EAE6AB88A307FA94EAE6AB88A307FA9");
            data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "0000000000000000000000000000000000");

            data = Algorithm.AscToBcd("4858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345");
            //data = Algorithm.BcdToAsc(data);
            string ret = Utils.IntToString(1, 2);
            ret = Utils.IntToString(15, 2);
            ret = Utils.IntToString(20, 2);

            var readers = SCReader.GetReaders();
            if(APDU.RegisterReader(readers[0]))
            {
                APDU.SelectCmd(Constant.CreditAid);
            }
        }
    }
}
