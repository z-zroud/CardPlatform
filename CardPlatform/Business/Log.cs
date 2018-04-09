using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Business
{
    public delegate void LogHandle(LogEntity entity);

    public enum LogLevel
    {
        Info = 0,
        Warning = 1,
        Error = 2
    }

    public class LogEntity
    {
        public LogLevel Level { get; set; }
        public string Title { get; set; }
        public string Info { get; set; }
    }

    public class Log
    {
        public event LogHandle LogRecord;

        public void ShowLog(LogLevel level, string title, string info)
        {
            LogEntity entity = new LogEntity();
            entity.Level = level;
            entity.Title = title;
            entity.Info = info;

            LogRecord?.Invoke(entity);
        }
    }
}
