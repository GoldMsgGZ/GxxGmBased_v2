
// GB28181ServerPlatformDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

class GB28181ServerSignalObject;


// CGB28181ServerPlatformDlg 对话框
class CGB28181ServerPlatformDlg : public CDialog
{
// 构造
public:
	CGB28181ServerPlatformDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GB28181SERVERPLATFORM_DIALOG };

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
	GB28181ServerSignalObject *object_;

public:
	CEdit m_cServerIp;
	CEdit m_cServerPort;
	CEdit m_ccServerGbcode;
	CEdit m_cServerUsername;
	CEdit m_cServerPassword;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
};
