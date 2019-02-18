// GxxGmGoVideoLoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmGoVideoMgr.h"
#include "GxxGmGoVideoLoginDlg.h"


// CGxxGmGoVideoLoginDlg 对话框

IMPLEMENT_DYNAMIC(CGxxGmGoVideoLoginDlg, CDialog)

CGxxGmGoVideoLoginDlg::CGxxGmGoVideoLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmGoVideoLoginDlg::IDD, pParent)
	, govideo_(NULL)
{

}

CGxxGmGoVideoLoginDlg::~CGxxGmGoVideoLoginDlg()
{
}

void CGxxGmGoVideoLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_cGoVideoIp);
	DDX_Control(pDX, IDC_EDIT_PWD, m_cGoVideoPort);
}


BEGIN_MESSAGE_MAP(CGxxGmGoVideoLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CGxxGmGoVideoLoginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CGxxGmGoVideoLoginDlg::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CGxxGmGoVideoLoginDlg 消息处理程序
BOOL CGxxGmGoVideoLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_cGoVideoIp.LimitText(16);
	m_cGoVideoPort.LimitText(6);

	// 读取上次登录的地址
	USES_CONVERSION;
	TCHAR ip[4096] = {0};
	TCHAR port[4096] = {0};
	GetPrivateProfileString(_T("LOGIN"), _T("IP"), _T("0.0.0.0"), ip, 4096, A2T(config_path_.c_str()));
	GetPrivateProfileString(_T("LOGIN"), _T("PORT"), _T("99"), port, 4096, A2T(config_path_.c_str()));
	//std::string ip = ini_cfg_->getString("LOGIN.IP", "127.0.0.1");
	//std::string port = ini_cfg_->getString("LOGIN.PORT", "127.0.0.1");

	m_cGoVideoIp.SetWindowText(ip);
	m_cGoVideoPort.SetWindowText(port);

	return TRUE;
}

void CGxxGmGoVideoLoginDlg::OnBnClickedBtnLogin()
{
	CString ipaddr;
	m_cGoVideoIp.GetWindowText(ipaddr);

	CString ipport;
	m_cGoVideoPort.GetWindowText(ipport);

	// 尝试登录
	USES_CONVERSION;
	int errCode = govideo_->Login(T2A(ipaddr.GetBuffer(0)), _ttoi(ipport.GetString()));
	if (errCode != 0)
		MessageBox(_T("登录错误！"), _T("错误"), MB_OK|MB_ICONERROR);
	else
	{
		// 保存参数
		//ini_cfg_->setString("LOGIN.IP", T2A(ipaddr.GetBuffer(0)));
		//ini_cfg_->setString("LOGIN.PORT", T2A(ipport.GetBuffer(0)));
		WritePrivateProfileString(_T("LOGIN"), _T("IP"), ipaddr.GetBuffer(0), A2T(config_path_.c_str()));
		WritePrivateProfileString(_T("LOGIN"), _T("PORT"), ipport.GetBuffer(0), A2T(config_path_.c_str()));
		CDialog::OnOK();
	}
}

void CGxxGmGoVideoLoginDlg::OnBnClickedBtnCancel()
{
	CDialog::OnCancel();
}
