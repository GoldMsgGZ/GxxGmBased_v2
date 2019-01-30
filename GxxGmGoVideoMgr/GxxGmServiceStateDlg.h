#pragma once

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"
#include "afxcmn.h"

// CGxxGmServiceStateDlg 对话框

class CGxxGmServiceStateDlg : public CDialog
{
	DECLARE_DYNAMIC(CGxxGmServiceStateDlg)

public:
	CGxxGmServiceStateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGxxGmServiceStateDlg();

// 对话框数据
	enum { IDD = IDD_CHILDDLG_SERVICESTATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	GxxGmGoVideo *govideo_;

public:
	CListCtrl m_cRunningServiceList;
	afx_msg void OnBnClickedBtnRefreshOnlineservice();
	afx_msg void OnLvnKeydownListServices(NMHDR *pNMHDR, LRESULT *pResult);
};
