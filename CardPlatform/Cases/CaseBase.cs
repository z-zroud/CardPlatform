using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CardPlatform.Business;
using CardPlatform.ViewModel;
using CardPlatform.Models;
using UtilLib;
using System.Windows.Media;

namespace CardPlatform.Cases
{
    public class CaseBase : IExcuteCase
    {
        private static readonly Dictionary<string, string> CaseDict = new Dictionary<string, string>();
        private static bool isLoaded = false;

        public CaseBase()
        {
            if (!isLoaded)
            {
                Load();
                isLoaded = true;
            }
        }

        public virtual void ExcuteCase(object srcData, CardRespDataType type)
        {

        }

        public virtual void ShowComparedResult(string tag)
        {

        }

        public void ShowInfo(string caseNo,string description, CaseLevel level)
        {
            string caseLevel = string.Empty;
            TransInfoModel caseInfo = new TransInfoModel();

            if (level == CaseLevel.CaseSucess) { caseInfo.ColorMark = new SolidColorBrush(Colors.Black); caseLevel = "成功"; }
            else if (level == CaseLevel.CaseWarn) { caseInfo.ColorMark = new SolidColorBrush(Colors.Yellow); caseLevel = "警告"; }
            else if (level == CaseLevel.CaseFailed) { caseInfo.ColorMark = new SolidColorBrush(Colors.Red); caseLevel = "失败"; }

            caseInfo.CaseNo = caseNo;
            caseInfo.CaseInfo = description;
            caseInfo.CaseLevel = caseLevel;

            ViewModelLocator locator = new ViewModelLocator();
            locator.Transaction.CaseInfos.Add(caseInfo);
        }

        public string GetDescription(string caseNo)
        {
            string desc;
            if(CaseDict.TryGetValue(caseNo,out desc))
            {
                return desc;
            }
            return string.Empty;
        }

        private void Load()
        {
            ISerialize serialize = new XmlSerialize();
            //CaseConfig cfg = new CaseConfig();
            //CaseItem item1 = new CaseItem();
            //item1.CaseDescription = "11";
            //item1.CaseNo = "11";
            //CaseItem item2 = new CaseItem();
            //item2.CaseDescription = "11";
            //item2.CaseNo = "11";
            //cfg.CaseList.Add(item1);
            //cfg.CaseList.Add(item2);
            //serialize.Serialize(cfg, "D:\\Text.xml");
            var caseConfig = (CaseConfig)serialize.DeserizlizeFromFile("CaseConfiguration.xml", typeof(CaseConfig));

            foreach(var caseItem in caseConfig.CaseList)
            {
                CaseDict.Add(caseItem.CaseNo, caseItem.CaseDescription);
            }
           
        }
    }
}
