#pragma once
#include "afxdtctl.h"
#include "atltime.h"
#include "afxwin.h"


// CDlgSearch �Ի���

class CDlgSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSearch)

public:
	CDlgSearch(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSearch();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_Search };
#endif

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnNMDblclkListSearchresult(NMHDR *pNMHDR, LRESULT *pResult);


	void ListInsterItem(const wchar_t *pStr, UINT64 u64Size = 0);

	CDateTimeCtrl m_csDTCSearchBegin;
	CDateTimeCtrl m_csDTCSearchEnd;
	CTime m_csTimeSearchBegin;
	CTime m_csTimeSearchEnd;
	CListCtrl m_csListSearchResult;

	CWnd *m_pParent;
	CString m_csStrSelectItem;
};