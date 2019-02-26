#pragma once
#include "afxcmn.h"

#include "..\GxxGmGoVideo\GxxGmGoVideo.h"
#include "afxwin.h"

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

public:
	CListCtrl m_cDGWList;
	CListCtrl m_cDevList;
	int current_select_dgw_id_;

	CEdit m_cDevID;
	CEdit m_cDevName;
	CComboBox m_cModelID;
	CComboBox m_cCategoryID;
	CEdit m_cConnInfo;
	CEdit m_cCfgVersion;
	CEdit m_cUsername;
	CEdit m_cPassword;
	CEdit m_cDevCode;
	CEdit m_cExtInfo;
	CEdit m_cDevGBCdeo;
	CEdit m_cNameAbbr;
	CEdit m_cDevVersion;
	CComboBox m_cDGWs;

public:
	afx_msg void OnBnClickedBtnRefreshServices();
	afx_msg void OnBnClickedBtnRefreshDevices();
	afx_msg void OnNMClickListDgws(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnModify();
	
	afx_msg void OnLvnKeydownListDevs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListDevs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnExportDevList();
	afx_msg void OnBnClickedBtnImportDevList();
};
