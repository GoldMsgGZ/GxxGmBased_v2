
// GB28181ClientPlatformDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "GB28181AgentSDK.h"
#include "DataStruct.h"

class DBBusiness;

// CGB28181ClientPlatformDlg 对话框
class CGB28181ClientPlatformDlg : public CDialog
{
// 构造
public:
	CGB28181ClientPlatformDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GB28181CLIENTPLATFORM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void BuildOrgTreeCtrl(struct OrgInfo root_org, HTREEITEM root_handle);

public:
	DBBusiness *db_business_;
	AGENT_HANDLE gb28181_handle_;

public:
	static void _AgentLogCallBack(EnumLogLevel eLevel, const char * szTemp, int iLen, void * pUserData);
	static SIP_REPSOND_CODE _DevInfoQueryCB(SESSION_HANDLE hSession, const char * czSrvGBCode, StruQueryReqDescri * stuQuery, void * pUserData);
	static SIP_REPSOND_CODE _DevControlCB(const char * czSrvGBCode, const StruDevCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _PlayControlCB(STREAM_HANDLE hStream, const StruPlayCtrlCmd * stuCmd, void * pUserData);
	static SIP_REPSOND_CODE _StreamRequestCB(STREAM_HANDLE hStream, const char * czSrvGBCode, EnumStreamRequest eRequest,	StruMediaInfo * pInMedia, const StruStreamDescription * pDescri, void * pUserData);
	static SIP_REPSOND_CODE _NotifyInfo_CallBackFunc(EnumNotifyType eType, const char * czSrvGBCode, void * pMsg, void * pUserData);
	static SIP_REPSOND_CODE _ExtendRqeustCallBack(SESSION_HANDLE hSession, EnumExtendType eType, const char * czTargetDevID, void * pMsg, void * pUserData);

public:
	void SetStatus(const char *status);

public:
	struct OrgInfo org_infos;
	StruCatalogInfo *org_and_devs;
	int GetOrgCounts(struct OrgInfo org_infos, unsigned int &count);
	StruCatalogInfo* BuildGB28181Orgs(struct OrgInfo org_infos, int count, int &index);


public:
	CEdit m_cCurrentIp;
	CEdit m_cCurrentPort;
	CEdit m_cCurrentGbcode;
	CEdit m_cRemoteIp;
	CEdit m_cRemotePort;
	CEdit m_cRemoteGbcode;
	CEdit m_cAuthenUsername;
	CEdit m_cAuthenPassword;

	CTreeCtrl m_cOrgAndDevAndChannel;
	HTREEITEM h_rootNode;

	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	CEdit m_cStatus;
};
