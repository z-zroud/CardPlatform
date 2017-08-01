#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	CCheckBoxUI::CCheckBoxUI()
	{
		//Ĭ��bk hotbk pushedbk��ɫһ��
		SetHotBkColor(GetBkColor());
		SetPushedBkColor(GetBkColor());

		SetBorderSize({ 0,0,0,0 });	//�ޱ߿�
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
