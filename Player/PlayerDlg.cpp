
// PlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"


#include "utils.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlayerDlg 对话框



CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAYER_DIALOG, pParent)
	, m_pDlgShow(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_NextFrame, &CPlayerDlg::OnBnClickedBtnNextframe)
	ON_BN_CLICKED(IDC_BTN_PlayPause, &CPlayerDlg::OnBnClickedBtnPlaypause)
	ON_BN_CLICKED(IDC_BTN_PrevFrame, &CPlayerDlg::OnBnClickedBtnPrevframe)
	ON_MESSAGE(PLAYCTRL_MSG, &CPlayerDlg::PlayCtrlMessage)

END_MESSAGE_MAP()



static UINT s_uIndicators[] =
{
	CPlayerDlg::_StatusBar_CPU,
	CPlayerDlg::_StatusBar_Memory,
	CPlayerDlg::_StatusBar_NetInterface,
};

// CPlayerDlg 消息处理程序

BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	{
		m_pDlgShow = new CDlgShow;
		if (m_pDlgShow != NULL)
		{
			m_pDlgShow->SetCreateWND(this);
			m_pDlgShow->Create(IDD_DLG_Show, GetDesktopWindow());
			m_pDlgShow->ShowWindow(SW_SHOW);
		}
	}
	{
		int s32Width = GetSystemMetrics(SM_CXSCREEN);
		int s32Height = GetSystemMetrics(SM_CYSCREEN);

		CRect csClient;
		GetClientRect(csClient);

		int s32Left = (s32Width - csClient.Width()) / 2 - 8;
		int s32Top = s32Height - csClient.Height() - 60;
		csClient.MoveToXY(s32Left, s32Top);
		
		csClient.right += 16;
		csClient.bottom += 20;

		::SetWindowPos(GetSafeHwnd(), NULL,//HWND_TOPMOST, 
			csClient.left, csClient.top,
			csClient.Width(), csClient.Height(),
			SWP_SHOWWINDOW);
	}


	{
		CRect csRect;

		GetClientRect(csRect);

		m_csStatusBar.Create(this);

		m_csStatusBar.SetIndicators(s_uIndicators, sizeof(s_uIndicators) / sizeof(UINT));

		m_csStatusBar.MoveWindow(0, csRect.bottom - 20, csRect.right, 20);
		m_csStatusBar.SetPaneInfo(_StatusBar_CPU,
			s_uIndicators[_StatusBar_CPU], SBPS_NORMAL, 60);
		m_csStatusBar.SetPaneInfo(_StatusBar_Memory,
			s_uIndicators[_StatusBar_Memory], SBPS_NORMAL, 60);
		m_csStatusBar.SetPaneInfo(_StatusBar_NetInterface,
			s_uIndicators[_StatusBar_NetInterface], SBPS_NORMAL, 180);

		m_csStatusBar.SetPaneText(_StatusBar_CPU, L"1", TRUE);
		m_csStatusBar.SetPaneText(_StatusBar_Memory, L"2", TRUE);
		m_csStatusBar.SetPaneText(_StatusBar_NetInterface, L"3", TRUE);

	}
	//SetConfig();
	//GetConfig();

	SetTimer(1, 1000, NULL);

	//SetTimer(2, 40, NULL);

	SetWindowText(L"1231312");
	m_csPlayCtrl.RegisterWNDMsg(GetSafeHwnd(), PLAYCTRL_MSG);
	//m_csPlayCtrl.SetFolder(L"F:\\GIT\\HKVS_Player\\Player\\");
	//m_csPlayCtrl.BeginRender(GetDlgItem(IDC_STATIC_Movie)->GetSafeHwnd());
	//m_csPlayCtrl.BeginSave();
	//m_csPlayCtrl.BeginLocalPlay(L"1536135558000.dat");

	{
		UINT32 u32Size = sizeof(StFrameHeader);
		u32Size = u32Size;
		StFrameHeader stHeader = { 1, 2, 3, 4, 5, 6, 7 };
		int32_t s32Ret = GetFrameHeaderCheckSum(&stHeader);
		s32Ret = GetFrameHeaderCheckSum(&stHeader, true);
		s32Ret = s32Ret;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CPlayerDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	KillTimer(1);
	KillTimer(2);

	if (m_pDlgShow != NULL)
	{
		delete m_pDlgShow;
	}

	return CDialogEx::DestroyWindow();
}

