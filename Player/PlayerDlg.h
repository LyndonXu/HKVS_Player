
// PlayerDlg.h : 头文件
//

#pragma once
#include "GetSysInfo.h"
#include "PlayCtrl.h"
#include "DlgShow.h"
#include "DlgSearch.h"
#include "afxwin.h"

#define STATUS_ERROR            -1


enum
{
	_StatusBar_CPU,
	_StatusBar_Memory,
	_StatusBar_NetInterface,
	_StatusBar_ErrorMessage,

	_StatusBar_Reserved,
};


// CPlayerDlg 对话框
class CPlayerDlg : public CDialogEx
{
// 构造
public:
	CPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CPlayerDlg();

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
	virtual BOOL DestroyWindow();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:


	CGetSysInfo m_csSysInfo;
	CPlayCtrl m_csPlayCtrl;

	CDlgShow *m_pDlgShow;

	wstring m_cswSaveFolder;
	UINT64 m_u64FolderMaxSize;		/* MB */
	UINT32 m_u32SaveContinusTime;	/* Minute */

	wstring m_cswTitle;

	bool m_boIsSliderChangingByUser;
	UINT32 m_u32ErrorTime;

	CComboBox m_csCBDevList;

	CString m_csLinkDevName;

	CMyCamera *m_pMyCamera;                // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
	MV_CC_DEVICE_INFO_LIST m_stDevList;         // ch:设备信息列表结构体变量，用来存储设备列表

	CEdit m_csEditExposure;
	CEdit m_csEditGain;
	CEdit m_csEditFPS;
	double m_d64Exposure;
	double m_d64Gain;
	double m_d64FPS;

public:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnNextframe();
	afx_msg void OnBnClickedBtnPlaypause();
	afx_msg void OnBnClickedBtnPrevframe();
	afx_msg void OnBnClickedBtnLocalopenclose();
	afx_msg void OnBnClickedBtnFramejump();
	afx_msg void OnBnClickedBtnFpsrateset();
	afx_msg void OnBnClickedBtnLocalsearch();

	afx_msg void OnBnClickedBtnDevicesearch();
	afx_msg void OnBnClickedBtnDeviceopenclose();
	afx_msg void OnBnClickedBtnDevicecapture();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnDeviceparamget();
	afx_msg void OnBnClickedBtnDeviceparamset();
	afx_msg void OnBnClickedBtnLocalset();
	afx_msg void OnBnClickedBtnHide();

	LRESULT PlayCtrlMessage(WPARAM wMsg, LPARAM lData);

	INT GetConfig(void);
	INT SetConfig(void);
	void ShowErrorMsg(wchar_t *pMsg, INT32 s32ErrorNo = 0);

	void LocalPlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable = TRUE);
	void DevicePlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable = TRUE);

	INT OpenDevice(void);
	INT CloseDevice(void);


	int GetExposureTime(void);
	int SetExposureTime(void);
	int GetGain(void);
	int SetGain(void);
	int GetFrameRate(void);
	int SetFrameRate(void);
	int GetFrameFormat(void);
	int SetFrameFormat(MvGvspPixelType emType);

	INT SendFrame(const void *pData, MV_FRAME_OUT_INFO_EX* pFrameInfo);
	INT SearchSaveFolder(uint64_t u64TimeBegin, uint64_t u64TimeEnd,
		PFUN_SearchSaveFolderCB pFunCB, void *pContext);


	void SetShowDlg(CDlgShow *pDlg)
	{
		m_pDlgShow = pDlg;
	}


	afx_msg void OnClose();
};

#define PLAYCTRL_MSG		(WM_USER + 200)

