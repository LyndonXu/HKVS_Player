// DlgShow.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "DlgShow.h"
#include "afxdialogex.h"
#include "PlayerDlg.h"


static UINT s_uIndicators[_StatusBar_Reserved] =
{
	_StatusBar_CPU,
	_StatusBar_Memory,
	_StatusBar_NetInterface,
	_StatusBar_ErrorMessage,
};

// CDlgShow 对话框

IMPLEMENT_DYNAMIC(CDlgShow, CDialogEx)

CDlgShow::CDlgShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_Show, pParent)
	, m_pDlgPlayCtrl(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CDlgShow::~CDlgShow()
{
	if (m_pDlgPlayCtrl != NULL)
	{
		delete m_pDlgPlayCtrl;
	}
}

void CDlgShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShow, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_Set, &CDlgShow::OnBnClickedBtnSet)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, &CDlgShow::OnToolTipNotify)
	ON_WM_SYSCOMMAND()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()


// CDlgShow 消息处理程序


void CDlgShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CWnd *pMovie = GetDlgItem(IDC_STATIC_BigMovie);
	if (pMovie != NULL && pMovie->GetSafeHwnd() != NULL)
	{
		CRect csClient;
		GetWindowRect(&csClient);
		csClient.left += 8;
		csClient.top += 32;
		csClient.right -= (8 + 80);
		csClient.bottom -= (8 + 20);

		ScreenToClient(&csClient);

		pMovie->MoveWindow(&csClient);
		pMovie->Invalidate();
	}

	CWnd *pBtn = GetDlgItem(IDC_BTN_Set);
	if (pBtn != NULL && pBtn->GetSafeHwnd() != NULL)
	{
		CRect csClient;
		GetWindowRect(&csClient);
		csClient.top += 32;
		csClient.right -= 8;

		csClient.left = csClient.right - 80;
		csClient.bottom = csClient.top + 30;

		ScreenToClient(&csClient);

		pBtn->MoveWindow(&csClient);
		pBtn->Invalidate();

	}
	pBtn = GetDlgItem(IDOK);
	if (pBtn != NULL && pBtn->GetSafeHwnd() != NULL)
	{
		CRect csClient;
		GetWindowRect(&csClient);

		csClient.right -= 8;
		csClient.bottom -= (8 + 20);

		csClient.left = csClient.right - 80;
		csClient.top = csClient.bottom - 30;

		ScreenToClient(&csClient);

		pBtn->MoveWindow(&csClient);
		pBtn->Invalidate();

	}

	if (m_csStatusBar.GetSafeHwnd() != NULL)
	{
		CRect csRect;
		GetClientRect(csRect);
		m_csStatusBar.MoveWindow(0, csRect.bottom - 20, csRect.right, 20);
	}


	// TODO: 在此处添加消息处理程序代码
}


BOOL CDlgShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

#if 1
	int s32Width = GetSystemMetrics(SM_CXSCREEN);
	//int s32Height = GetSystemMetrics(SM_CYSCREEN);
	int s32Left = (s32Width - 600) / 2;
	int s32Top = 50;
	CRect csClient(s32Left, s32Top, s32Left + 600, s32Top + 338);

	::SetWindowPos(GetSafeHwnd(), NULL,//HWND_TOPMOST, 
		csClient.left, csClient.top,
		csClient.Width(), csClient.Height(),
		SWP_SHOWWINDOW);

	GetDlgItem(IDC_STATIC_BigMovie)->GetWindowRect(csClient);
#endif

	ShowWindow(SW_MAXIMIZE);

	{
		CRect csRect;

		GetClientRect(csRect);

		m_csStatusBar.Create(this);

		m_csStatusBar.SetIndicators(s_uIndicators, sizeof(s_uIndicators) / sizeof(UINT));

		m_csStatusBar.MoveWindow(0, csRect.bottom - 20, csRect.right, 20);
		m_csStatusBar.SetPaneInfo(_StatusBar_CPU,
			s_uIndicators[_StatusBar_CPU], SBPS_NORMAL, 60);
		m_csStatusBar.SetPaneInfo(_StatusBar_Memory,
			s_uIndicators[_StatusBar_Memory], SBPS_NORMAL, 60);
		m_csStatusBar.SetPaneInfo(_StatusBar_NetInterface,
			s_uIndicators[_StatusBar_NetInterface], SBPS_NORMAL, 200);

		m_csStatusBar.SetPaneInfo(_StatusBar_ErrorMessage,
			s_uIndicators[_StatusBar_ErrorMessage], SBPS_NORMAL, 200);

#if 0
		m_csStatusBar.SetPaneText(_StatusBar_CPU, L"1", TRUE);
		m_csStatusBar.SetPaneText(_StatusBar_Memory, L"2", TRUE);
		m_csStatusBar.SetPaneText(_StatusBar_NetInterface, L"3", TRUE);
		m_csStatusBar.SetPaneText(_StatusBar_ErrorMessage, L"4", TRUE);
#endif

	}


	{
		m_pDlgPlayCtrl = (CDialogEx *)(new CPlayerDlg);
		if (m_pDlgPlayCtrl != NULL)
		{
			((CPlayerDlg *)m_pDlgPlayCtrl)->SetShowDlg(this);
			m_pDlgPlayCtrl->Create(IDD_PLAYER_DIALOG, GetDesktopWindow());
			m_pDlgPlayCtrl->ShowWindow(SW_HIDE);
		}
	}

	EnableToolTips();
#if 1
	{
		
		m_csToolTips.Create(this);
		m_csToolTips.Activate(TRUE);
		RECT stRect = { 0, -32, 32, 0 };
		
		m_csToolTips.AddTool(this, L"天津市北洋天泽智能机器人科技有限公司", &stRect, IDD_DLG_Show);
	}
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgShow::OnBnClickedBtnSet()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pDlgPlayCtrl->ShowWindow(SW_HIDE);
	m_pDlgPlayCtrl->ShowWindow(SW_SHOW);
	ShowWindow(SW_NORMAL);

}
BOOL CDlgShow::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;

	UINT  uID = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		uID = ::GetDlgCtrlID((HWND)uID);
	}

	if (uID == 0)
	{
		return FALSE;
	}
	//else if (uID == IDC_BTN_Set)
	//{
	//	pTTT->lpszText = L"test";
	//	uID = uID;
	//}
	return   TRUE;
}

void CDlgShow::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nID == SC_MOVE || nID == 0xF012)
	{
		return;
	}
	CDialogEx::OnSysCommand(nID, lParam);
}


void CDlgShow::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CDialogEx::OnNcLButtonDblClk(nHitTest, point);
}


BOOL CDlgShow::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_csToolTips.GetSafeHwnd() != NULL)
	{
		m_csToolTips.RelayEvent(pMsg);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

