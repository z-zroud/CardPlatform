﻿using System;
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
            if(handle != 0x1000000)
            {
                _log = new Log();
            }
            return _log;
        }

        public void TraceLog(LogLevel level, string format, params object[] args)
        {
            string info = string.Format(format, args);
            info = level.ToString() + ":" + info;
            CDll.TraceLog(handle, info);
        }
    }
}
