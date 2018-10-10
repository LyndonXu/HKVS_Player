
// PlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <MMSystem.h>
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"
#include "DlgLocalSet.h"

#include "utils.h"
#include "sundries.h"

#pragma comment(lib, "Winmm.lib")

#define DEFALT_PASSWORD		"JAHMwxk7VCHh0iZJ"
#define DEFALT_TITLE		"bmymvCpCOBe0NUDqPluNh56Q0RTkicVIhaNlxRvPS9dv9OnAwDH4iKYTM4xY5oXcj0ydp6TtY0j1KSam/ttMgFBdMlNoo3/X"	

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
	, m_cswSaveFolder(L"F:\\")
	, m_u64FolderMaxSize(20 * 1024)
	, m_u32SaveContinusTime(5)

	, m_cswTitle(L"User Config")

	, m_d64BackupExposure(4000)
	, m_d64BackupGain(15)
	, m_d64BackupFPS(200)
	, m_u32BackupWidth(336)
	, m_u32BackupHeight(256)
	
	, m_boIsSliderChangingByUser(false)
	, m_pMyCamera(NULL)

	, m_u32ErrorTime(NULL)
	, m_d64Exposure(0)
	, m_d64Gain(0.0)
	, m_d64FPS(0.0)
	, m_u32Width(0)
	, m_u32Height(0)
{
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPlayerDlg::~CPlayerDlg()
{
	DestroyWindow();
}


void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DevList, m_csCBDevList);
	DDX_Control(pDX, IDC_EDIT_Exposure, m_csEditExposure);
	DDX_Control(pDX, IDC_EDIT_Gain, m_csEditGain);
	DDX_Control(pDX, IDC_EDIT_FPS, m_csEditFPS);
	DDX_Text(pDX, IDC_EDIT_Exposure, m_d64Exposure);
	DDX_Text(pDX, IDC_EDIT_Gain, m_d64Gain);
	DDX_Text(pDX, IDC_EDIT_FPS, m_d64FPS);
	DDX_Control(pDX, IDC_EDIT_Width, m_csEditWidth);
	DDX_Control(pDX, IDC_EDIT_Height, m_csEditHeight);
	DDX_Text(pDX, IDC_EDIT_Width, m_u32Width);
	DDV_MinMaxUInt(pDX, m_u32Width, 0, 672);
	DDX_Text(pDX, IDC_EDIT_Height, m_u32Height);
	DDV_MinMaxUInt(pDX, m_u32Height, 0, 512);
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

	ON_BN_CLICKED(IDC_BTN_LocalOpenClose, &CPlayerDlg::OnBnClickedBtnLocalopenclose)
	ON_BN_CLICKED(IDC_BTN_FrameJump, &CPlayerDlg::OnBnClickedBtnFramejump)
	ON_BN_CLICKED(IDC_BTN_FPSRateSet, &CPlayerDlg::OnBnClickedBtnFpsrateset)
	ON_BN_CLICKED(IDC_BTN_DeviceSearch, &CPlayerDlg::OnBnClickedBtnDevicesearch)
	ON_BN_CLICKED(IDC_BTN_DeviceOpenClose, &CPlayerDlg::OnBnClickedBtnDeviceopenclose)
	ON_BN_CLICKED(IDC_BTN_DeviceCapture, &CPlayerDlg::OnBnClickedBtnDevicecapture)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_LocalSearch, &CPlayerDlg::OnBnClickedBtnLocalsearch)
	ON_BN_CLICKED(IDC_BTN_DeviceParamGet, &CPlayerDlg::OnBnClickedBtnDeviceparamget)
	ON_BN_CLICKED(IDC_BTN_DeviceParamSet, &CPlayerDlg::OnBnClickedBtnDeviceparamset)
	ON_BN_CLICKED(IDC_BTN_LocalSet, &CPlayerDlg::OnBnClickedBtnLocalset)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CPlayerDlg::OnBnClickedBtnHide)
	ON_WM_CLOSE()
END_MESSAGE_MAP()



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
		
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
#ifndef _DEBUG
	RegAutoRun();
#endif


#if 0
	{
		//int32_t GetDecodePassword(const char *pBase64, char **p2Password);
		//int32_t GetEncodePassword(const char *pPassword, char **p2Base64);
		char *pPassword = NULL, *pBase64 = NULL;
		GetEncodePassword("高速飞剪信息采集存储监控系统  版本V10.0.1  天津北洋天泽 版权所有", &pBase64);
		GetDecodePassword(pBase64, &pPassword);

		if (pPassword != NULL)
		{
			free(pPassword);
		}

		if (pBase64 != NULL)
		{
			free(pBase64);
		}
	}
#endif

#if 0
	{
		m_pDlgShow = new CDlgShow;
		if (m_pDlgShow != NULL)
		{
			m_pDlgShow->SetCreateWND(this);
			m_pDlgShow->Create(IDD_DLG_Show, GetDesktopWindow());
			m_pDlgShow->ShowWindow(SW_SHOW);
		}
	}
#endif
	{
		int s32Width = GetSystemMetrics(SM_CXSCREEN);
		//int s32Height = GetSystemMetrics(SM_CYSCREEN);

		CRect csClient;
		GetClientRect(csClient);

#ifdef _DEBUG
		int s32Left = (s32Width - csClient.Width()) / 2 - 8;
		int s32Top = 50 + 338;//s32Height - csClient.Height() - 60;
		csClient.MoveToXY(s32Left, s32Top);
		
		csClient.right += 16;
		csClient.bottom += 32;
#else
		int s32Left = (s32Width - csClient.Width()) / 2 - 3;
		int s32Top = 50 + 338;//s32Height - csClient.Height() - 60;
		csClient.MoveToXY(s32Left, s32Top);
		
		csClient.right += 6;
		csClient.bottom += 32;
#endif
		::SetWindowPos(GetSafeHwnd(), NULL,//HWND_TOPMOST, 
			csClient.left, csClient.top,
			csClient.Width(), csClient.Height(),
			SWP_SHOWWINDOW);
	}


	GetConfig();
	//SetConfig();

	SetTimer(1, 1000, NULL);

	if (m_pDlgShow != NULL)
	{
		m_pDlgShow->SetWindowText(m_cswTitle.c_str());
	}
	m_csPlayCtrl.RegisterWNDMsg(GetSafeHwnd(), PLAYCTRL_MSG);
	m_csPlayCtrl.SetFolder(m_cswSaveFolder.c_str());
	m_csPlayCtrl.SetSaveContinusTime(m_u32SaveContinusTime);

