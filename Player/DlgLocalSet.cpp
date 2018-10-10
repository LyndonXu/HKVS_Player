// DlgLocalSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Player.h"
#include "DlgLocalSet.h"
#include "afxdialogex.h"


// CDlgLocalSet �Ի���

IMPLEMENT_DYNAMIC(CDlgLocalSet, CDialogEx)

CDlgLocalSet::CDlgLocalSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_LocalSet, pParent)
	, m_csStrSaveFolder(_T(""))
	, m_u32FolderSize(20)
	, m_s32RecordContinusTime(0)
	, m_csStrTitle(_T(""))
	, m_csStrNewPW(_T(""))
	, m_csStrNewPWRepeat(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgLocalSet::~CDlgLocalSet()
{
}

void CDlgLocalSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SaveFolder, m_csEditSaveFolder);
	DDX_Text(pDX, IDC_EDIT_SaveFolder, m_csStrSaveFolder);
	DDX_Control(pDX, IDC_EDIT_FolderSize, m_csEditFolderSize);
	DDX_Text(pDX, IDC_EDIT_FolderSize, m_u32FolderSize);
	DDX_Control(pDX, IDC_COMBO_RecordContinusTime, m_csComboRecordContinusTime);
	DDX_CBIndex(pDX, IDC_COMBO_RecordContinusTime, m_s32RecordContinusTime);
	DDV_MinMaxInt(pDX, m_s32RecordContinusTime, 0, 9);
	DDX_Control(pDX, IDC_EDIT_Title, m_csEditTitle);
	DDX_Text(pDX, IDC_EDIT_Title, m_csStrTitle);
	DDX_Control(pDX, IDC_EDIT_Password, m_csEditPassword);
	DDX_Control(pDX, IDC_EDIT_NewPW, m_csEditNewPW);
	DDX_Control(pDX, IDC_EDIT_NewPWRepeat, m_csEditNewPWRepeat);
	DDX_Text(pDX, IDC_EDIT_NewPW, m_csStrNewPW);
	DDX_Text(pDX, IDC_EDIT_NewPWRepeat, m_csStrNewPWRepeat);
}


BEGIN_MESSAGE_MAP(CDlgLocalSet, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SaveFolderSelect, &CDlgLocalSet::OnBnClickedBtnSavefolderselect)
	ON_BN_CLICKED(IDOK, &CDlgLocalSet::OnBnClickedOk)
	ON_EN_UPDATE(IDC_EDIT_Password, &CDlgLocalSet::OnEnUpdateEditPassword)
END_MESSAGE_MAP()


// CDlgLocalSet ��Ϣ�������


BOOL CDlgLocalSet::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���


	return CDialogEx::DestroyWindow();
}


BOOL CDlgLocalSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}

	for (INT i = 0; i < 10; i++)
	{
		CString csStr;
		csStr.Format(L"%d����", i + 1);
		m_csComboRecordContinusTime.AddString(csStr);
	}

	UpdateData(FALSE);

	m_csEditPassword.m_csInitString = L"����������";
	m_csEditPassword.SetWindowText(m_csEditPassword.m_csInitString);

	m_csEditNewPW.m_csInitString = L"������";
	m_csEditNewPW.SetWindowText(m_csEditNewPW.m_csInitString);

	m_csEditNewPWRepeat.m_csInitString = L"�ٴ�����";
	m_csEditNewPWRepeat.SetWindowText(m_csEditNewPWRepeat.m_csInitString);

	m_csEditNewPW.EnableWindow(FALSE);
	m_csEditNewPWRepeat.EnableWindow(FALSE);
	m_csEditTitle.EnableWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgLocalSet::OnBnClickedBtnSavefolderselect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR           szFolderPath[MAX_PATH] = { 0 };

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("��ѡ��洢·��");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			m_csEditSaveFolder.SetWindowText(szFolderPath);
		}
	}
}


void CDlgLocalSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData();

	if (m_csStrNewPW != L"������" || m_csStrNewPWRepeat != L"�ٴ�����")
	{
		if (m_csStrNewPW != m_csStrNewPWRepeat)
		{
			int nRet = MessageBox(L"�����벻һ��, �Ƿ��˳���", L"����", MB_YESNO | MB_ICONWARNING);
			if (nRet == IDNO)
			{
				return;
			}
		}
	}

	CDialogEx::OnOK();
}


void CDlgLocalSet::OnEnUpdateEditPassword()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// �������Խ� EM_SETEVENTMASK ��Ϣ���͵��ÿؼ���
	// ͬʱ�� ENM_UPDATE ��־�������㵽 lParam �����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	CString csStr;
	m_csEditPassword.GetWindowTextW(csStr);
	if (csStr == m_csStrPasswordOld)
	{
		m_csEditNewPW.EnableWindow(TRUE);
		m_csEditNewPWRepeat.EnableWindow(TRUE);
		m_csEditTitle.EnableWindow(TRUE);
	}
	else
	{
		m_csEditNewPW.EnableWindow(FALSE);
		m_csEditNewPWRepeat.EnableWindow(FALSE);
		m_csEditTitle.EnableWindow(FALSE);
	}
}


BOOL CDlgLocalSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
