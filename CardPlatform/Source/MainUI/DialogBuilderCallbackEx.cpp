#include "StdAfx.h"
#include "IDialogBuilderCallbackEx.h"
#include "TreeViewNav.h"
#include "ModifyKmcUI.h"
#include "PersonaliztionUI.h"

#define DUI_CTR_MODIFY_KMC_UI	_T("ModifyKmcUI")
#define DUI_CTR_TREE_VIEW_NAV	_T("TreeViewNav")
#define DUI_CTR_PERSONALIZATION_UI	_T("PersonalizationUI")

CDialogBuilderCallbackEx::CDialogBuilderCallbackEx(CPaintManagerUI *pPM)
{
	m_pPM = pPM;
}

CControlUI*	CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, DUI_CTR_TREE_VIEW_NAV) == 0) return new CTreeViewNav(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_MODIFY_KMC_UI) == 0) return new CModifyKmcUI(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_PERSONALIZATION_UI) == 0) return new CPersonalizationUI(m_pPM);

	return NULL;
}