#if 0
	SetTimer(2, 40, NULL);
	if (m_pDlgShow != NULL)
	{
		m_csPlayCtrl.BeginRender(m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->GetSafeHwnd());
	}
	m_csPlayCtrl.BeginSave();
	//m_csPlayCtrl.BeginLocalPlay(L"F:\\SaveFolder\\2018-09-06 03-38-39_915.dat");
#endif

	LocalPlayWidgetEnable(0, FALSE);
	DevicePlayWidgetEnable(0, FALSE);

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

	if (m_pMyCamera != NULL)
	{
		delete m_pMyCamera;
		m_pMyCamera = NULL;
	}

	KillTimer(1);
	KillTimer(2);

	//if (m_pDlgShow != NULL)
	//{
	//	delete m_pDlgShow;
	//}

	return CDialogEx::DestroyWindow();
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
			if (u64Size > m_u64FolderMaxSize)/* 20GB */
			{
				m_csPlayCtrl.ReduceFolderSize(m_u64FolderMaxSize);
			}
			break;
		}
		case _WND_Msg_PlayMode:
		{
			if (lData == _PlayMode_Play || lData == _PlayMode_Pause)
			{
				if (!GetDlgItem(IDC_BTN_PlayPause)->IsWindowEnabled())
				{
					LocalPlayWidgetEnable(1);
					DevicePlayWidgetEnable(1, FALSE);
					GetDlgItem(IDC_BTN_LocalOpenClose)->SetWindowText(L"关闭");
				}
				if (lData == _PlayMode_Play)
				{
					GetDlgItem(IDC_BTN_PlayPause)->SetWindowText(L"暂停");
				}
				else
				{
					GetDlgItem(IDC_BTN_PlayPause)->SetWindowText(L"恢复");
				}
			}
			else
			{
				//if (GetDlgItem(IDC_BTN_PlayPause)->IsWindowEnabled())
				//{
				//	GetDlgItem(IDC_BTN_PlayPause)->EnableWindow(FALSE);
				//	GetDlgItem(IDC_BTN_NextFrame)->EnableWindow(FALSE);
				//	GetDlgItem(IDC_BTN_PrevFrame)->EnableWindow(FALSE);
				//}

			}
			break;
		}
		case _WND_Msg_LocalPlayFileIndexSize:
		{
			//PRINT("Totoal Index: %d\n", lData);
			UINT32 u32TotalIndex = lData;
			if (u32TotalIndex != 0)
			{
				CString csStr;
				csStr.Format(L"%d", u32TotalIndex);
				((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_PlayPos)))->SetRange(1, u32TotalIndex, TRUE);
				GetDlgItem(IDC_STATIC_TotalFrame)->SetWindowText(csStr);
			}
			else
			{
				m_csPlayCtrl.StopLocalPlay();
				Sleep(60);/* let render receive all frame */
				m_csPlayCtrl.StopRender();

				LocalPlayWidgetEnable(0, FALSE);
				DevicePlayWidgetEnable(1, TRUE);
				DevicePlayWidgetEnable(0, FALSE);

				GetDlgItem(IDC_BTN_LocalOpenClose)->SetWindowText(L"打开");
				ShowErrorMsg(L"file format error");
			}
			break;
		}
		case _WND_Msg_LocalPlayFileCurIndex:
		{
			//PRINT("current Index: %d\n", lData);
			//if (GetDlgItem(IDC_SLIDER_PlayPos)->IsWindowEnabled())
			{
				UINT32 u32CurrentIndex = lData + 1;
				CString csStr;
				csStr.Format(L"%d", u32CurrentIndex);
				GetDlgItem(IDC_STATIC_CurFrame)->SetWindowText(csStr);

				if (!m_boIsSliderChangingByUser)
				{
					((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_PlayPos)))->SetPos(u32CurrentIndex);
				}
			}
			break;
		}
		case _WND_Msg_ShowRectInvalidate:
		{
			m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->Invalidate();
			//m_pDlgShow->SetWindowText(L"");
			break;
		}
		case _WND_Msg_ShowName:
		{
			wchar_t *pStr = (wchar_t *)lData;
			if (pStr != NULL)
			{
				//m_pDlgShow->SetWindowText(pStr);
				free(pStr);
			}
			break;
		}
		case _WND_Msg_LocalPlayOut:
		{
			((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_PlayPos)))->SetRange(0, 100);
			((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_PlayPos)))->SetPos(0);
			GetDlgItem(IDC_STATIC_CurFrame)->SetWindowText(L"0");
			GetDlgItem(IDC_STATIC_TotalFrame)->SetWindowText(L"0");

			break;
		}
		default:
		break;
	}

	return 0;
}
UINT64 GetUNIXTimeStampMS(void)
{
	SYSTEMTIME stSysTime;
	GetLocalTime(&stSysTime);
	CTime csTime(stSysTime);

	__time64_t u64Time = csTime.GetTime();
	u64Time = u64Time * 1000 + stSysTime.wMilliseconds;

	return u64Time;
}
INT CPlayerDlg::SendFrame(const void *pData, MV_FRAME_OUT_INFO_EX* pFrameInfo)
{
	if (pData == NULL || pFrameInfo == NULL)
	{
		return -1;
	}

	if (pFrameInfo->enPixelType != PixelType_Gvsp_RGB8_Packed)
	{
		ShowErrorMsg(L"Not RGB8 Format");
		return -1;
	}

	//UINT64 u64TimeTmp = pFrameInfo->nDevTimeStampHigh;
	//u64TimeTmp = (u64TimeTmp << 32) + pFrameInfo->nDevTimeStampLow;

	//PRINT("u64TimeTmp: %lld\n", u64TimeTmp);
	UINT64 u64TimeTmp = GetUNIXTimeStampMS();

	StFrameHeader stFrameHeader = { 0 };
	stFrameHeader.u32Header = FRAME_HEADER;
	stFrameHeader.u16Width = pFrameInfo->nWidth;
	stFrameHeader.u16Height = pFrameInfo->nHeight;

	stFrameHeader.u32EncodeType = PixelType_Gvsp_RGB8_Packed;

	stFrameHeader.u32TimeStampHigh = (uint32_t)(u64TimeTmp >> 32);
	stFrameHeader.u32TimeStampLow = (uint32_t)u64TimeTmp;

	GetFrameHeaderCheckSum(&stFrameHeader);

	UINT32 u32Length = pFrameInfo->nFrameLen;

	StParamV stParam[3] =
	{
		{ &stFrameHeader, sizeof(stFrameHeader) },
		{ &u32Length, sizeof(UINT32) },
		{ pData, u32Length },
	};

	return m_csPlayCtrl.SendShareData(stParam, 3, SHARE_DATA_RENDER | SHARE_DATA_SAVE);
}

