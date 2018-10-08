// DlgLocalSet.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "DlgLocalSet.h"
#include "afxdialogex.h"


// CDlgLocalSet 对话框

IMPLEMENT_DYNAMIC(CDlgLocalSet, CDialogEx)

CDlgLocalSet::CDlgLocalSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_LocalSet, pParent)
	, m_csStrSaveFolder(_T(""))
	, m_u32FolderSize(20)
	, m_s32RecordContinusTime(0)
	, m_csStrTitle(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgLocalSet::~CDlgLocalSet()
{
}

void CDlgLocalSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SaveFolder, m_csEditSaveFolder);
	DDX_Text(pDX, IDC_EDIT_SaveFolder, m_csStrSaveFolder);
	DDX_Control(pDX, IDC_EDIT_FolderSize, m_csEditFolderSize);
	DDX_Text(pDX, IDC_EDIT_FolderSize, m_u32FolderSize);
	DDX_Control(pDX, IDC_COMBO_RecordContinusTime, m_csComboRecordContinusTime);
	DDX_CBIndex(pDX, IDC_COMBO_RecordContinusTime, m_s32RecordContinusTime);
	DDV_MinMaxInt(pDX, m_s32RecordContinusTime, 0, 9);
	DDX_Control(pDX, IDC_EDIT_Title, m_csEditTitle);
	DDX_Text(pDX, IDC_EDIT_Title, m_csStrTitle);
}


BEGIN_MESSAGE_MAP(CDlgLocalSet, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SaveFolderSelect, &CDlgLocalSet::OnBnClickedBtnSavefolderselect)
	ON_BN_CLICKED(IDOK, &CDlgLocalSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLocalSet 消息处理程序


BOOL CDlgLocalSet::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类


	return CDialogEx::DestroyWindow();
}


BOOL CDlgLocalSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}

	for (INT i = 0; i < 10; i++)
	{
		CString csStr;
		csStr.Format(L"%d分钟", i + 1);
		m_csComboRecordContinusTime.AddString(csStr);
	}


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgLocalSet::OnBnClickedBtnSavefolderselect()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR           szFolderPath[MAX_PATH] = { 0 };

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			m_csEditSaveFolder.SetWindowText(szFolderPath);
		}
	}
}


void CDlgLocalSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData();

	CDialogEx::OnOK();
}
