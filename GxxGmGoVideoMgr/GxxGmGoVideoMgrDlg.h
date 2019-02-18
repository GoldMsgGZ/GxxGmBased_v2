
// GxxGmGoVideoMgrDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "GxxGmGoVideoLoginDlg.h"
#include "GxxGmDevMgrDlg.h"
#include "GxxGmServiceStateDlg.h"

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"

#include "Poco/Util/IniFileConfiguration.h"


// CGxxGmGoVideoMgrDlg 对话框
class CGxxGmGoVideoMgrDlg : public CDialog
{
// 构造
public:
	CGxxGmGoVideoMgrDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GXXGMGOVIDEOMGR_DIALOG };

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
	CTabCtrl m_cTabMain;

public:
	GxxGmGoVideo govideo_;

public:
	Poco::AutoPtr<Poco::Util::IniFileConfiguration> ini_cfg_;

public:
	CGxxGmGoVideoLoginDlg login_dlg_;
	CGxxGmDevMgrDlg dev_mgr_dlg_;
	CGxxGmServiceStateDlg service_state_dlg_;
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
};