INT CPlayerDlg::SearchSaveFolder(uint64_t u64TimeBegin, uint64_t u64TimeEnd,
	PFUN_SearchSaveFolderCB pFunCB, void *pContext)
{
	return m_csPlayCtrl.SearchSaveFolder(u64TimeBegin, u64TimeEnd, pFunCB, pContext);
}



#define WIDTH	672	
#define HEIGHT	512

void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_u32ErrorTime != 0)
	{
		if ((timeGetTime() - m_u32ErrorTime) > 10 * 1000)
		{
			if (m_pDlgShow != NULL)
			{
				m_pDlgShow->SetPaneText(_StatusBar_ErrorMessage, NULL, TRUE);
			}
		}
	}

	if ((nIDEvent == 1) && (m_pDlgShow != NULL))
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
		m_pDlgShow->SetPaneText(_StatusBar_CPU, csStr, TRUE);

		csStr.Format(L"内存: %d%%", u32MemLoad);
		m_pDlgShow->SetPaneText(_StatusBar_Memory, csStr, TRUE);


		UINT32 u32Speed[2] ={ 0 };
		UINT32 u32SpeedUnit[2] = { 0 };
		const wchar_t *pwUnit[] =
		{
			L"",
			L"K",
			L"M",
			L"G"
		};

		m_csSysInfo.GetNetworkInfo(0, u32Speed[0], u32Speed[1]);

		for (INT i = 0; i < 2; i++)
		{
			for (INT j = 0; j < 3; j++)
			{
				if (u32Speed[i] > 1000)
				{
					u32Speed[i] = (u32Speed[i] + 500) / 1000;
					u32SpeedUnit[i] = j + 1;
				}
			}
		}

		csStr.Format(L"网络: 接收%d%sbps, 发送%d%sbps", 
			u32Speed[0], pwUnit[u32SpeedUnit[0]],
			u32Speed[1], pwUnit[u32SpeedUnit[1]]);

		m_pDlgShow->SetPaneText(_StatusBar_NetInterface, csStr, TRUE);
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

			stFrameHeader.u32EncodeType = PixelType_Gvsp_RGB8_Packed;

			stFrameHeader.u32TimeStampHigh = (uint32_t)(u64TimeTmp >> 32);
			stFrameHeader.u32TimeStampLow = (uint32_t)u64TimeTmp;

			GetFrameHeaderCheckSum(&stFrameHeader);


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
	CString csStr;
	GetDlgItem(IDC_BTN_PlayPause)->GetWindowTextW(csStr);
	if (csStr == L"暂停")
	{
		m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Pause);
	}
	else
	{
		m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Play);
	}
}


void CPlayerDlg::OnBnClickedBtnPrevframe()
{
	// TODO: 在此添加控件通知处理程序代码
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_PrevFrame);
}


void CPlayerDlg::OnBnClickedBtnLocalopenclose()
{
	// TODO: 在此添加控件通知处理程序代码

	CString csStr;
	GetDlgItem(IDC_BTN_LocalOpenClose)->GetWindowText(csStr);
	if (csStr == L"关闭")
	{
		m_csPlayCtrl.StopLocalPlay();
		Sleep(60);/* let render receive all frame */
		m_csPlayCtrl.StopRender();

		LocalPlayWidgetEnable(0, FALSE);
		DevicePlayWidgetEnable(1, TRUE);
		DevicePlayWidgetEnable(0, FALSE);

		GetDlgItem(IDC_BTN_LocalOpenClose)->SetWindowText(L"打开");

		return;
	}

	CFileDialog  csDlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"DAT Files(*.dat)|*.dat|"
		L"All Files(*.*)|*.*||");
	if (csDlg.DoModal() == IDCANCEL)
	{
		return;
	}

	csStr = csDlg.GetPathName();
	
	m_csPlayCtrl.BeginRender(m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->GetSafeHwnd());

	int32_t ret = m_csPlayCtrl.BeginLocalPlay(csStr.GetString());
	if (ret == 0)
	{
	}
	else
	{
		m_csPlayCtrl.StopRender();
	}
}


void CPlayerDlg::OnBnClickedBtnFramejump()
{
	// TODO: 在此添加控件通知处理程序代码

	CString csStr;
	GetDlgItem(IDC_EDIT_FrameJump)->GetWindowText(csStr);

	UINT32 u32JumpNumber = _wtoi(csStr.GetString());

	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Jump, (void *)u32JumpNumber);

}


void CPlayerDlg::OnBnClickedBtnFpsrateset()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csStr;
	GetDlgItem(IDC_EDIT_FPSRate)->GetWindowText(csStr);

	const wchar_t *pStr = csStr.GetString();
	UINT32 u32Start = 0;
	if (pStr[0] != L'-')
	{
		u32Start = 0;
	}
	else
	{
		u32Start = 1;
	}
	for (UINT32 i = u32Start; i < wcslen(pStr); i++)
	{
		if ((pStr[i] > L'9') || (pStr[i] < L'0'))
		{
			MessageBox(L"输入格式错误");
			return;
		}
	}
	INT32 s32Rate = _wtoi(pStr + u32Start);
	if (u32Start != 0)
	{
		s32Rate = 0 - s32Rate;
	}
	if (s32Rate == 0 || s32Rate > 30 || s32Rate < -30)
	{
		MessageBox(L"输入格式错误");
		return;
	}

	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_ChangeRate, (void *)s32Rate);
}

