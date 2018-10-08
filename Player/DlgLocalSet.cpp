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
}


BEGIN_MESSAGE_MAP(CDlgLocalSet, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SaveFolderSelect, &CDlgLocalSet::OnBnClickedBtnSavefolderselect)
	ON_BN_CLICKED(IDOK, &CDlgLocalSet::OnBnClickedOk)
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

	CDialogEx::OnOK();
}
