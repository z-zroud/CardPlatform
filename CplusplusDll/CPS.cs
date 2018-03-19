using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CplusplusDll
{
    public class CPS : ICPS
    {
        public bool DoPersonlization(string cpsFile, string iniConfigFile)
        {
            return CDll.DoPersonlization(cpsFile, iniConfigFile);
        }

        public bool GenCpsFile(string dllName, string funcName, string dpFile, string ruleFile)
        {
            return CDll.GenCpsFile(dllName, funcName, dpFile, ruleFile);
        }

        public void SetPersonlizationConfig(string isd, string kmc, int divMethod, int secureLevel)
        {
            CDll.SetPersonlizationConfig(isd, kmc, divMethod, secureLevel);
        }
    }
}
