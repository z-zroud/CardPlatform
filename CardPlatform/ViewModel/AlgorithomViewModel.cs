using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using UtilLib;
using CardPlatform.Models;

namespace CardPlatform.ViewModel
{
    public class AlgorithomViewModel : ViewModelBase
    {
        public AlgorithomViewModel()
        {
            EncryptType = new List<string>();
            DecryptType = new List<string>();
            KMCs = new List<string>();
            DivType = new List<string>();
            Secure = new List<string>();

            Load();
        }

        #region DataBinding
        /// <summary>
        /// 对称算法 输入
        /// </summary>
        private string _inputData;
        public string InputData
        {
            get { return _inputData; }
            set
            {
                Set(ref _inputData, value);
            }
        }

        /// <summary>
        /// 对称算法 输出
        /// </summary>
        private string _outputData;
        public string OutputData
        {
            get { return _outputData; }
            set
            {
                Set(ref _outputData, value);
            }
        }

        /// <summary>
        /// 对称算法 加解密方式
        /// </summary>
        private List<string> _encryptType;
        public List<string> EncryptType
        {
            get { return _encryptType; }
            set
            {
                Set(ref _encryptType, value);
            }
        }

        /// <summary>
        /// 对称算法 被选中的加解密方式
        /// </summary>
        private int _selectedEncryptType;
        public int SelectedEncryptType
        {
            get { return _selectedEncryptType; }
            set
            {
                Set(ref _selectedEncryptType, value);
            }
        }

        /// <summary>
        /// 对称算法密钥
        /// </summary>
        private string _algorithmKey;
        public string AlgorithmKey
        {
            get { return _algorithmKey; }
            set
            {
                Set(ref _algorithmKey, value);
            }
        }

        /// <summary>
        /// 加密的文件的路径
        /// </summary>
        private string _encrytpedFilePath;
        public string EncryptFilePath
        {
            get { return _encrytpedFilePath; }
            set
            {
                Set(ref _encrytpedFilePath, value);
            }
        }

        /// <summary>
        /// 文件解密的方式
        /// </summary>
        private List<string> _decryptType;
        public List<string> DecryptType
        {
            get { return _decryptType; }
            set
            {
                Set(ref _decryptType, value);
            }
        }

        /// <summary>
        /// 文件被选中的解密方式
        /// </summary>
        private int _selectedDecryptType;
        public int SelectedDecryptType
        {
            get { return _selectedDecryptType; }
            set
            {
                Set(ref _selectedDecryptType, value);
            }
        }

        /// <summary>
        /// 文件的解密密钥
        /// </summary>
        private string _fileKey;
        public string FileKey
        {
            get { return _fileKey; }
            set
            {
                Set(ref _fileKey, value);
            }
        }

        /// <summary>
        /// KMC列表
        /// </summary>
        private List<string> _kmcs;
        public List<string> KMCs
        {
            get { return _kmcs; }
            set
            {
                Set(ref _kmcs, value);
            }
        }

        /// <summary>
        /// 选中的KMC
        /// </summary>
        private string _selectedKMC;
        public string SelectedKMC
        {
            get { return _selectedKMC; }
            set
            {
                Set(ref _selectedKMC, value);
            }
        }

        /// <summary>
        /// KMC分散方式列表
        /// </summary>
        private List<string> _divType;
        public List<string> DivType
        {
            get { return _divType; }
            set
            {
                Set(ref _divType, value);
            }
        }

        /// <summary>
        /// 被选中的分散方式
        /// </summary>
        private int _divTypeIndex;
        public int DivTypeIndex
        {
            get { return _divTypeIndex; }
            set
            {
                Set(ref _divTypeIndex, value);
            }
        }

        /// <summary>
        /// 打开安全通道的安全等级
        /// </summary>
        private List<string> _secure;
        public List<string> Secure
        {
            get { return _secure; }
            set
            {
                Set(ref _secure, value);
            }
        }

        /// <summary>
        /// 被选中的安全等级
        /// </summary>
        private int _secureIndex;
        public int SecureIndex
        {
            get { return _secureIndex; }
            set
            {
                Set(ref _secureIndex, value);
            }
        }

        /// <summary>
        /// 分散数据
        /// </summary>
        private string _divData;
        public string DivData
        {
            get { return _divData; }
            set
            {
                Set(ref _divData, value);
            }
        }

        /// <summary>
        /// 随机数
        /// </summary>
        private string _random;
        public string Random
        {
            get { return _random; }
            set
            {
                Set(ref _random, value);
            }
        }
        #endregion

        #region Command Binding
        private ICommand _algorithmComfirmCmd;
        public ICommand AlgorithmComfirmCmd
        {
            get
            {
                if (_algorithmComfirmCmd == null)
                    _algorithmComfirmCmd = new RelayCommand(DoAlgorithm);
                return _algorithmComfirmCmd;
            }
        }

        private ICommand _openEncryptFilePath;
        public ICommand OpenEncryptFilePath
        {
            get
            {
                if (_openEncryptFilePath == null)
                    _openEncryptFilePath = new RelayCommand(OpenEncryptFile);
                return _openEncryptFilePath;
            }
        }

        private ICommand _decryptFileComfirmCmd;
        public ICommand DecryptFileComfirmCmd
        {
            get
            {
                if (_decryptFileComfirmCmd == null)
                    _decryptFileComfirmCmd = new RelayCommand(DoDecryptFile);
                return _decryptFileComfirmCmd;
            }
        }

        private ICommand _kmcDivComfirmCmd;
        public ICommand KmcDivComfirmCmd
        {
            get
            {
                if (_kmcDivComfirmCmd == null)
                    _kmcDivComfirmCmd = new RelayCommand(DoKmcDiv);
                return _kmcDivComfirmCmd;
            }
        }
        #endregion

        private void DoAlgorithm()
        {

        }

        private void DoDecryptFile()
        {

        }

        private void OpenEncryptFile()
        {

        }

        private void DoKmcDiv()
        {

        }

        private void Load()
        {
            ISerialize serialize = new XmlSerialize();
            var algorithmCfg = (AlgorithmConfig)serialize.DeserizlizeFromFile("AlgorithmConfiguration.xml", typeof(AlgorithmConfig));
            EncryptType = algorithmCfg.AlgorithmType;
            if (EncryptType.Count > 0)
                SelectedEncryptType = 0;
            DecryptType = algorithmCfg.FileDecryptType;
            if (DecryptType.Count > 0)
                SelectedDecryptType = 0;
            KMCs = algorithmCfg.KMCs;
            if (KMCs.Count > 0)
                SelectedKMC = KMCs.First();
            DivType = algorithmCfg.DivType;
            if (DivType.Count > 0)
                DivTypeIndex = 0;
            Secure = algorithmCfg.Secure;
            if (Secure.Count > 0)
                SecureIndex = 0;
        }
    }
}