void CPlayerDlg::OnBnClickedBtnLocalsearch()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgSearch csDlg(this);
	INT_PTR nRet = csDlg.DoModal();

	if (nRet = IDOK)
	{
		if (csDlg.m_csStrSelectItem != L"")
		{
			m_csPlayCtrl.BeginRender(m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->GetSafeHwnd());

			CString csABSName = m_cswSaveFolder.c_str();
			csABSName += L"\\";
			csABSName += csDlg.m_csStrSelectItem;

			int nRet = m_csPlayCtrl.BeginLocalPlay(csABSName);
			if (nRet == 0)
			{
			}
		}		
	}

}
void CPlayerDlg::OnBnClickedBtnDevicesearch()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strMsg;

	// ch:清除设备列表框中的信息 | en:Clear Device List Information
	m_csCBDevList.ResetContent();

	// ch:初始化设备信息列表 | en:Device Information List Initialization
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
	int nRet = CMyCamera::EnumDevices(&m_stDevList);
	if (MV_OK != nRet)
	{
		return;
	}

	// ch:将值加入到信息列表框中并显示出来 | en:Add value to the information list box and display
	unsigned int i;
	int nIp1, nIp2, nIp3, nIp4;
	for (i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		if (NULL == pDeviceInfo)
		{
			continue;
		}
		if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
		{
			nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
			nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
			nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
			nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

			wchar_t* pUserName = NULL;
			if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256] = { 0 };
				sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}
			strMsg.Format(_T("[%d]GigE:    %s  (%d.%d.%d.%d)"), i,
				pUserName, nIp1, nIp2, nIp3, nIp4);
			if (NULL != pUserName)
			{
				delete(pUserName);
				pUserName = NULL;
			}

		}
		else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
		{
			wchar_t* pUserName = NULL;

			if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256];
				sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}
			strMsg.Format(_T("[%d]UsbV3:  %s"), i, pUserName);
			if (NULL != pUserName)
			{
				delete(pUserName);
				pUserName = NULL;
			}

		}
		else
		{
			ShowErrorMsg(TEXT("Unknown device enumerated"), 0);;
		}
		m_csCBDevList.AddString(strMsg);
	}

	if (0 == m_stDevList.nDeviceNum)
	{
		ShowErrorMsg(TEXT("No device"), 0);
		return;
	}
	m_csCBDevList.SetCurSel(0);

}


void CPlayerDlg::OnBnClickedBtnDeviceopenclose()
{
	// TODO: 在此添加控件通知处理程序代码

	CString csStr;
	GetDlgItem(IDC_BTN_DeviceOpenClose)->GetWindowText(csStr);
	if (csStr == L"打开")
	{
		if (OpenDevice() == MV_OK)
		{
			GetDlgItem(IDC_BTN_DeviceOpenClose)->SetWindowText(L"关闭");

			LocalPlayWidgetEnable(1, FALSE);
			DevicePlayWidgetEnable(1, TRUE);
			ShowErrorMsg(TEXT(""), 0);

			SetExposureTime(&m_d64BackupExposure);
			SetGain(&m_d64BackupGain);
			SetFrameRate(&m_d64BackupFPS);
			SetWidth(&m_u32BackupWidth);
			SetHeight(&m_u32BackupHeight);

			m_u32Width = m_u32BackupWidth;
			m_u32Height = m_u32BackupHeight;

			OnBnClickedBtnDeviceparamget();

			m_csCBDevList.GetLBText(m_csCBDevList.GetCurSel(), m_csLinkDevName);

			StartCapture();

		}
	}
	else
	{
		GetDlgItem(IDC_BTN_DeviceCapture)->GetWindowText(csStr);
		if (csStr == L"停止录制")
		{
			if (m_pMyCamera != NULL)
			{
				m_pMyCamera->StopGrabbing();
				m_csPlayCtrl.StopRender();
				m_csPlayCtrl.StopSave();
			}
			GetDlgItem(IDC_BTN_DeviceCapture)->SetWindowText(L"开始录制");
		}

		CloseDevice();
		GetDlgItem(IDC_BTN_DeviceOpenClose)->SetWindowText(L"打开");
		DevicePlayWidgetEnable(0, FALSE);

		LocalPlayWidgetEnable(1, TRUE);
		LocalPlayWidgetEnable(0, FALSE);

	}
}


void __stdcall ImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo,
	void* pUser)
{
#if 0
	static bool boIsSave = false;
	if (!boIsSave)
	{
		FILE *pFile = fopen("123.rgb", "wb+");
		if (pFile != NULL)
		{
			fwrite(pData, 1, pFrameInfo->nWidth * pFrameInfo->nHeight * 3, pFile);
			fclose(pFile);
		}
		boIsSave = true;
	}
#endif
	if (pUser != NULL)
	{
		((CPlayerDlg *)pUser)->SendFrame(pData, pFrameInfo);
	}
}

void CPlayerDlg::OnBnClickedBtnDevicecapture()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csStr;
	GetDlgItem(IDC_BTN_DeviceOpenClose)->GetWindowText(csStr);
	if (csStr == L"打开")
	{
		return;
	}

	if (m_pMyCamera == NULL)
	{
		return;
	}

	GetDlgItem(IDC_BTN_DeviceCapture)->GetWindowText(csStr);

	int nRet = 0;
	if (csStr == L"开始录制")
	{
		m_csPlayCtrl.BeginSave();
		GetDlgItem(IDC_BTN_DeviceCapture)->SetWindowText(L"停止录制");

	}
	else
	{
		m_csPlayCtrl.StopSave();
		GetDlgItem(IDC_BTN_DeviceCapture)->SetWindowText(L"开始录制");
	}
}


void CPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UINT u32RealPos = ((CSliderCtrl *)pScrollBar)->GetPos();

	//PRINT("OnHscroll nSBCode: %d, nPos: %d, u32RealPos: %d\n", nSBCode, nPos, u32RealPos);

	if (nSBCode < SB_ENDSCROLL)
	{
		m_boIsSliderChangingByUser = true;
	}
	else
	{
		m_boIsSliderChangingByUser = false;
	}

	if (nSBCode < SB_ENDSCROLL)
	{
		CString csStr;
		GetDlgItem(IDC_BTN_LocalOpenClose)->GetWindowText(csStr);
		if (csStr == L"关闭")
		{
			m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Jump, (void *)u32RealPos);
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}



INT CPlayerDlg::SetConfig(void)
{
	wchar_t wcFileName[256] = { 0 };
	CString csStrTmp;
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
		, m_cswSaveFolder.c_str()
		, wcFileName
	);

	csStrTmp.Format(L"%lldMB", m_u64FolderMaxSize);

	WritePrivateProfileString(
		_T("LocalSet")
		, _T("FolderMaxSize")
		, csStrTmp
		, wcFileName
	);

	csStrTmp.Format(L"%dmin", m_u32SaveContinusTime);
	WritePrivateProfileString(
		_T("LocalSet")
		, _T("SaveContinusTime")
		, csStrTmp
		, wcFileName
	);

	char *pBase64 = NULL;
	string csStr;
	wstring cswStr;

	Convert(m_cswTitle.c_str(), csStr);
	GetEncodeString(csStr.c_str(), &pBase64);
	if (pBase64 != NULL)
	{
		Convert(pBase64, cswStr);
		free(pBase64);

		WritePrivateProfileString(
			_T("LocalSet")
			, _T("Title")
			, cswStr.c_str()
			, wcFileName
		);
	}


	Convert(m_cswPassword.c_str(), csStr);
	GetEncodeString(csStr.c_str(), &pBase64);
	if (pBase64 != NULL)
	{
		Convert(pBase64, cswStr);
		free(pBase64);

		WritePrivateProfileString(
			_T("LocalSet")
			, _T("Password")
			, cswStr.c_str()
			, wcFileName
		);
	}

	csStrTmp.Format(L"%.06f", m_d64BackupExposure);
	WritePrivateProfileString(
		_T("CameraSet")
		, _T("Exposure")
		, csStrTmp.GetString()
		, wcFileName
	);

	csStrTmp.Format(L"%.06f", m_d64BackupGain);
	WritePrivateProfileString(
		_T("CameraSet")
		, _T("Gain")
		, csStrTmp.GetString()
		, wcFileName
	);

	csStrTmp.Format(L"%.06f", m_d64BackupFPS);
	WritePrivateProfileString(
		_T("CameraSet")
		, _T("FrameRate")
		, csStrTmp.GetString()
		, wcFileName
	);

	csStrTmp.Format(L"%d", m_u32BackupWidth);
	WritePrivateProfileString(
		_T("CameraSet")
		, _T("Width")
		, csStrTmp.GetString()
		, wcFileName
	);

	csStrTmp.Format(L"%d", m_u32BackupHeight);
	WritePrivateProfileString(
		_T("CameraSet")
		, _T("Height")
		, csStrTmp.GetString()
		, wcFileName
	);
	return 0;
}
INT CPlayerDlg::GetConfig(void)
{
	wchar_t wcFileName[512] = { 0 };
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
	m_cswSaveFolder = wcValue;


	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("FolderMaxSize")
		, _T("20480MB")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	swscanf(wcValue, L"%lldMB", &m_u64FolderMaxSize);
	if (m_u64FolderMaxSize < 20 * 1024)
	{
		m_u64FolderMaxSize = 20 * 1024;
	}
	PRINT("FolderMaxSize: %lldMB\n", m_u64FolderMaxSize);

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("SaveContinusTime")
		, _T("5min")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	swscanf(wcValue, L"%dmin", &m_u32SaveContinusTime);
	if (m_u32SaveContinusTime < 1)
	{
		m_u32SaveContinusTime = 1;
	}
	else if (m_u32SaveContinusTime > 10)
	{
		m_u32SaveContinusTime = 10;
	}
	PRINT("SaveContinusTime: %d\n", m_u32SaveContinusTime);

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("Title")
		, _T(DEFALT_TITLE)
		, wcValue, 512 - 2
		, wcFileName
	);
	{
		Convert(wcValue, csStr);
		PRINT("Password: %s\n", csStr.c_str());
		char *pPassword = NULL;
		GetDecodePassword(csStr.c_str(), &pPassword);
		if (pPassword == NULL)
		{
			ShowErrorMsg(L"password fromate error", 0);
			GetDecodePassword(DEFALT_TITLE, &pPassword);
		}
		Convert(pPassword, m_cswTitle);
		free(pPassword);
	}

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("Password")
		, _T(DEFALT_PASSWORD)
		, wcValue, 256 - 2
		, wcFileName
	);
	{
		Convert(wcValue, csStr);
		PRINT("Password: %s\n", csStr.c_str());
		char *pPassword = NULL;
		GetDecodePassword(csStr.c_str(), &pPassword);
		if (pPassword == NULL)
		{
			ShowErrorMsg(L"password fromate error", 0);
			GetDecodePassword(DEFALT_PASSWORD, &pPassword);
		}
		Convert(pPassword, m_cswPassword);
		free(pPassword);
	}

	dwRet = GetPrivateProfileString(
		_T("CameraSet")
		, _T("Exposure")
		, _T("4000")
		, wcValue, 256 - 2
		, wcFileName
	);
	m_d64BackupExposure = _wtof(wcValue);

	dwRet = GetPrivateProfileString(
		_T("CameraSet")
		, _T("Gain")
		, _T("15")
		, wcValue, 256 - 2
		, wcFileName
	);
	m_d64BackupGain = _wtof(wcValue);

	dwRet = GetPrivateProfileString(
		_T("CameraSet")
		, _T("FrameRate")
		, _T("200")
		, wcValue, 256 - 2
		, wcFileName
	);
	m_d64BackupFPS = _wtof(wcValue);

	dwRet = GetPrivateProfileString(
		_T("CameraSet")
		, _T("Width")
		, _T("336")
		, wcValue, 256 - 2
		, wcFileName
	);
	m_u32BackupWidth = _wtoi(wcValue);

	dwRet = GetPrivateProfileString(
		_T("CameraSet")
		, _T("Height")
		, _T("256")
		, wcValue, 256 - 2
		, wcFileName
	);
	m_u32BackupHeight = _wtoi(wcValue);

	return 0;
}


