// DlgShow.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Player.h"
#include "DlgShow.h"
#include "afxdialogex.h"


// CDlgShow �Ի���

IMPLEMENT_DYNAMIC(CDlgShow, CDialogEx)

CDlgShow::CDlgShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_Show, pParent)
	, m_pCreateWnd(NULL)
{

}

CDlgShow::~CDlgShow()
{
}

void CDlgShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShow, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgShow ��Ϣ�������


void CDlgShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CWnd *pMovie = GetDlgItem(IDC_STATIC_BigMovie);
	if (pMovie != NULL && pMovie->GetSafeHwnd() != NULL)
	{
		CRect csClient;
		GetWindowRect(&csClient);
		csClient.left += 8;
		csClient.top += 31;
		csClient.right -= 8;
		csClient.bottom -= 8;

		ScreenToClient(&csClient);

		pMovie->MoveWindow(&csClient);
		pMovie->Invalidate();
	}

	// TODO: �ڴ˴������Ϣ����������
}


BOOL CDlgShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
