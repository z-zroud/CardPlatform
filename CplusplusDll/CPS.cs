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

        public List<string> GenCpsFile(string dllName, string funcName, string dpFile, string ruleFile)
        {
            var cpsFiles = new List<string>();
            if (CDll.GenCpsFile(dllName, funcName, dpFile, ruleFile))
            {                
                int count = 128;
                IntPtr[] intPtrs = new IntPtr[count];
                for (int i = 0; i < count; i++)
                {
                    intPtrs[i] = new IntPtr();
                }
                CDll.GetCpsFiles(intPtrs, ref count);
                for (int j = 0; j < count; j++)
                {
                    string cpsFile = Marshal.PtrToStringAnsi(intPtrs[j]);
                    cpsFiles.Add(cpsFile);
                }
            }
            return cpsFiles;
        }

        public void SetPersonlizationConfig(string isd, string kmc, int divMethod, int secureLevel)
        {
            CDll.SetPersonlizationConfig(isd, kmc, divMethod, secureLevel);
        }
    }
}
