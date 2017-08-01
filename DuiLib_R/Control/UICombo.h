#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    CDuiString GetText() const;
    void SetEnabled(bool bEnable = true);

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);

    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    bool Activate();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);

	bool GetFillPart()const;			//是否将控件划分为两部分考虑，右边部分可自定义背景图，而不用整个控件贴背景图
	void SetFillPart(bool bFillPart);

	DWORD GetFillPartWidth()const;		//设置右半部分的宽度
	void SetFillPartWidth(DWORD dwWidth);

    TListInfoUI* GetListInfo();
    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
	DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

	CListLabelElementUI* GetCurSelected();	//获取当前选中控件
	CDuiString GetCurItemString();	//获取当前选中控件字符串

protected:
    CComboWnd* m_pWindow;

    int m_iCurSel;
    RECT m_rcTextPadding;
    CDuiString m_sDropBoxAttributes;
    SIZE m_szDropBox;
    UINT m_uButtonState;

    TListInfoUI m_ListInfo;

	DWORD m_dwHotBkColor;
	bool m_bFillPart;	//是否填充控件部分位置
	DWORD m_dwFillPartWidth;	//填空控件部分位置的宽度
};

} // namespace DuiLib

#endif // __UICOMBO_H__
