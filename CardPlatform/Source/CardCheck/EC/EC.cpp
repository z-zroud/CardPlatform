#include "StdAfx.h"
#include "EC.h"


EC::EC(IPCSC* pReader) : PBOC(COMM_TOUCH, pReader)
{

}

void EC::ReadECRelativeTags()
{
    vector<string> vecTags = { Tag9F79, Tag9F6D, Tag9F51,Tag9F52, Tag9F53, Tag9F54,Tag9F55,Tag9F56,
    Tag9F57,Tag9F58,Tag9F59,Tag9F5C,Tag9F5D,Tag9F72,Tag9F73,Tag9F75,Tag9F76,Tag9F4F,TagDF4F,Tag9F17,
    Tag9F79,Tag9F77,Tag9F78,TagDF62,TagDF63,TagDF77,TagDF78,TagDF79,TagDF71,TagDF76,TagDF72,Tag9F6D,
    Tag9F36,Tag9F13,Tag9F68,Tag9F6B,TagDF20,TagDF21,TagDF23,TagDF25,TagDF26,TagDF27,TagDF50 };

    for (auto tag : vecTags)
    {
        string value = ReadTagValueFromCard(tag);
        m_tags.insert(pair<string,string>(tag, value));
    }
}

void EC::ReadECRelativeTagsAfterGAC()
{
    vector<string> vecTags = { Tag9F5D, Tag9F77, Tag9F78,Tag9F79};

    for (auto tag : vecTags)
    {
        string value = ReadTagValueFromCard(tag);
        m_tags.insert(pair<string, string>(tag, value));
    }
}


void EC::DoTrans()
{
    SelectApplication();        //第一步： 应用选择
    InitilizeApplication();     //第二步： 应用初始化
    ReadApplicationData();      //第三步： 读应用数据

    //需要判断EC发卡行授权码是否存在
    if (GetTagValue(Tag9F74).empty())
    {//如果不存在，执行PBOC流程

    }
    else {
        //在进行下一步流程时，需要读取卡片中与EC相关的TAG值
        ReadECRelativeTags();
        OfflineDataAuth();          //第四步： 脱机数据认证
        HandleLimitation();         //第五步： 处理限制
        CardHolderValidation();     //第六步： 持卡人验证
        TerminalRiskManagement();   //第七步： 终端风险管理  
        TerminalActionAnalized();   //第八步： 终端行为分析
        CardActionAnalized();       //第九步： 卡片行为分析
        //OnlineBussiness();          //第十步： 发卡行认证, 电子现金不需要发卡行认证
        EndTransaction();           //第十一步： 交易结束处理
    }
}