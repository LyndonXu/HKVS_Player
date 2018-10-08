#pragma once


// CDlgShow �Ի���

class CDlgShow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgShow)

public:
	CDlgShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgShow();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_Show };
#endif

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnSet();
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

private:
	CDialogEx *m_pDlgPlayCtrl;
	CStatusBar m_csStatusBar;

	CToolTipCtrl m_csToolTips;

public:
	BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE)
	{
		return m_csStatusBar.SetPaneText(nIndex, lpszNewText, bUpdate);
	}

};
