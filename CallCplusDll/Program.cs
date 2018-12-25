//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Runtime.InteropServices;
//using System.Data;
//using UtilLib;
//using CplusplusDll;


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace WfpApp
{
    class PositionNotepad
    {
        [DllImport("User32.dll", EntryPoint = "FindWindow")]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
        [DllImport("user32.dll")]
        static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindow);
        [DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr hWnd);
        ///<summary>  
        /// 定位到txt文件指定行  
        ///</summary>  
        ///<param name="strFullName">文件路径</param>  
        ///<param name="strRow">指定行</param>  
        ///<returns>定位是否成功</returns>  
        public static bool PositionNotePad(string strFullName, string strRow)
        {
            int iRow;
            int.TryParse(strRow, out iRow);
            if (iRow <= 0)
            {
                return false;
            }
            //查看当前文件是否已打开 
            IntPtr hwnd = FindWindow("Notepad", string.Format("{0} - 记事本", Path.GetFileName(strFullName)));
            if (hwnd.ToInt32() == 0)
            {
                Process p = Process.Start(@"notepad.exe", strFullName);
                //等一秒，等文本打开，焦点去到notepad
                p.WaitForInputIdle(1000);
                SendKeys.SendWait("{DOWN " + (iRow - 1) + "}");
                SendKeys.SendWait("{HOME}"); //行首
                SendKeys.SendWait("+{END}"); //选中当前行
                return true;
            }
            else
            {
                hwnd = FindWindowEx(hwnd, IntPtr.Zero, "Edit", string.Empty);
                if (hwnd.ToInt32() == 0)
                    return false;
                else
                {
                    SetForegroundWindow(hwnd);
                    SendKeys.SendWait("^{HOME}");//将光标定位到首行
                    SendKeys.SendWait("{DOWN " + (iRow - 1) + "}");
                    SendKeys.SendWait("{HOME}"); //行首
                    SendKeys.SendWait("+{END}"); //选中当前行
                }
            }
            return true;
        }
        static void Main(string[] args)
        {
            PositionNotePad("D:\\test.log", "14");
        }
    }
}