INT CPlayerDlg::SetConfig(void)
{
	wchar_t wcFileName[256] = { 0 };
	DWORD dwRet = GetModuleFileName(NULL, wcFileName, 256);

	if (dwRet != 0 && dwRet < 254)
	{
		TCHAR *pName = wcsrchr(wcFileName, _T('\\'));
		if (pName != NULL)
		{
			wsprintf(pName + 1, _T("%s"), _T("config.ini"));
		}
	}
	else
	{
		wsprintf(wcFileName, _T("%s"), _T("config.ini"));
	}

	WritePrivateProfileString(
		_T("LocalSet")
		, _T("SaveFolder")
		, _T("F:\\SaveFolder")
		, wcFileName
	);

	WritePrivateProfileString(
		_T("LocalSet")
		, _T("FolderMaxSize")
		, _T("20480MB")
		, wcFileName
	);

	WritePrivateProfileString(
		_T("LocalSet")
		, _T("SaveContinusTime")
		, _T("5min")
		, wcFileName
	);

	WritePrivateProfileString(
		_T("LocalSet")
		, _T("Title")
		, _T("User config")
		, wcFileName
	);

	return 0;
}
INT CPlayerDlg::GetConfig(void)
{
	wchar_t wcFileName[256] = { 0 };
	DWORD dwRet = GetModuleFileName(NULL, wcFileName, 256);

	if (dwRet != 0 && dwRet < 254)
	{
		TCHAR *pName = wcsrchr(wcFileName, _T('\\'));
		if (pName != NULL)
		{
			wsprintf(pName + 1, _T("%s"), _T("config.ini"));
		}
	}
	else
	{
		wsprintf(wcFileName, _T("%s"), _T("config.ini"));
	}

	wchar_t wcValue[256];
	string csStr;

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("SaveFolder")
		, _T("F:\\SaveFolder")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);

	PRINT("SaveFolder: %s\n", csStr.c_str());


	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("FolderMaxSize")
		, _T("20480MB")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("FolderMaxSize: %s\n", csStr.c_str());

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("SaveContinusTime")
		, _T("5min")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("SaveContinusTime: %s\n", csStr.c_str());

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("Title")
		, _T("User config")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("Title: %s\n", csStr.c_str());

	return 0;
}


LRESULT CPlayerDlg::PlayCtrlMessage(WPARAM wMsg, LPARAM lData)
{
	switch (wMsg)
	{
		case _WND_Msg_FoldSize:
		{
			UINT64 u64Size = lData;
			//u64Size *= (1024 * 1024);
			TRACE(L"FoldSize: %lld MB\n", u64Size);
			if (u64Size > 20 * 1024)/* 20GB */
			{
				m_csPlayCtrl.ReduceFolderSize(20 * 1024);
			}
		}
		default:
		break;
	}

	return 0;
}


#define WIDTH	672	
#define HEIGHT	512

void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值


	if (nIDEvent == 1)
	{
		UINT32 u32CPUIdle = 0;
		UINT64 u64MemTotal = 0;
		UINT32 u32MemLoad = 0;
		UINT32 u32NetworkCount = 0;
		m_csSysInfo.GetCpuIdle(u32CPUIdle);
		m_csSysInfo.GetMemInfo(u64MemTotal, u32MemLoad);
		m_csSysInfo.GetNetworkInfo(u32NetworkCount);

		CString csStr;
		csStr.Format(L"CPU: %d%%", 100 - u32CPUIdle);
		m_csStatusBar.SetPaneText(_StatusBar_CPU, csStr, TRUE);

		csStr.Format(L"内存: %d%%", u32MemLoad);
		m_csStatusBar.SetPaneText(_StatusBar_Memory, csStr, TRUE);


		UINT32 u32InSpeed = 0;
		UINT32 u32OutSpeed = 0;

		m_csSysInfo.GetNetworkInfo(0, u32InSpeed, u32OutSpeed);

		csStr.Format(L"网络: 接收%dKbps, 发送%dKbps", u32InSpeed / 1024, u32OutSpeed / 1024);
		m_csStatusBar.SetPaneText(_StatusBar_NetInterface, csStr, TRUE);
	}
	if (nIDEvent == 2)
	{
		for (INT  i = 0; i < 1; i++)
		{
			static UINT32 u32Cnt = 0;
			static unsigned char u8Buf[WIDTH * HEIGHT * 3];
			static UINT64 u64Time = time(NULL);
			unsigned char *pTmp = u8Buf;
			for (int32_t j = 0; j < WIDTH * HEIGHT; j++, pTmp += 3)
			{
				pTmp[0] = 0;
				pTmp[1] = 0;
				pTmp[2] = u32Cnt;
			}

			UINT64 u64TimeTmp = u64Time * 1000 + u32Cnt * 15;

			StFrameHeader stFrameHeader = { 0 };
			stFrameHeader.u32Header = FRAME_HEADER;
			stFrameHeader.u16Width = WIDTH;
			stFrameHeader.u16Height = HEIGHT;

			stFrameHeader.u32TimeStampHigh = (uint32_t)(u64TimeTmp >> 32);
			stFrameHeader.u32TimeStampLow = (uint32_t)u64TimeTmp;


			UINT32 u32Length = WIDTH * HEIGHT * 3;

			StParamV stParam[3] = 
			{
				{ &stFrameHeader, sizeof(stFrameHeader) },
				{ &u32Length, sizeof(UINT32) },
				{ u8Buf, u32Length },
			};

			u32Cnt++;
			m_csPlayCtrl.SendShareData(stParam, 3, SHARE_DATA_RENDER | SHARE_DATA_SAVE);
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CPlayerDlg::OnBnClickedBtnNextframe()
{
	// TODO: 在此添加控件通知处理程序代码
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_NextFrame);
}


void CPlayerDlg::OnBnClickedBtnPlaypause()
{
	// TODO: 在此添加控件通知处理程序代码
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Play);
}


void CPlayerDlg::OnBnClickedBtnPrevframe()
{
	// TODO: 在此添加控件通知处理程序代码
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_PrevFrame);
}
