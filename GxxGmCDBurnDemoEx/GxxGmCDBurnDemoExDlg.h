
// GxxGmCDBurnDemoExDlg.h : 头文件
//

#pragma once
#include "cdrecordocxctrl1.h"
#include "afxwin.h"


// CGxxGmCDBurnDemoExDlg 对话框
class CGxxGmCDBurnDemoExDlg : public CDialog
{
// 构造
public:
	CGxxGmCDBurnDemoExDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GXXGMCDBURNDEMOEX_DIALOG };

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
	CCdrecordocxctrl1 m_cPlugin;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnInit();
	afx_msg void OnBnClickedBtnInstallCallback();
	afx_msg void OnBnClickedBtnFindDevice();
	afx_msg void OnBnClickedBtnBurn();
	CListBox m_cState;
	afx_msg void OnBnClickedBtnCheckCd();
};
