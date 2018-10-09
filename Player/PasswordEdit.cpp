// PasswordEdit.cpp : ʵ���ļ�
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



// CPasswordEdit ��Ϣ�������

HBRUSH CPasswordEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  �ڴ˸��� DC ���κ�����
	COLORREF c = pDC->SetTextColor(/*RGB(255, 0, 0)*/m_dwTextColor);
	c = c;
	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	return m_csBrushBackGnd;
}


void CPasswordEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	


	CEdit::OnChar(nChar, nRepCnt, nFlags);

	GetWindowText(m_csPassword);
	m_csPassword = m_csPassword;
}


void CPasswordEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// TODO: �ڴ˴������Ϣ����������

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

	// TODO: �ڴ˴������Ϣ����������
	if (m_csPassword == L"")
	{
		SetWindowText(m_csInitString);
		m_dwTextColor = RGB(0x70, 0x70, 0x70);
		SetPasswordChar(0);
		Invalidate();
	}
}

