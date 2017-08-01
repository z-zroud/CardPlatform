#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	CCheckBoxUI::CCheckBoxUI()
	{
		//默认bk hotbk pushedbk颜色一致
		SetHotBkColor(GetBkColor());
		SetPushedBkColor(GetBkColor());

		SetBorderSize({ 0,0,0,0 });	//无边框
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
