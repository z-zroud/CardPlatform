#include "StdAfx.h"
#include "ModifyKmc.h"
#include "Interface\InterfaceInstance.h"

CModifyKmc::CModifyKmc(string readerName)
{
    m_readerName = readerName;
}


bool CModifyKmc::StartModifyKMC(const string &aid, const string &oldKmc, const string &newKmc, DIV_METHOD_FLAG flag)
{
    IPCSC* pPCSC = GetPCSCInterface();
    if (!pPCSC->OpenReader(m_readerName.c_str()))
    {
        return false;
    }
    if (!pPCSC->SelectAID(aid))
    {
        return false;
    }
    if (!pPCSC->OpenSecureChannel(oldKmc))
    {
        return false;
    }
    if (pPCSC->SetKMC(newKmc, flag)) {
        return true;
    }

    return false;
}