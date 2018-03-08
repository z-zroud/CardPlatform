using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CplusplusDll
{
    public interface ICDll
    {
        List<string> GetReaders();
        bool OpenReader(string reader);
        string GetATR();
        bool WarmReset();
        bool ColdReset();
        void CloseReader();
        int GetCardStatus();
        int GetTransProtocol();
        int SendApdu(string cmd, ref string result);
        int SendApdu(string cmd);
    }
}
