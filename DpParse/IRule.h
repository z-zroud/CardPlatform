#pragma once
#include <string>
#include<vector>

/**************************************************************
* �����׼CPS��ʽ,�κ���Ҫ����CPS��׼�ļ���DP���ݶ���Ҫת��Ϊ��
* �ṹ����ʽ
***************************************************************/
struct CPS
{
    std::string fileName;    //CPS�ļ�����
    std::string aid;         //AID ��� or ����
    std::vector<DGI> dgis;    //DGI���鼯��
    void AddDGI(DGI item);
};

struct IRule 
{
    virtual bool SetRuleCfg(const char* szRuleConfig) = 0;
    virtual void HandleRule(CPS& cps) = 0;
};