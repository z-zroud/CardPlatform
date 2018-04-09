﻿using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace CardPlatform.Models
{
    public class TransInfoModel : ObservableObject
    {
        public TransInfoModel()
        {
            ColorMark = new SolidColorBrush(Colors.Black);
        }

        private string _caseNo;
        public string CaseNo
        {
            get { return _caseNo; }
            set
            {
                Set(ref _caseNo, value);
            }
        }

        private string _caseInfo;
        public string CaseInfo
        {
            get { return _caseInfo; }
            set
            {
                Set(ref _caseInfo, value);
            }
        }

        private string _caseLevel;
        public string CaseLevel
        {
            get { return _caseLevel; }
            set
            {
                Set(ref _caseLevel, value);
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