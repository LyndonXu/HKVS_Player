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

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	CWnd *m_pCreateWnd;

	void SetCreateWND(CWnd *pWnd)
	{
		m_pCreateWnd = pWnd;
	}

};
