using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Data;
using UtilLib;

namespace CallCplusDll
{
    class Program
    {
        static void Main(string[] args)
        {
            IExcelOp excelOp = new ExcelOp();
            if(excelOp.OpenExcel("E:\\C#\\temp2018.xlsx",OpExcelType.Modify))
            {
                //string sheetName = "Sheet1";
                var sheetName = excelOp.GetSheetNames().First();
                DataTable excelDataTable = excelOp.GetSheetData(sheetName);
                List<string> newRow = new List<string>(){ "XXX", "YYY", "ZZZ" };
                excelOp.AppendRow(sheetName, newRow);
                
            }

        }
    }
}
