
// PlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"


#include "utils.h"


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CPlayerDlg �Ի���



CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAYER_DIALOG, pParent)
	, m_pDlgShow(NULL)
	, m_cswSaveFolder(L"F:\\")
	, m_u64FolderMaxSize(20 * 1024)
	, m_u32SaveContinusTime(5)

	, m_cswTitle(L"User Config")

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

	ON_BN_CLICKED(IDC_BTN_LocalOpenClose, &CPlayerDlg::OnBnClickedBtnLocalopenclose)
	ON_BN_CLICKED(IDC_BTN_FrameJump, &CPlayerDlg::OnBnClickedBtnFramejump)
	ON_BN_CLICKED(IDC_BTN_FPSRateSet, &CPlayerDlg::OnBnClickedBtnFpsrateset)
END_MESSAGE_MAP()



static UINT s_uIndicators[] =
{
	CPlayerDlg::_StatusBar_CPU,
	CPlayerDlg::_StatusBar_Memory,
	CPlayerDlg::_StatusBar_NetInterface,
};

// CPlayerDlg ��Ϣ�������

BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CDlgSearch csDlg;
	INT_PTR nRet = csDlg.DoModal();

	if (nRet = IDOK)
	{
		nRet = nRet;
	}
	

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
		//int s32Height = GetSystemMetrics(SM_CYSCREEN);

		CRect csClient;
		GetClientRect(csClient);

		int s32Left = (s32Width - csClient.Width()) / 2 - 8;
		int s32Top = 50 + 338;//s32Height - csClient.Height() - 60;
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
	GetConfig();

	SetTimer(1, 1000, NULL);

	//SetTimer(2, 40, NULL);

	SetWindowText(m_cswTitle.c_str());
	m_csPlayCtrl.RegisterWNDMsg(GetSafeHwnd(), PLAYCTRL_MSG);
	m_csPlayCtrl.SetFolder(m_cswSaveFolder.c_str());
	m_csPlayCtrl.SetSaveContinusTime(m_u32SaveContinusTime);
	if (m_pDlgShow != NULL)
	{
		//m_csPlayCtrl.BeginRender(m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->GetSafeHwnd());
	}
	//m_csPlayCtrl.BeginSave();
	//m_csPlayCtrl.BeginLocalPlay(L"F:\\SaveFolder\\2018-09-06 03-38-39_915.dat");

	LocalPlayWidgetEnable(0, FALSE);
	DevicePlayWidgetEnable(0, FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CPlayerDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

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
	m_cswSaveFolder = wcValue;


	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("FolderMaxSize")
		, _T("20480MB")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("FolderMaxSize: %s\n", csStr.c_str());
	swscanf(wcValue, L"%lldMB", &m_u64FolderMaxSize);

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("SaveContinusTime")
		, _T("5min")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("SaveContinusTime: %s\n", csStr.c_str());
	swscanf(wcValue, L"%dmin", &m_u32SaveContinusTime);

	dwRet = GetPrivateProfileString(
		_T("LocalSet")
		, _T("Title")
		, _T("User config")
		, wcValue, 256 - 2
		, wcFileName
	);

	Convert(wcValue, csStr);
	PRINT("Title: %s\n", csStr.c_str());
	m_cswTitle = wcValue;

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
					GetDlgItem(IDC_BTN_LocalOpenClose)->SetWindowText(L"�ر�");
				}
				if (lData == _PlayMode_Play)
				{
					GetDlgItem(IDC_BTN_PlayPause)->SetWindowText(L"��ͣ");
				}
				else
				{
					GetDlgItem(IDC_BTN_PlayPause)->SetWindowText(L"�ָ�");
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
			CString csStr;
			csStr.Format(L"%d", (UINT32)lData);
			GetDlgItem(IDC_STATIC_TotalFrame)->SetWindowText(csStr);
			break;
		}
		case _WND_Msg_LocalPlayFileCurIndex:
		{
			//PRINT("current Index: %d\n", lData);
			CString csStr;
			csStr.Format(L"%d", (UINT32)lData + 1);
			GetDlgItem(IDC_STATIC_CurFrame)->SetWindowText(csStr);
			break;
		}
		case _WND_Msg_ShowRectInvalidate:
		{
			m_pDlgShow->GetDlgItem(IDC_STATIC_BigMovie)->Invalidate();
			break;
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ


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

		csStr.Format(L"�ڴ�: %d%%", u32MemLoad);
		m_csStatusBar.SetPaneText(_StatusBar_Memory, csStr, TRUE);


		UINT32 u32InSpeed = 0;
		UINT32 u32OutSpeed = 0;

		m_csSysInfo.GetNetworkInfo(0, u32InSpeed, u32OutSpeed);

		csStr.Format(L"����: ����%dKbps, ����%dKbps", u32InSpeed / 1024, u32OutSpeed / 1024);
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

		IDC_BTN_LocalOpenClose,
		IDC_BTN_LocalSearch,
	};
	UINT32 u32Count = 0;
	if (u32Level == 0)
	{
		u32Count = 7;
	}
	else
	{
		u32Count = 9;
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

		IDC_BTN_DeviceOpenClose,
	};
	UINT32 u32Count = 0;
	if (u32Level == 0)
	{
		u32Count = 6;
	}
	else
	{
		u32Count = 7;
	}
	for (UINT32 i = 0; i < u32Count; i++)
	{
		GetDlgItem(u32ID[i])->EnableWindow(boIsEnable);
	}
}



void CPlayerDlg::OnBnClickedBtnNextframe()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_NextFrame);
}


void CPlayerDlg::OnBnClickedBtnPlaypause()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString csStr;
	GetDlgItem(IDC_BTN_PlayPause)->GetWindowTextW(csStr);
	if (csStr == L"��ͣ")
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_PrevFrame);
}


void CPlayerDlg::OnBnClickedBtnLocalopenclose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString csStr;
	GetDlgItem(IDC_BTN_LocalOpenClose)->GetWindowText(csStr);
	if (csStr == L"�ر�")
	{
		m_csPlayCtrl.StopLocalPlay();
		Sleep(60);/* let render receive all frame */
		m_csPlayCtrl.StopRender();

		LocalPlayWidgetEnable(0, FALSE);
		DevicePlayWidgetEnable(1, TRUE);
		DevicePlayWidgetEnable(0, FALSE);

		GetDlgItem(IDC_BTN_LocalOpenClose)->SetWindowText(L"��");

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

	m_csPlayCtrl.BeginLocalPlay(csStr.GetString());
}


void CPlayerDlg::OnBnClickedBtnFramejump()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString csStr;
	GetDlgItem(IDC_EDIT_FrameJump)->GetWindowText(csStr);

	UINT32 u32JumpNumber = _wtoi(csStr.GetString());

	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_Jump, (void *)u32JumpNumber);

}


void CPlayerDlg::OnBnClickedBtnFpsrateset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			MessageBox(L"�����ʽ����");
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
		MessageBox(L"�����ʽ����");
		return;
	}

	m_csPlayCtrl.SendLocalPlayMessage(_LocalPlay_Msg_ChangeRate, (void *)s32Rate);
}
