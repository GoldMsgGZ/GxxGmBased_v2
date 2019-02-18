#pragma once
#include "afxwin.h"

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"

#include "Poco/Util/IniFileConfiguration.h"


// CGxxGmGoVideoLoginDlg 对话框

class CGxxGmGoVideoLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CGxxGmGoVideoLoginDlg)

public:
	CGxxGmGoVideoLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGxxGmGoVideoLoginDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	GxxGmGoVideo *govideo_;

public:
	std::string config_path_;
	Poco::AutoPtr<Poco::Util::IniFileConfiguration> ini_cfg_;

public:
	CEdit m_cGoVideoIp;
	CEdit m_cGoVideoPort;
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnCancel();
};
