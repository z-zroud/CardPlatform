using DocumentFormat.OpenXml.Spreadsheet;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExcelLib
{
    public enum OpExcelType
    {
        Read = 0,
        Modify = 1,
        Create = 2
    }

    public interface IExcelOp
    {
        /// <summary>
        /// 打开Excel表格，也可以是创建(默认名称为sheet 1)
        /// </summary>
        /// <param name="path"></param>
        /// <param name="isEditable"></param>
        /// <returns></returns>
        bool OpenExcel(string path, OpExcelType opExcelType = OpExcelType.Read);

        /// <summary>
        /// 创建Excel表格，指定表格名称
        /// </summary>
        /// <param name="path"></param>
        /// <param name="sheetName"></param>
        /// <returns></returns>
        bool CreateExcel(string path, string sheetName);

        /// <summary>
        /// 关闭Excel文档
        /// </summary>
        void Close();

        /// <summary>
        /// 获取Excel表中所有数据
        /// </summary>
        /// <returns></returns>
        DataSet GetData();

        /// <summary>
        /// 获取Excel表中指定的sheet表格数据
        /// </summary>
        /// <param name="sheetName"></param>
        /// <returns></returns>
        DataTable GetSheetData(string sheetName);

        /// <summary>
        /// 获取Excel 所有sheet名称
        /// </summary>
        /// <returns></returns>
        List<string> GetSheetNames();

        /// <summary>
        /// 在sheetName表格中，添加一行数据
        /// </summary>
        /// <param name="sheetName"></param>
        /// <param name="rowData"></param>
        /// <returns></returns>
        bool AppendRow(string sheetName, List<string> rowData);

        /// <summary>
        /// 向sheetName表单中添加指定位置的单元格内容
        /// </summary>
        /// <param name="sheetName"></param>
        /// <param name="columen"></param>
        /// <param name="row"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        bool AddTextCell(string sheetName, int columen, int row, string value);

        /// <summary>
        /// 修改某个单元格的内容
        /// </summary>
        /// <param name="sheetName"></param>
        /// <param name="column"></param>
        /// <param name="row"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        bool ModifyCell(string sheetName, int column, int row, string value);

        /// <summary>
        /// 附加value值到指定的单元格上
        /// </summary>
        /// <param name="sheetName"></param>
        /// <param name="column"></param>
        /// <param name="row"></param>
        /// <param name="vlaue"></param>
        /// <returns></returns>
        bool AppendCell(string sheetName, int column, int row, string vlaue);

        /// <summary>
        /// 获取最后一次接口调用失败的原因
        /// </summary>
        /// <returns></returns>
        string GetLastError();
    }
}
