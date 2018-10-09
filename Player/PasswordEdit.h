#pragma once


// CPasswordEdit

class CPasswordEdit : public CEdit
{
	DECLARE_DYNAMIC(CPasswordEdit)

public:
	CPasswordEdit();
	virtual ~CPasswordEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CBrush m_csBrushBackGnd;
	COLORREF m_dwTextColor;
	CString m_csInitString;
	CString m_csPassword;

	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