void CPlayerDlg::LocalPlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable/* = TRUE*/)
{
	const UINT32 u32ID[] =
	{
		IDC_BTN_PrevFrame,
		IDC_BTN_PlayPause,
		IDC_BTN_NextFrame,
		IDC_BTN_FrameJump,
		IDC_BTN_FPSRateSet,
		IDC_EDIT_FrameJump,
		IDC_EDIT_FPSRate,
		IDC_SLIDER_PlayPos,

		IDC_BTN_LocalOpenClose,

		IDC_BTN_LocalSearch,
	};
	UINT32 u32Count = 0;
	if (u32Level == 0)
	{
		u32Count = 8;
	}
	else
	{
		u32Count = 10;
	}
	for (UINT32 i = 0; i < u32Count; i++)
	{
		GetDlgItem(u32ID[i])->EnableWindow(boIsEnable);
	}

	if (!boIsEnable)
	{
		GetDlgItem(IDC_STATIC_CurFrame)->SetWindowText(L"0");
		GetDlgItem(IDC_STATIC_TotalFrame)->SetWindowText(L"0");
		GetDlgItem(IDC_EDIT_FrameJump)->SetWindowText(L"");
		GetDlgItem(IDC_EDIT_FPSRate)->SetWindowText(L"");
		((CSliderCtrl *)GetDlgItem(IDC_SLIDER_PlayPos))->SetRange(0, 100);
		((CSliderCtrl *)GetDlgItem(IDC_SLIDER_PlayPos))->SetPos(0);
	}

}

void CPlayerDlg::DevicePlayWidgetEnable(UINT32 u32Level, BOOL boIsEnable/* = TRUE*/)
{
	const UINT32 u32ID[] =
	{
		IDC_BTN_DeviceCapture,
		IDC_BTN_DeviceParamGet,
		IDC_BTN_DeviceParamSet,
		IDC_EDIT_Exposure,
		IDC_EDIT_Gain,
		IDC_EDIT_FPS,
		IDC_EDIT_Width,
		IDC_EDIT_Height,

		IDC_BTN_DeviceOpenClose,
	};
	UINT32 u32Count = 0;
	if (u32Level == 0)
	{
		u32Count = 8;
	}
	else
	{
		u32Count = 9;
	}
	for (UINT32 i = 0; i < u32Count; i++)
	{
		GetDlgItem(u32ID[i])->EnableWindow(boIsEnable);
	}
}


void CPlayerDlg::ShowErrorMsg(wchar_t *pMsg, INT32 s32ErrorNo)
{
	if (m_pDlgShow == NULL)
	{
		return;
	}
	CString csStrMsg;
	if (s32ErrorNo == 0)
	{
		csStrMsg.Format(_T("%s"), pMsg);
	}
	else
	{
		csStrMsg.Format(_T("%s: Error = %x: "), pMsg, s32ErrorNo);
	}

	switch (s32ErrorNo)
	{
	case MV_E_HANDLE:
	{
		csStrMsg += L"Error or invalid handle ";
		break;
	}
	case MV_E_SUPPORT:
	{
		csStrMsg += L"Not supported function ";
		break;
	}
	case MV_E_BUFOVER:
	{
		csStrMsg += L"Cache is full ";
		break;
	}
	case MV_E_CALLORDER:
	{
		csStrMsg += L"Function calling order error ";
		break;
	}
	case MV_E_PARAMETER:
	{        csStrMsg += L"Incorrect parameter ";
	break;
	}
	case MV_E_RESOURCE:
	{
		csStrMsg += L"Applying resource failed ";
		break;
	}
	case MV_E_NODATA:
	{
		csStrMsg += L"No data ";
		break;
	}
	case MV_E_PRECONDITION:
	{
		csStrMsg += L"Precondition error, or running environment changed ";
		break;
	}
	case MV_E_VERSION:
	{ 
		csStrMsg += L"Version mismatches ";
		break;
	}
	case MV_E_NOENOUGH_BUF:
	{
		csStrMsg += L"Insufficient memory ";
		break;
	}
	case MV_E_ABNORMAL_IMAGE:
	{
		csStrMsg += L"Abnormal image, maybe incomplete image because of lost packet ";
		break;
	}
	case MV_E_UNKNOW:
	{
		csStrMsg += L"Unknown error ";
		break;
	}
	case MV_E_GC_GENERIC:
	{
		csStrMsg += L"General error ";
		break;
	}
	case MV_E_GC_ACCESS:
	{
		csStrMsg += L"Node accessing condition error ";
		break;
	}
	case MV_E_ACCESS_DENIED:
	{
		csStrMsg += L"No permission ";
		break;
	}
	case MV_E_BUSY:
	{
		csStrMsg += L"Device is busy, or network disconnected ";
		break;
	}
	case MV_E_NETER:
	{
		csStrMsg += L"Network error ";
		break;
	}
	}

	m_u32ErrorTime = (UINT32)timeGetTime();

	m_pDlgShow->SetPaneText(_StatusBar_ErrorMessage, csStrMsg, TRUE);
}


