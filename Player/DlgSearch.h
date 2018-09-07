#pragma once
#include "afxdtctl.h"
#include "atltime.h"
#include "afxwin.h"


// CDlgSearch 对话框

class CDlgSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSearch)

public:
	CDlgSearch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSearch();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_Search };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	CDateTimeCtrl m_csDTCSearchBegin;
	CDateTimeCtrl m_csDTCSearchEnd;
	CTime m_csTimeSearchBegin;
	CTime m_csTimeSearchEnd;
	CListCtrl m_csListSearchResult;
	afx_msg void OnNMDblclkListSearchresult(NMHDR *pNMHDR, LRESULT *pResult);
};
