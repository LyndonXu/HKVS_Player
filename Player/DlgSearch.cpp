// DlgSearch.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "DlgSearch.h"
#include "afxdialogex.h"
#include "PlayerDlg.h"

// CDlgSearch 对话框

IMPLEMENT_DYNAMIC(CDlgSearch, CDialogEx)

CDlgSearch::CDlgSearch(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_Search, pParent)
	, m_csTimeSearchBegin(0)
	, m_csTimeSearchEnd(0)
	, m_pParent(NULL)
	, m_csStrSelectItem(L"")
{
	m_pParent = pParent;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgSearch::~CDlgSearch()
{
}

void CDlgSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DMP_SearchBegin, m_csDTCSearchBegin);
	DDX_Control(pDX, IDC_DMP_SearchEnd, m_csDTCSearchEnd);
	DDX_DateTimeCtrl(pDX, IDC_DMP_SearchBegin, m_csTimeSearchBegin);
	DDX_DateTimeCtrl(pDX, IDC_DMP_SearchEnd, m_csTimeSearchEnd);
	DDX_Control(pDX, IDC_LIST_SearchResult, m_csListSearchResult);
}


BEGIN_MESSAGE_MAP(CDlgSearch, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSearch::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SearchResult, &CDlgSearch::OnNMDblclkListSearchresult)
	ON_BN_CLICKED(IDC_BTN_Search, &CDlgSearch::OnBnClickedBtnSearch)
END_MESSAGE_MAP()


// CDlgSearch 消息处理程序



BOOL CDlgSearch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	__time64_t u64Time = _time64(NULL);


	m_csTimeSearchBegin = u64Time - 24 * 3600;
	m_csTimeSearchEnd = u64Time;

	UpdateData(FALSE);

	m_csDTCSearchBegin.SetFormat(L"yyyy' - 'MM' - 'dd'   'HH' : 'mm' : 'ss");
	m_csDTCSearchEnd.SetFormat(L"yyyy' - 'MM' - 'dd'   'HH' : 'mm' : 'ss");

	LVCOLUMN lvcolumn;
	m_csListSearchResult.SetExtendedStyle(m_csListSearchResult.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_csListSearchResult.InsertColumn(0, L"文件");
	m_csListSearchResult.GetColumn(0, &lvcolumn);
	lvcolumn.mask = LVCF_WIDTH;
	lvcolumn.cx = 360;
	m_csListSearchResult.SetColumn(0, &lvcolumn);

	m_csListSearchResult.InsertColumn(1, L"大小");
	m_csListSearchResult.GetColumn(1, &lvcolumn);
	lvcolumn.mask = LVCF_WIDTH;
	lvcolumn.cx = 100;
	m_csListSearchResult.SetColumn(1, &lvcolumn);

	m_csStrSelectItem = L"";

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgSearch::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::DestroyWindow();
}
int32_t SearchSaveFolderCB(const StRecoderFileInfo *pInfo, void *pContext)
{
	if (pContext != NULL)
	{
		((CDlgSearch *)pContext)->ListInsterItem(pInfo->cwStrName.c_str(), pInfo->u64FileSize);
	}

	return 0;
}
void CDlgSearch::ListInsterItem(const wchar_t *pStr, UINT64 u64Size)
{
	INT nCount = m_csListSearchResult.GetItemCount();
	m_csListSearchResult.InsertItem(nCount, pStr);
	
	if (u64Size != 0)
	{
		CString csStr;
		csStr.Format(L"%lldMB", (u64Size + (1024 * 512)) / (1024 * 1024));
		m_csListSearchResult.SetItemText(nCount, 1, csStr);
	}
}

void CDlgSearch::OnBnClickedBtnSearch()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	if (m_pParent != NULL)
	{
		m_csListSearchResult.DeleteAllItems();
		((CPlayerDlg *)m_pParent)->SearchSaveFolder(
			m_csTimeSearchBegin.GetTime(),
			m_csTimeSearchEnd.GetTime(),
			SearchSaveFolderCB, this);
	}

}

void CDlgSearch::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	int s32Item = m_csListSearchResult.GetNextItem(-1, LVIS_SELECTED);
	if (s32Item != -1)
	{
		m_csStrSelectItem = m_csListSearchResult.GetItemText(s32Item, 0);
	}

	CDialogEx::OnOK();
}


void CDlgSearch::OnNMDblclkListSearchresult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	m_csStrSelectItem = m_csListSearchResult.GetItemText(pNMItemActivate->iItem, 0);
	*pResult = 0;

	EndDialog(IDOK);
}