INT CPlayerDlg::OpenDevice(void)
{
	int nIndex = m_csCBDevList.GetCurSel();
	if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
	{
		ShowErrorMsg(TEXT("Please select device"), 0);
		return STATUS_ERROR;
	}

	// ch:由设备信息创建设备实例 | en:Device instance created by device information
	if (NULL == m_stDevList.pDeviceInfo[nIndex])
	{
		ShowErrorMsg(TEXT("Device does not exist"), 0);
		return STATUS_ERROR;
	}

	if (NULL != m_pMyCamera)
	{
		return STATUS_ERROR;
	}

	m_pMyCamera = new CMyCamera;
	if (NULL == m_pMyCamera)
	{
		return STATUS_ERROR;
	}

	int nRet = m_pMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
	if (MV_OK != nRet)
	{
		delete m_pMyCamera;
		m_pMyCamera = NULL;
		ShowErrorMsg(TEXT("Open Device fail!"), nRet);
		return nRet;
	}

	// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
	if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
	{
		int nPacketSize = m_pMyCamera->GetOptimalPacketSize();
		if (nPacketSize > 0)
		{
			nRet = m_pMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
			if (nRet != MV_OK)
			{
				ShowErrorMsg(TEXT("Warning: Set Packet Size fail!"), nRet);
			}
		}
		else
		{
			ShowErrorMsg(TEXT("Warning: Get Packet Size fail!"), nPacketSize);
		}
	}

	return MV_OK;
}

INT CPlayerDlg::CloseDevice(void)
{
	if (m_pMyCamera != NULL)
	{
		m_pMyCamera->Close();
		delete m_pMyCamera;
		m_pMyCamera = NULL;
	}

	return MV_OK;
}

INT CPlayerDlg::StartCapture(void)
{
	if (m_pMyCamera == NULL)
	{
		return -1;
	}
	int nRet = 0;

	nRet = m_pMyCamera->SetEnumValue("TriggerMode", MV_TRIGGER_MODE_OFF);
	nRet = SetFrameFormat(PixelType_Gvsp_RGB8_Packed);
	nRet = m_pMyCamera->RegisterImageCallBack(ImageCallBack, this);

	nRet = m_pMyCamera->StartGrabbing();
	if (nRet == MV_OK)
	{
		/* <TODO> */
		m_csPlayCtrl.BeginRender(m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->
			GetSafeHwnd());
	}
	return nRet;

}

INT CPlayerDlg::StopCapture(void)
{
	if (m_pMyCamera == NULL)
	{
		return -1;
	}
	int nRet = 0;

	nRet = m_pMyCamera->StopGrabbing();
	if (nRet == MV_OK)
	{
		m_csPlayCtrl.StopRender();
	}

	return nRet;
	
}



// ch:获取曝光时间 | en:Get Exposure Time
int CPlayerDlg::GetExposureTime(void)
{
	float  fFloatValue = 0.0;
	int nRet = m_pMyCamera->GetFloatValue("ExposureTime", &fFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	m_d64Exposure = fFloatValue;

	return MV_OK;
}

// ch:设置曝光时间 | en:Set Exposure Time
INT CPlayerDlg::SetExposureTime(double *pData/* = NULL*/)
{
	// ch:调节这两个曝光模式，才能让曝光时间生效
	// en:Adjust these two exposure mode to allow exposure time effective
	int nRet = m_pMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	nRet = m_pMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

	double d64Data = m_d64Exposure;
	if (pData != NULL)
	{
		d64Data = *pData;
	}

	nRet = m_pMyCamera->SetFloatValue("ExposureTime", (float)d64Data);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	return MV_OK;
}

// ch:获取增益 | en:Get Gain
INT CPlayerDlg::GetGain(void)
{
	float  fFloatValue = 0.0;

	int nRet = m_pMyCamera->GetFloatValue("Gain", &fFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	m_d64Gain = (int)fFloatValue;

	return MV_OK;
}

// ch:设置增益 | en:Set Gain
INT CPlayerDlg::SetGain(double *pData/* = NULL*/)
{
	// ch:设置增益前先把自动增益关闭，失败无需返回
	//en:Set Gain after Auto Gain is turned off, this failure does not need to return
	int nRet = m_pMyCamera->SetEnumValue("GainAuto", 0);

	double d64Data = m_d64Gain;
	if (pData != NULL)
	{
		d64Data = *pData;
	}
	return m_pMyCamera->SetFloatValue("Gain", (float)d64Data);
}

// ch:获取帧率 | en:Get Frame Rate
INT CPlayerDlg::GetFrameRate(void)
{

	float  fFloatValue = 0.0;

	int nRet = m_pMyCamera->GetFloatValue("ResultingFrameRate", &fFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	m_d64FPS = fFloatValue;

	return MV_OK;
}

// ch:设置帧率 | en:Set Frame Rate
INT CPlayerDlg::SetFrameRate(double *pData/* = NULL*/)
{
	int nRet = m_pMyCamera->SetBoolValue("AcquisitionFrameRateEnable", true);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	double d64Data = m_d64FPS;
	if (pData != NULL)
	{
		d64Data = *pData;
	}
	return m_pMyCamera->SetFloatValue("AcquisitionFrameRate", (float)d64Data);
}

INT CPlayerDlg::GetFrameFormat(void)
{
	unsigned int nType = 0;
	int nRet = m_pMyCamera->GetEnumValue("PixelFormat", &nType);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	return nType;
}
INT CPlayerDlg::SetFrameFormat(MvGvspPixelType emType)
{
	unsigned int nType = emType;
	return m_pMyCamera->SetEnumValue("PixelFormat", nType);
}
INT CPlayerDlg::GetWidth(void)
{
	return 0;
}
INT CPlayerDlg::SetWidth(UINT32 *pData/* = NULL*/)
{
	unsigned int u32Data = m_u32BackupWidth;
	if (pData != NULL)
	{
		u32Data = *pData;
	}
	return m_pMyCamera->SetIntValue("WidthMax", u32Data);

}
INT CPlayerDlg::GetHeight(void)
{
	return 0;

}
INT CPlayerDlg::SetHeight(UINT32 *pData/* = NULL*/)
{
	unsigned int u32Data = m_u32BackupHeight;
	if (pData != NULL)
	{
		u32Data = *pData;
	}
	return m_pMyCamera->SetIntValue("HeightMax", u32Data);
}


void CPlayerDlg::OnBnClickedBtnDeviceparamget()
{
	// TODO: 在此添加控件通知处理程序代码

	int nRet;
	nRet = GetExposureTime();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Exposure Time Fail"), nRet);
	}

	nRet = GetGain();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Gain Fail"), nRet);
	}

	nRet = GetFrameRate();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Frame Rate Fail"), nRet);
	}

	nRet = GetFrameFormat();
	{
		MvGvspPixelType emType = ((MvGvspPixelType)nRet);
		emType = emType;
	}
	UpdateData(FALSE);

}


