#pragma once
#include "afxcmn.h"

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"

// CGxxGmDevMgrDlg 对话框

class CGxxGmDevMgrDlg : public CDialog
{
	DECLARE_DYNAMIC(CGxxGmDevMgrDlg)

public:
	CGxxGmDevMgrDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGxxGmDevMgrDlg();

// 对话框数据
	enum { IDD = IDD_CHILDDLG_DEVMGR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	GxxGmGoVideo *govideo_;
	CListCtrl m_cDGWList;
	CListCtrl m_cDevList;

public:
	afx_msg void OnBnClickedBtnRefreshServices();
	afx_msg void OnBnClickedBtnRefreshDevices();
	afx_msg void OnNMClickListDgws(NMHDR *pNMHDR, LRESULT *pResult);
};
