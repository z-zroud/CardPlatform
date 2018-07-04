using CardPlatform.Common;
using GalaSoft.MvvmLight;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TransResultModel : ObservableObject
    {
        public TransResultModel(TransType type, TransResult result)
        {
            Result = result;
            TransType = type;
        }

        private TransResult _result;
        public TransResult Result
        {
            get { return _result; }
            set
            {
                Set(ref _result, value);
                switch(_result)
                {
                    case TransResult.Failed:
                        ResultDescripton = "失败"; ColorMark = new SolidColorBrush(Colors.Red); break;
                    case TransResult.Sucess:
                        ResultDescripton = "成功"; ColorMark = new SolidColorBrush(Colors.Black); break;
                    case TransResult.Unknown:
                        ResultDescripton= "未执行"; ColorMark = new SolidColorBrush(Colors.Yellow); break;
                }
            }
        }

        private TransType _transType;
        public TransType TransType
        {
            get { return _transType; }
            set
            {
                Set(ref _transType, value);
                switch(_transType)
                {
                    case TransType.PBOC_DES: TransTypeDescription = "PBOC 国际";break;
                    case TransType.PBOC_SM: TransTypeDescription = "PBOC 国密"; break;
                    case TransType.UICS_DES: TransTypeDescription = "UICS 国际"; break;
                    case TransType.UICS_SM: TransTypeDescription = "UICS 国密"; break;
                    case TransType.ECC_DES: TransTypeDescription = "ECC 国际"; break;
                    case TransType.ECC_SM: TransTypeDescription = "ECC 国密"; break;
                    case TransType.QPBOC_DES: TransTypeDescription = "QPBOC 国际"; break;
                    case TransType.QPBOC_SM: TransTypeDescription = "QPBOC 国密"; break;
                }
            }
        }

        private string _transTypeDescription;
        public string TransTypeDescription
        {
            get { return _transTypeDescription; }
            set
            {
                Set(ref _transTypeDescription, value);
            }
        }

        private string _resultDescription;
        public string ResultDescripton
        {
            get { return _resultDescription; }
            set
            {
                Set(ref _resultDescription, value);
            }
        }

        private SolidColorBrush _colorMark;
        public SolidColorBrush ColorMark
        {
            get { return _colorMark; }
            set
            {
                Set(ref _colorMark, value);
            }
        }
    }
}
