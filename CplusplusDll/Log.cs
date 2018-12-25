using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CplusplusDll
{


    public enum LogLevel
    {
        Info,
        Warn,
        Error
    }

    public class Log
    {
        [DllImport("User32.dll", EntryPoint = "FindWindow")]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
        [DllImport("user32.dll")]
        static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindow);
        [DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr hWnd);

        private static Log _log;
        private static int handle;
        private static string fileName;



        public static Log CreateLog(string path)
        {
            handle = CDll.CreateLog(path);
            fileName = path;
            return CreateLog();
        }

        public static Log CreateLog()
        {
            if (handle != 0x10000000)
            {
                _log = new Log();
            }
            return _log;
        }

        /// <summary>
        /// 清空日志文件
        /// </summary>
        public void ClearLog()
        {
            CDll.ClearLog(handle);
        }

        /// <summary>
        /// 添加带有信息等级的日志
        /// </summary>
        /// <param name="level"></param>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public void TraceLog(LogLevel level, string format, params object[] args)
        {
            _TraceLog(true, level, format, args);
        }

        /// <summary>
        /// 添加不带信息等级的日志
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public void TraceLog(string format, params object[] args)
        {
            _TraceLog(false, LogLevel.Info, format, args);
        }

        public int GetCurrentLine()
        {
            return CDll.GetCurLine();
        }

        private void _TraceLog(bool containsLevel,LogLevel level,string format, params object[] args)
        {
            string info = string.Format(format, args);
            if (containsLevel)
            { 
                info = level.ToString() + ":" + info;
            }
            info +=  "\n";
            
            CDll.TraceLog(handle, info);
        }

        public bool ShowLogPos(int row)
        {
            if (row <= 0)
            {
                return false;
            }
            //查看当前文件是否已打开 
            IntPtr hwnd = FindWindow("Notepad", string.Format("{0} - 记事本", Path.GetFileName(fileName)));
            if (hwnd.ToInt32() == 0)
            {
                Process p = Process.Start(@"notepad.exe", fileName);
                //等一秒，等文本打开，焦点去到notepad
                p.WaitForInputIdle(1000);
                SendKeys.SendWait("{DOWN " + (row - 1) + "}");
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
                    SendKeys.SendWait("{DOWN " + (row - 1) + "}");
                    SendKeys.SendWait("{HOME}"); //行首
                    SendKeys.SendWait("+{END}"); //选中当前行
                }
            }
            return true;
        }
    }
}
