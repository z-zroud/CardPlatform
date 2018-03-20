using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CplusplusDll
{
    public interface ICPS
    {
        List<string> GenCpsFile(string dllName, string funcName, string dpFile, string ruleFile);
        void SetPersonlizationConfig(string isd, string kmc, int divMethod, int secureLevel);
        bool DoPersonlization(string cpsFile, string iniConfigFile);
    }
}
