using DocumentFormat.OpenXml;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Spreadsheet;
using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExcelLib
{
    public class ExcelOp : IExcelOp
    {
        private SpreadsheetDocument _excelDoc;
        private WorkbookPart _workBookPart;
        private string _error;

        /// <summary>
        /// 打开Excel文件
        /// </summary>
        /// <param name="path">文件路径</param>
        /// <param name="opExcelType">指定打开的方式</param>
        /// <returns></returns>
        public bool OpenExcel(string path, OpExcelType opExcelType)
        {
            try
            {
                switch (opExcelType)
                {
                    case OpExcelType.Read:
                        _excelDoc = SpreadsheetDocument.Open(path, false);
                        _workBookPart = _excelDoc.WorkbookPart;
                        break;
                    case OpExcelType.Modify:
                        _excelDoc = SpreadsheetDocument.Open(path, true);
                        _workBookPart = _excelDoc.WorkbookPart;
                        break;
                    case OpExcelType.Create:
                        CreateExcel(path,"Sheet 1");
                        break;
                    default:
                        return false;
                }
            }
            catch (Exception e)
            {
                _error = e.Message;
                return false;
            }
            return _workBookPart != null;
        }

        /// <summary>
        /// 创建一个空白Excel文件
        /// </summary>
        /// <param name="path"></param>
        /// <param name="sheetName"></param>
        /// <returns></returns>
        public bool CreateExcel(string path, string sheetName)
        {
            //1. 创建Excel 工作薄
            _excelDoc = SpreadsheetDocument.Create(path, SpreadsheetDocumentType.Workbook);
            _workBookPart = _excelDoc.AddWorkbookPart();
            _workBookPart.Workbook = new Workbook();

            //2. 在工作薄中添加一个表单
            var worksheetPart = _workBookPart.AddNewPart<WorksheetPart>();
            var sheets = _excelDoc.WorkbookPart.Workbook.AppendChild(new Sheets());
            var sheet = new Sheet()
            {
                Id = _excelDoc.WorkbookPart.GetIdOfPart(worksheetPart),
                SheetId = 1,
                Name = sheetName
            };
            sheets.AppendChild(sheet);

            //3. 提供向Excel表格添加空白数据
            var sheetData = new SheetData();
            worksheetPart.Worksheet = new Worksheet(sheetData);

            return true;
        }

        /// <summary>
        /// 将Excel所有表单数据存入到DataSet对象中
        /// </summary>
        /// <returns></returns>
        public DataSet GetData()
        {
            var dataSet = new DataSet();
            var sheetNames = GetSheetNames();
            foreach (var sheetName in sheetNames)
            {
                var sheetData = GetSheetData(sheetName);
                dataSet.Tables.Add(sheetData);
            }

            return dataSet;
        }

        /// <summary>
        /// 将指定表单名的数据存入到DataTable中
        /// </summary>
        /// <param name="sheetName"></param>
        /// <returns></returns>
        public DataTable GetSheetData(string sheetName)
        {
            var dataTable = new DataTable(sheetName);

            var workSheet = GetWorkSheet(sheetName);
            var sheetData = workSheet.Elements<SheetData>().First();
            var rows = sheetData.Elements<Row>();

            foreach (var row in rows)
            {
                //获取Excel中的列头
                if (row.RowIndex == 1)
                {
                    List<DataColumn> listCols = GetDataColumn(row);
                    dataTable.Columns.AddRange(listCols.ToArray());
                }
                var dataRow = GetDataRow(row, dataTable);
                if (dataRow != null)
                {
                    dataTable.Rows.Add(dataRow);
                }
            }
            return dataTable;
        }

        /// <summary>
        /// 获取Excel文件中所有表单的名称
        /// </summary>
        /// <returns></returns>
        public List<string> GetSheetNames()
        {
            var sheetNames = new List<string>();
            var sheets = _workBookPart.Workbook.Descendants<Sheet>().ToList();
            foreach (var sheet in sheets)
            {
                sheetNames.Add(sheet.Name);
            }
            return sheetNames;
        }

        public bool AddTextCell(string sheetName, int column, int row, string text)
        {
            var workSheet = GetWorkSheet(sheetName);
            var sheetData = workSheet.Elements<SheetData>().First();
            var cell = CreateTextCell(column, text);
            var newRow = GetRow(sheetData, row);
            newRow.AppendChild(cell);
            workSheet.Save();
            return true;
        }

        public bool AppendRow(string sheetName, List<string> rowData)
        {
            int currentRow = GetRowCount(sheetName);
            for(int i = 0; i < rowData.Count; i++)
            {
                AddTextCell(sheetName, i, currentRow + 1, rowData[i]);
            }

            return true;
        }

        public bool AppendCell(string sheetName, int column, int row, string text)
        {
            var workSheet = GetWorkSheet(sheetName);
            var sheetData = workSheet.Elements<SheetData>().First();
            var cell = CreateTextCell(column, text);
            var newRow = GetRow(sheetData, row);
            newRow.AppendChild(cell);
            workSheet.Save();
            return true;
        }

        public bool ModifyCell(string sheetName, int column, int row, string value)
        {
            var workSheet = GetWorkSheet(sheetName);

            InsertTextCellValue(workSheet, column, (uint)row, value);

            return true;
        }



        /// <summary>
        /// 获取最近一次调用失败的信息
        /// </summary>
        /// <returns></returns>
        public string GetLastError()
        {
            return _error;
        }

        /// <summary>
        /// 关闭文档
        /// </summary>
        public void Close()
        {
            _workBookPart.Workbook.Save();
            _excelDoc.Close();
        }

        private int GetRowCount(string sheetName)
        {
            var dataTable = new DataTable(sheetName);

            var workSheet = GetWorkSheet(sheetName);
            var sheetData = workSheet.Elements<SheetData>().First();
            var rows = sheetData.Elements<Row>().ToList();

            return rows.Count;
        }
        

        /// <summary>
        /// 获取单元格的内容
        /// </summary>
        /// <param name="cell"></param>
        /// <returns></returns>
        private string GetCellValue(Cell cell)
        {
            string cellValue = string.Empty;
            if (cell.ChildElements.Count == 0)//Cell节点下没有子节点
            {
                return cellValue;
            }
            string cellRefId = cell.CellReference.InnerText;//获取引用相对位置
            string cellInnerText = cell.CellValue.InnerText;//获取Cell的InnerText
            cellValue = cellInnerText;//指定默认值(其实用来处理Excel中的数字)

            //获取WorkbookPart中共享String数据
            SharedStringTable sharedTable = _workBookPart.SharedStringTablePart.SharedStringTable;

            try
            {
                EnumValue<CellValues> cellType = cell.DataType;//获取Cell数据类型
                if (cellType != null)//Excel对象数据
                {
                    switch (cellType.Value)
                    {
                        case CellValues.SharedString://字符串
                            int cellIndex = int.Parse(cellInnerText);
                            cellValue = sharedTable.ChildElements[cellIndex].InnerText;
                            break;
                        case CellValues.Boolean://布尔
                            cellValue = (cellInnerText == "1") ? "TRUE" : "FALSE";
                            break;
                        case CellValues.Date://日期
                            cellValue = Convert.ToDateTime(cellInnerText).ToString();
                            break;
                        case CellValues.Number://数字
                            cellValue = Convert.ToDecimal(cellInnerText).ToString();
                            break;
                        default: cellValue = cellInnerText; break;
                    }
                }
            }
            catch (Exception exp)
            {
                _error = exp.Message;
                cellValue = "N/A";
            }
            return cellValue;
        }

        private List<DataColumn> GetDataColumn(Row row)
        {
            List<DataColumn> listCols = new List<DataColumn>();
            foreach (Cell cell in row)
            {
                string cellValue = GetCellValue(cell);
                DataColumn col = new DataColumn(cellValue);
                listCols.Add(col);
            }
            return listCols;
        }

        private static Cell ReturnCell(Worksheet worksheet, string columnName, uint row)
        {
            Row targetRow = ReturnRow(worksheet, row);

            if (targetRow == null)
                return null;

            return targetRow.Elements<Cell>().Where(c =>
               string.Compare(c.CellReference.Value, columnName + row,
               true) == 0).First();
        }
        private static Row GetRow(SheetData sheetData, int row)
        {
            var rows = sheetData.Elements<Row>().ToList();
            foreach (var oneRow in rows)
            {
                if (oneRow.RowIndex == row)
                {
                    return oneRow;
                }
            }
            var newRow = new Row { RowIndex = (uint)row };
            sheetData.AppendChild(newRow);

            return newRow;
        }
        private static Row ReturnRow(Worksheet worksheet, uint row)
        {
            return worksheet.GetFirstChild<SheetData>().
            Elements<Row>().Where(r => r.RowIndex == row).First();
        }

        /// <summary>
        /// 创建文本形式的单元格
        /// </summary>
        /// <param name="column"></param>
        /// <param name="text"></param>
        /// <returns></returns>
        private Cell CreateTextCell(int column, string text)
        {
            var cell = new Cell
            {
                DataType = CellValues.InlineString,
                CellReference = ColumnLetter(column) + column
            };

            var istring = new InlineString();
            var t = new Text { Text = text };
            istring.AppendChild(t);
            cell.AppendChild(istring);
            return cell;
        }

        private void InsertTextCellValue(Worksheet worksheet, int column, uint row, string value)
        {
            Cell cell = ReturnCell(worksheet, ColumnLetter(column), row);
            CellValue v = new CellValue();
            v.Text = value;
            cell.AppendChild(v);
            cell.DataType = new EnumValue<CellValues>(CellValues.String);
            worksheet.Save();
        }
        private void InsertNumberCellValue(Worksheet worksheet, int column, uint row, string value)
        {
            Cell cell = ReturnCell(worksheet, ColumnLetter(column), row);
            CellValue v = new CellValue();
            v.Text = value;
            cell.AppendChild(v);
            cell.DataType = new EnumValue<CellValues>(CellValues.Number);

            worksheet.Save();
        }

        /// <summary>
        /// 将数字索引转换为字符串索引。例如 0 代表A1 1代表B1 ....
        /// </summary>
        /// <param name="intCol"></param>
        /// <returns></returns>
        private string ColumnLetter(int intCol)
        {
            var intFirstLetter = ((intCol) / 676) + 64;
            var intSecondLetter = ((intCol % 676) / 26) + 64;
            var intThirdLetter = (intCol % 26) + 65;

            var firstLetter = (intFirstLetter > 64) ? (char)intFirstLetter : ' ';
            var secondLetter = (intSecondLetter > 64) ? (char)intSecondLetter : ' ';
            var thirdLetter = (char)intThirdLetter;

            return string.Concat(firstLetter, secondLetter, thirdLetter).Trim();
        }

        private DataRow GetDataRow(Row row, DataTable dateTable)
        {
            //读取Excel中数据,一一读取单元格,若整行为空则忽视该行
            DataRow dataRow = dateTable.NewRow();
            IEnumerable<Cell> cells = row.Elements<Cell>();

            int cellIndex = 0;//单元格索引
            int nullCellCount = cellIndex;//空行索引
            foreach (Cell cell in row)
            {
                string cellVlue = GetCellValue(cell);
                if (string.IsNullOrEmpty(cellVlue))
                {
                    nullCellCount++;
                }

                dataRow[cellIndex] = cellVlue;
                cellIndex++;
            }
            if (nullCellCount == cellIndex)//剔除空行
            {
                dataRow = null;//一行中单元格索引和空行索引一样
            }
            return dataRow;
        }

        private Worksheet GetWorkSheet(string sheetName)
        {
            var sheets = _workBookPart.Workbook.Descendants<Sheet>().ToList();
            var sheet = (from st in sheets where st.Name == sheetName select st).First();

            //get work sheet columns
            var workSheet = ((WorksheetPart)_workBookPart.GetPartById(sheet.Id)).Worksheet;

            return workSheet;
        }
    }
}