void CPlayerDlg::OnBnClickedBtnDeviceparamset()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	bool bIsSetSucceed = true;
	int nRet = SetExposureTime();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Exposure Time Fail"), nRet);
	}
	nRet = SetGain();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Gain Fail"), nRet);
	}
	nRet = SetFrameRate();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Frame Rate Fail"), nRet);
	}

	if (bIsSetSucceed)
	{
		ShowErrorMsg(TEXT("Set Parameter Succeed"), nRet);
	}

	if (m_u32Height > 512 || m_u32Width > 672)
	{
		MessageBox(L"分辨率超出范围", L"错误", MB_ICONERROR);
	}

	m_u32BackupHeight = m_u32Height;
	m_u32BackupWidth = m_u32Width;
	m_d64BackupExposure = m_d64Exposure;
	m_d64BackupGain = m_d64Gain;
	m_d64BackupFPS = m_d64FPS;

	SetConfig();
}


void CPlayerDlg::OnBnClickedBtnLocalset()
{
	// TODO: 在此添加控件通知处理程序代码

	CDlgLocalSet csDlg;
	csDlg.m_csStrSaveFolder = m_cswSaveFolder.c_str();
	csDlg.m_u32FolderSize = ((UINT)(m_u64FolderMaxSize / 1024));
	csDlg.m_s32RecordContinusTime = m_u32SaveContinusTime - 1;
	csDlg.m_csStrTitle = m_cswTitle.c_str();
	csDlg.m_csStrPasswordOld = m_cswPassword.c_str();

	int nRet = csDlg.DoModal();
	if (nRet != IDOK)
	{
		nRet = nRet;
		return;
	}

	bool boNeedSaveConfig = false;

	if ((UINT)(m_u64FolderMaxSize / 1024) != csDlg.m_u32FolderSize)
	{
		m_u64FolderMaxSize = csDlg.m_u32FolderSize;
		m_u64FolderMaxSize *= 1024;
	}

	if (m_cswSaveFolder != csDlg.m_csStrSaveFolder.GetString())
	{
		m_cswSaveFolder = csDlg.m_csStrSaveFolder.GetString();
		m_csPlayCtrl.SetFolder(m_cswSaveFolder.c_str());
		boNeedSaveConfig = true;
	}

	if (csDlg.m_s32RecordContinusTime != (int)(m_u32SaveContinusTime - 1))
	{
		m_u32SaveContinusTime = csDlg.m_s32RecordContinusTime + 1;
		if (m_u32SaveContinusTime < 1)
		{
			m_u32SaveContinusTime = 1;
		}
		else if (m_u32SaveContinusTime > 10)
		{
			m_u32SaveContinusTime = 10;
		}

		m_csPlayCtrl.SetSaveContinusTime(m_u32SaveContinusTime);
		boNeedSaveConfig = true;
	}

	if (csDlg.m_csStrTitle != m_cswTitle.c_str())
	{
		m_cswTitle = csDlg.m_csStrTitle.GetString();
		if (m_pDlgShow != NULL)
		{
			m_pDlgShow->SetWindowText(csDlg.m_csStrTitle);
		}
		boNeedSaveConfig = true;
	}

	if (csDlg.m_csStrNewPW == csDlg.m_csStrNewPWRepeat)
	{
		if (csDlg.m_csStrNewPW != m_cswPassword.c_str() 
			&& csDlg.m_csStrNewPW.GetLength() != 0)
		{
			m_cswPassword = csDlg.m_csStrNewPW.GetString();
			boNeedSaveConfig = true;
		}
	}

	if (boNeedSaveConfig)
	{
		SetConfig();
	}

}

void CPlayerDlg::OnBnClickedBtnHide()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
	if (m_pDlgShow != NULL)
	{
		m_pDlgShow->ShowWindow(SW_MAXIMIZE);
	}
}



void CPlayerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedBtnHide();

	CDialogEx::OnClose();
}

void CPlayerDlg::RegAutoRun()
{
	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL, hExeKey = NULL;
	TCHAR *pRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";
	TCHAR tcFileFullName[MAX_PATH] = { 0 };
	TCHAR tcFileName[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, tcFileFullName, MAX_PATH);

	TCHAR *pStr = wcsrchr(tcFileFullName, L'\\');
	if (pStr != NULL)
	{
		memcpy(tcFileName, pStr + 1, sizeof(TCHAR) * (wcsrchr(tcFileFullName, L'.') - pStr - 1));
	}
	BOOL boFindKey = FALSE;
	do
	{
		LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, pRun, 0, KEY_READ, &hKey);
		if (lRet != ERROR_SUCCESS)
		{
			break;
		}
		TCHAR tcValue[MAX_PATH] = { 0 };
		DWORD dLen = MAX_PATH;
		lRet = RegGetValue(hKey, NULL, tcFileName, RRF_RT_REG_SZ, NULL, tcValue, &dLen);
		if (lRet != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			break;
		}
		if (wcscmp(tcFileFullName, tcValue) == 0)
		{
			boFindKey = TRUE;
		}

		RegCloseKey(hKey);
	} while (0);
	if (!boFindKey)
	{
		LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, pRun, 0, KEY_WRITE, &hKey);
		if (lRet != ERROR_SUCCESS)
		{
			MessageBox(L"请使用管理员权限重新打开并设置", L"提醒");
			return;
		}
		lRet = RegSetValueEx(hKey, tcFileName, 0, REG_SZ, (const BYTE*)tcFileFullName, MAX_PATH);
		RegCloseKey(hKey);
	}
	//else
	//{
	//	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, pRun, 0, KEY_WRITE, &hKey);
	//	if (lRet != ERROR_SUCCESS)
	//	{
	//		MessageBox("请使用管理员权限重新打开并设置", "提醒");
	//		return;
	//	}
	//	lRet = RegDeleteValue(hKey, tcFileName);
	//	RegCloseKey(hKey);
	//}
}


BOOL CPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
