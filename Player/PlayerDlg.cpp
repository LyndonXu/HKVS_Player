
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
END_MESSAGE_MAP()


int32_t GetFolderSizeCB(const WCHAR *pName, WIN32_FIND_DATA *pInfo, void *pContext)
{
	TRACE(L"%s\n", pName);

	return 0;
}

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


	int64_t s64Size = GetFolderSize(L"G:\\GBServer"/*, GetFolderSizeCB*/);
	TRACE(L"G:\\GBServer size is:%lld\n", s64Size);

	SetTimer(1, 1000, NULL);
	SetTimer(2, 15, NULL);

	SetWindowText(L"1231312");
	m_csPlayCtrl.BeginRender(GetDlgItem(IDC_STATIC_Movie)->GetSafeHwnd());

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

	return CDialogEx::DestroyWindow();
}


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
		for (INT  i = 0; i < 3; i++)
		{
			static UINT32 u32Cnt = 0;
			static unsigned char u8Buf[800 * 600 * 3];
			unsigned char *pTmp = u8Buf;
			for (int32_t j = 0; j < 800 * 600; j++, pTmp += 3)
			{
				pTmp[0] = 0;
				pTmp[1] = 0;
				pTmp[2] = u32Cnt;
			}
			u32Cnt++;
			m_csPlayCtrl.SendShareData(u8Buf, 800 * 600 * 3);
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}
