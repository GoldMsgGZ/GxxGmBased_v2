
// GxxGmChromeWndFollowDlg.h : ͷ�ļ�
//

#pragma once


// CGxxGmChromeWndFollowDlg �Ի���
class CGxxGmChromeWndFollowDlg : public CDialog
{
// ����
public:
	CGxxGmChromeWndFollowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GXXGMCHROMEWNDFOLLOW_DIALOG };

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
};
