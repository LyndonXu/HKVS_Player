#pragma once
#include "afxwin.h"


// CDlgLocalSet �Ի���

class CDlgLocalSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLocalSet)

public:
	CDlgLocalSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLocalSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LocalSet };
#endif

protected:

	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	CEdit m_csEditSaveFolder;
	CString m_csStrSaveFolder;
	CEdit m_csEditFolderSize;
	UINT m_u32FolderSize;
	CComboBox m_csComboRecordContinusTime;
	int m_s32RecordContinusTime;
	CEdit m_csEditTitle;
	CString m_csStrTitle;
	afx_msg void OnBnClickedBtnSavefolderselect();
	afx_msg void OnBnClickedOk();
};
