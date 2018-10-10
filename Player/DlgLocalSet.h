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
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_csEditSaveFolder;
	CString m_csStrSaveFolder;
	CEdit m_csEditFolderSize;
	UINT m_u32FolderSize;
	CComboBox m_csComboRecordContinusTime;
	int m_s32RecordContinusTime;
	CEdit m_csEditTitle;
	CString m_csStrTitle;

	CString m_csStrPasswordOld;

	CPasswordEdit m_csEditPassword;

	CPasswordEdit m_csEditNewPW;
	CString m_csStrNewPW;

	CPasswordEdit m_csEditNewPWRepeat;
	CString m_csStrNewPWRepeat;

	afx_msg void OnBnClickedBtnSavefolderselect();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnUpdateEditPassword();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
