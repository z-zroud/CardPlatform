﻿using System;
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
            string arc = "0012";
            string acSessionKey = Authencation.GenMcSessionKeySkd("F180230B9257CBB5E607A7E6EF15B646", "0011", "E3C387D5");
            //Authencation.GenArpc(acSessionKey,)
            //IExcelOp op = new ExcelOp();
            //op.OpenExcel("F:\\Goldpac\\Perso.xlsm", OpExcelType.Modify);
            //var names = op.GetSheetNames();
            //string name = "PBOC&UICS";
            ////op.ModifyCell(name, 0x0E, 41, "ABCDEFG");
            //op.ModifyCell(name, 0x04, 41, "7777");
            //op.ModifyCell(name, 0x04, 42, "7777");
            //op.ModifyCell(name, 0x04, 227, "7777");
            //op.AddTextCell(name, 0x08, 41, "88888");
            //bool r = false;
            //string bcd1 = "70B39381B03C7B01E12BDCAA437640A4DB515AD8B02A7199F3B2DA854008756070D6CAFD88A33CD75982CBFF5D664B2F7B0AF3C3886CCA6DC31599294B70A33770B7736E20C0DF089C85AF3FE7509D41DA93A247CCDB112F496C51AAC102E304E9A3026AFEA6522AA63A25A9E36312F3A74DF3583E0F29E84A6F22F2C352B0195DB175BA221066A43709B0600BB396A364A0A737FD5BA93E5371C67D3C4AB5BB5429DC995D113FE5D6D754B80B6E69773B1530ADAF";
            //string bcd2 = "6F688408A000000333010101A55C500E556E696F6E5061792044656269748701019F381BDF60019F66049F02069F03069F1A0295055F2A029A039C019F37045F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0E9F4D020B0ADF4D020C0ADF610142";
            //string bcd3 = "6F688408A000000333010101A55C500E556E696F6E5061792044656269748701019F381BDF60019F66049F02069F03069F1A0295055F2A029A039C019F37045F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0E9F4D020B0ADF4D020C0ADF61014212345";
            //r = DataParse.IsTLV(bcd1);
            //r = DataParse.IsTLV(bcd2);
            //r = DataParse.IsTLV(bcd3);

            //string ret;
            //ret = Utils.StrToBcd("cn");
            //ret = Utils.BcdToStr(ret);
            //ret = Convert.ToString(0x80, 16);
            //ret = Authencation.GenDesKcv("12345678900987765434123456789761");
            //ret = Authencation.GenSmKcv("12345678900987765434123456789761");
            //ret = Authencation.GenCAPublicKey("03", "A000000333");
            //string data;
            //data = Algorithm.Des3Encrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            //data = Algorithm.Des3Decrypt("12345678900987765434123456789761", "4EAE6AB88A307FA94EAE6AB88A307FA9");
            //data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "00000000000000000000000000000000");
            //data = Algorithm.Des3ECBEncrypt("12345678900987765434123456789761", "0000000000000000000000000000000000");

            //data = Algorithm.AscToBcd("4858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345123094858694034593451230948586940345934512309485869403459345");
            ////data = Algorithm.BcdToAsc(data);
            //ret = Utils.IntToHexStr(1, 2);
            //ret = Utils.IntToHexStr(15, 2);
            //ret = Utils.IntToHexStr(20, 2);

            //string bcdBuffer = "6F688408A000000333010101A55C500E556E696F6E5061792044656269748701019F381BDF60019F66049F02069F03069F1A0295055F2A029A039C019F37045F2D027A689F1101019F120E556E696F6E506179204465626974BF0C0E9F4D020B0ADF4D020C0ADF610142";
            //List<TLV> result = DataParse.ParseTLV(bcdBuffer);

            //string tlBuffer = "9F7A019F02065F2A02DF6901";
            //List<TL> tlList = DataParse.ParseTL(tlBuffer);

            //string aflBuffer = "08010100100103011010100018010400";
            //List<AFL> aflList = DataParse.ParseAFL(aflBuffer);

            //var readers = SCReader.GetReaders();
            //if(APDU.RegisterReader(readers[0]))
            //{
            //    //APDU.SelectCmd(Constant.CreditAid);
            //}
        }
    }
}
