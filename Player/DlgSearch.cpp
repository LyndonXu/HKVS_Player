// DlgSearch.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Player.h"
#include "DlgSearch.h"
#include "afxdialogex.h"


// CDlgSearch �Ի���

IMPLEMENT_DYNAMIC(CDlgSearch, CDialogEx)

CDlgSearch::CDlgSearch(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_Search, pParent)
	, m_csTimeSearchBegin(0)
	, m_csTimeSearchEnd(0)
{

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
END_MESSAGE_MAP()


// CDlgSearch ��Ϣ�������


void CDlgSearch::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


BOOL CDlgSearch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	__time64_t u64Time = _time64(NULL);


	m_csTimeSearchBegin = u64Time - 24 * 3600;
	m_csTimeSearchEnd = u64Time;

	UpdateData(FALSE);

	m_csDTCSearchBegin.SetFormat(L"yyyy' - 'MM' - 'dd'   'HH' : 'mm' : 'ss");
	m_csDTCSearchEnd.SetFormat(L"yyyy' - 'MM' - 'dd'   'HH' : 'mm' : 'ss");

	LVCOLUMN lvcolumn;
	m_csListSearchResult.SetExtendedStyle(m_csListSearchResult.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_csListSearchResult.InsertColumn(0, L"�ļ�����");
	m_csListSearchResult.GetColumn(0, &lvcolumn);
	lvcolumn.mask = LVCF_WIDTH;
	lvcolumn.cx = 360;
	m_csListSearchResult.SetColumn(0, &lvcolumn);

	for (UINT i = 0; i < 50; i++)
	{
		CString csTmp;
		csTmp.Format(L"test :%d", i);
		INT nCount = m_csListSearchResult.GetItemCount();
		m_csListSearchResult.InsertItem(nCount, csTmp);

	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDlgSearch::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::DestroyWindow();
}


void CDlgSearch::OnNMDblclkListSearchresult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString csStr = m_csListSearchResult.GetItemText(pNMItemActivate->iItem, 0);
	*pResult = 0;

	//SendMessage(WM_CLOSE, 0, 0);
	EndDialog(IDOK);
}
