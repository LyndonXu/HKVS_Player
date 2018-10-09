// PasswordEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "PasswordEdit.h"


// CPasswordEdit

IMPLEMENT_DYNAMIC(CPasswordEdit, CEdit)

CPasswordEdit::CPasswordEdit()
{
	m_dwTextColor = RGB(0x70, 0x70, 0x70);
	m_csBrushBackGnd.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

CPasswordEdit::~CPasswordEdit()
{
}


BEGIN_MESSAGE_MAP(CPasswordEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CPasswordEdit 消息处理程序

HBRUSH CPasswordEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  在此更改 DC 的任何特性
	COLORREF c = pDC->SetTextColor(/*RGB(255, 0, 0)*/m_dwTextColor);
	c = c;
	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	return m_csBrushBackGnd;
}


void CPasswordEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	


	CEdit::OnChar(nChar, nRepCnt, nFlags);

	GetWindowText(m_csPassword);
	m_csPassword = m_csPassword;
}


void CPasswordEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码

	if (m_csPassword == L"")
	{
		SetWindowText(L"");
		m_dwTextColor = RGB(0, 0, 0);
		SetPasswordChar(L'*');
	}
}


void CPasswordEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码
	if (m_csPassword == L"")
	{
		SetWindowText(m_csInitString);
		m_dwTextColor = RGB(0x70, 0x70, 0x70);
		SetPasswordChar(0);
		Invalidate();
	}
}

