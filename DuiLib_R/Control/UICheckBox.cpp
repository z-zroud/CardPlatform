#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	CCheckBoxUI::CCheckBoxUI()
	{
		////Ä¬ÈÏbk hotbk pushedbkÑÕÉ«Ò»ÖÂ
		//SetHotBkColor(GetBkColor());
		//SetPushedBkColor(GetBkColor());

        //SetBorderSize(0);
        m_dwBackColor = 0xFFFFFFFF;
        m_dwBackColor2 = 0xFFFFFFFF;
        m_dwBackColor3 = 0xFFFFFFFF;
        m_dwHotBkColor = 0xFFFFFFFF;
        m_dwPushedBkColor = 0xFFFFFFFF;

        m_nBorderSize = 0;
	}

	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}

	void CCheckBoxUI::SetCheck(bool bCheck)
	{
		Selected(bCheck);
	}

	bool  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}
}
