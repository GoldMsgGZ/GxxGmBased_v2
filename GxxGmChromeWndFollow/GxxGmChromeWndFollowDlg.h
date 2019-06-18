
// GxxGmChromeWndFollowDlg.h : 头文件
//

#pragma once


// CGxxGmChromeWndFollowDlg 对话框
class CGxxGmChromeWndFollowDlg : public CDialog
{
// 构造
public:
	CGxxGmChromeWndFollowDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GXXGMCHROMEWNDFOLLOW_DIALOG };

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
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

};
