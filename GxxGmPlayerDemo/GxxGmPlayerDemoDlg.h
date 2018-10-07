
// GxxGmPlayerDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "GxxGmGoVideo.h"
#include "afxcmn.h"


// CGxxGmPlayerDemoDlg 对话框
class CGxxGmPlayerDemoDlg : public CDialog
{
// 构造
public:
	CGxxGmPlayerDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GXXGMPLAYERDEMO_DIALOG };

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
	CEdit m_cGoVideoIP;
	CEdit m_cGoVideoPort;
	CEdit m_cVideoURL;
	//CListBox m_cOnlineDevices;
	CListBox m_cState;
	CListCtrl m_cOnlineDevices;

public:
	GxxGmGoVideo govideo;

public:
	afx_msg void OnBnClickedBtnLogin();
	
	afx_msg void OnBnClickedBtnGetOnlineDevice();
	
	afx_msg void OnNMDblclkListOnlineDev(NMHDR *pNMHDR, LRESULT *pResult);
};
