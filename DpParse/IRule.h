#pragma once
#include <string>
#include<vector>

/**************************************************************
* 定义标准CPS格式,任何需要生成CPS标准文件的DP数据都需要转换为此
* 结构体形式
***************************************************************/
struct CPS
{
    std::string fileName;    //CPS文件名称
    std::string aid;         //AID 借记 or 贷记
    std::vector<DGI> dgis;    //DGI分组集合
    void AddDGI(DGI item);
};

struct IRule 
{
    virtual bool SetRuleCfg(const char* szRuleConfig) = 0;
    virtual void HandleRule(CPS& cps) = 0;
};