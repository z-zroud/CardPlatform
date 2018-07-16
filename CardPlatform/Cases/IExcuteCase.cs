using System;
using CardPlatform.Config;
using CardPlatform.Common;

namespace CardPlatform.Cases
{
    public interface IExcuteCase
    {
        /// <summary>
        /// 根据APDU返回指令执行case
        /// </summary>
        /// <param name="batchNo">第一次跑app交易，为了测试case,需要重复跑几次</param>
        /// <param name="app">应用类型</param>
        /// <param name="step">交易步骤</param>
        /// <param name="srcData">case传入的数据,一般为ApduRespone对象</param>
        void Excute(int batchNo, TransactionApp app, TransactionStep step, Object srcData);

        /// <summary>
        /// 输出当前case执行情况
        /// </summary>
        /// <param name="level"></param>
        /// <param name="caseNo"></param>
        /// <param name="format"></param>
        /// <param name="args"></param>
        void TraceInfo(TipLevel level, string caseNo, string format, params object[] args);
    }
}
