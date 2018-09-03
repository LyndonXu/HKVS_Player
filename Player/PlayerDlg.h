
// PlayerDlg.h : ͷ�ļ�
//

#pragma once
#include "GetSysInfo.h"

// CPlayerDlg �Ի���
class CPlayerDlg : public CDialogEx
{
// ����
public:
	CPlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
