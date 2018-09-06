
// PlayerDlg.h : 头文件
//

#pragma once
#include "GetSysInfo.h"
#include "PlayCtrl.h"
#include "DlgShow.h"

// CPlayerDlg 对话框
class CPlayerDlg : public CDialogEx
{
// 构造
public:
	CPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	enum
	{
		_StatusBar_CPU,
		_StatusBar_Memory,
		_StatusBar_NetInterface,
	};

private:

	CStatusBar m_csStatusBar;

	CGetSysInfo m_csSysInfo;
	CPlayCtrl m_csPlayCtrl;

	CDlgShow *m_pDlgShow;

	wstring m_cswSaveFolder;
	UINT64 m_u64FolderMaxSize;		/* MB */
	UINT32 m_u32SaveContinusTime;	/* Minute */

	wstring m_cswTitle;

public:

	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnNextframe();
	afx_msg void OnBnClickedBtnPlaypause();
	afx_msg void OnBnClickedBtnPrevframe();

	LRESULT PlayCtrlMessage(WPARAM wMsg, LPARAM lData);

	INT GetConfig(void);
	INT SetConfig(void);

	void LocalPlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable = TRUE);
	void DevicePlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable = TRUE);


	afx_msg void OnBnClickedBtnLocalopenclose();
	afx_msg void OnBnClickedBtnFramejump();
	afx_msg void OnBnClickedBtnFpsrateset();
};

#define PLAYCTRL_MSG		(WM_USER + 200)

