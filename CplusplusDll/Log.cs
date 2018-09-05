using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
        private static Log _log;
        private static int handle;

        public static Log CreateLog(string path)
        {
            handle = CDll.CreateLog(path);
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

        public void TraceLog(LogLevel level, string format, params object[] args)
        {
            string info = string.Format(format, args);
            info = level.ToString() + ":" + info + "\n";
            CDll.TraceLog(handle, info);
        }
    }
}
