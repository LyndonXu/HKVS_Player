#pragma once
#include "afxwin.h"
#include "PasswordEdit.h"


// CDlgLocalSet 对话框

class CDlgLocalSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLocalSet)

public:
	CDlgLocalSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLocalSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_LocalSet };
#endif

protected:

	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

	CString m_csStrPasswordOld;

	afx_msg void OnBnClickedBtnSavefolderselect();
	afx_msg void OnBnClickedOk();
	CPasswordEdit m_csEditPassword;
	CPasswordEdit m_csEditNewPW;
	CPasswordEdit m_csEditNewPWRepeat;
	afx_msg void OnEnUpdateEditPassword();
	CString m_csStrNewPW;
	CString m_csStrNewPWRepeat;
};
