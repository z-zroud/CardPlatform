using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardPlatform.Common
{
    public class TVR
    {
        private Int64 tvr = 0;

        private static TVR tvrObj;
        public static TVR GetInstance()
        {
            if (tvrObj == null)
            {
                tvrObj = new TVR();
            }
            return tvrObj;
        }

        public void Clear()
        {
            tvr = 0;
        }

        /// <summary>
        /// 设置是否执行了脱机数据认证流程
        /// </summary>
        /// <param name="isPerformed"></param>
        public void SetPerformOfflineDataAuthentication(bool isPerformed)
        {
            if(!isPerformed)
            {
                tvr = tvr & 0x8000000000;
            }
        }


        /// <summary>
        /// 设置SDA执行结果
        /// </summary>
        /// <param name="isSucess"></param>
        public void SetSDAResult(bool isSucess)
        {
            if(!isSucess)
            {
                tvr = tvr & 0x4000000000;
            }
        }

        /// <summary>
        /// 设置DDA执行结果
        /// </summary>
        /// <param name="isSucess"></param>
        public void SetDDAResult(bool isSucess)
        {
            if(!isSucess)
            {
                tvr = tvr & 0x0800000000;
            }
        }

        /// <summary>
        /// 设置CDA执行结果
        /// </summary>
        /// <param name="isSucess"></param>
        public void SetCDAResult(bool isSucess)
        {
            if(!isSucess)
            {
                tvr = tvr & 0x0400000000;
            }
        }

        /// <summary>
        /// 设置应用是否失效
        /// </summary>
        /// <param name="isExpired"></param>
        public void SetApplicationIsExpired(bool isExpired)
        {
            if(isExpired)
            {
                tvr = tvr & 0x0040000000;
            }
        }

        /// <summary>
        /// 设置应用是否生效
        /// </summary>
        /// <param name="isEffective"></param>
        public void SetApplicationIsEffective(bool isEffective)
        {
            if(!isEffective)
            {
                tvr = tvr & 0x0020000000;
            }
        }

        /// <summary>
        /// 设置是否为新卡
        /// </summary>
        /// <param name="isNewCard"></param>
        public void SetIsNewCard(bool isNewCard)
        {
            if(isNewCard)
            {
                tvr = tvr & 0x0008000000;
            } 
        }

        /// <summary>
        /// 设置持卡人验证是否成功
        /// </summary>
        /// <param name="isSucess"></param>
        public void SetCardholderVerification(bool isSucess)
        {
            if(!isSucess)
            {
                tvr = tvr & 0x0000800000;
            }
        }

        /// <summary>
        /// 设置脱机PIN尝试次数是否超过限制
        /// </summary>
        /// <param name="isExceeded"></param>
        public void SetPinTryExceeded(bool isExceeded)
        {
            if(isExceeded)
            {
                tvr = tvr & 0x0000200000;
            }
        }

        /// <summary>
        /// 设置交易金额是否低于最低限额
        /// </summary>
        /// <param name="isExceeded"></param>
        public void SetTransactionExceedFloorLimit(bool isExceeded)
        {
            if(isExceeded)
            {
                tvr = tvr & 0x0000008000;
            }
        }

        public void SetTranscationExceedLowerConsecutiveOfflineLimit(bool isExceeded)
        {
            if(isExceeded)
            {
                tvr = tvr & 0x0000004000;
            }
        }

        public void SetTranscationExceedUpperConsecutiveOfflineLimit(bool isExceeded)
        {
            if(isExceeded)
            {
                tvr = tvr & 0x0000002000;
            }
        }
    }
}
