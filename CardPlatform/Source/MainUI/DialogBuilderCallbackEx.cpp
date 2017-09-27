#include "StdAfx.h"
#include "IDialogBuilderCallbackEx.h"
#include "TreeViewNav.h"
#include "UIModifyKmc.h"
#include "UIPersonaliztion.h"
#include "UICardCheck.h"
#include "UIYLConverter.h"
#include "UIHTConverter.h"
#include "UIGEConverter.h"
#include "UIYLConfig.h"
#include "UIPersonalizationConfig.h"
#include "UIDecryptFile.h"
#include "UIECLoad.h"

#define DUI_CTR_MODIFY_KMC_UI       _T("ModifyKmcUI")
#define DUI_CTR_TREE_VIEW_NAV       _T("TreeViewNav")
#define DUI_CTR_PERSONALIZATION_UI	_T("PersonalizationUI")
#define DUI_CTR_CARD_CHECK_UI       _T("CardCheckUI")
#define DUI_CTR_YL_DO_CONVERT_UI    _T("YLConverterUI")
#define DUI_CTR_HT_DO_CONVERT_UI    _T("HTConverterUI")
#define DUI_CTR_GE_DO_CONVERT_UI    _T("GEConverterUI")
#define DUI_CTR_YL_CONFIG_UI        _T("YLConfigUI")
#define DUI_CTR_PERSO_CONFIG_UI     _T("PersoConfigUI")
#define DUI_CTR_FILE_DECRYPT_UI     _T("FileDecryptUI")
#define DUI_CTR_EC_LOAD_CRIYPT_UI   _T("ECLoadScriptUI")

CDialogBuilderCallbackEx::CDialogBuilderCallbackEx(CPaintManagerUI *pPM)
{
	m_pPM = pPM;
}

CControlUI*	CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, DUI_CTR_TREE_VIEW_NAV) == 0) return new CTreeViewNav(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_MODIFY_KMC_UI) == 0) return new CModifyKmcUI(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_PERSONALIZATION_UI) == 0) return new CPersonalizationUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_CARD_CHECK_UI) == 0) return new CCardCheckUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_YL_DO_CONVERT_UI) == 0) return new CYLConverterUI(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_HT_DO_CONVERT_UI) == 0) return new CHTConverterUI(m_pPM);
	else if (_tcscmp(pstrClass, DUI_CTR_GE_DO_CONVERT_UI) == 0) return new CGEConverterUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_YL_CONFIG_UI) == 0) return new CYLConfigUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_PERSO_CONFIG_UI) == 0) return new CPersonalizationConfigUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_FILE_DECRYPT_UI) == 0) return new CDecryptFileUI(m_pPM);
    else if (_tcscmp(pstrClass, DUI_CTR_EC_LOAD_CRIYPT_UI) == 0) return new CECLoadUI(m_pPM);
	return NULL;
}