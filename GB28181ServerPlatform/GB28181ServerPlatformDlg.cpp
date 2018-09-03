
// GB28181ServerPlatformDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GB28181ServerPlatform.h"
#include "GB28181ServerPlatformDlg.h"

#include "GB28181ServerSignalObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGB28181ServerPlatformDlg 对话框




CGB28181ServerPlatformDlg::CGB28181ServerPlatformDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGB28181ServerPlatformDlg::IDD, pParent)
	, object_(new GB28181ServerSignalObject())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGB28181ServerPlatformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERVER_IP, m_cServerIp);
	DDX_Control(pDX, IDC_EDIT_SERVER_PORT, m_cServerPort);
	DDX_Control(pDX, IDC_EDIT_SERVER_GBCODE, m_ccServerGbcode);
	DDX_Control(pDX, IDC_EDIT_SERVER_USERNAME, m_cServerUsername);
	DDX_Control(pDX, IDC_EDIT_SERVER_PASSWORD, m_cServerPassword);
}

BEGIN_MESSAGE_MAP(CGB28181ServerPlatformDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CGB28181ServerPlatformDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CGB28181ServerPlatformDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CGB28181ServerPlatformDlg 消息处理程序

BOOL CGB28181ServerPlatformDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 得到当前程序所在目录
	TCHAR current_program_path[4096] = {0};
	GetModuleFileName(NULL, current_program_path, 4096);
#ifdef UNICODE
	std::wstring current_program_path_stl = current_program_path;
	int pos = current_program_path_stl.find_last_of(L"\\");
	std::wstring current_program_dir = current_program_path_stl.substr(0, pos +1);
	std::wstring server_config_ini = current_program_dir;
	server_config_ini.append(L"server_config.ini");
#else
	std::string current_program_path_stl = current_program_path;
	int pos = current_program_path_stl.find_last_of("\\");
	std::string current_program_dir = current_program_path_stl.substr(0, pos + 1);
	std::string server_config_ini = current_program_dir;
	server_config_ini.append("server_config.ini");
#endif
	

	// 读取配置文件，保存的连接信息
	int errCode = 0;
	TCHAR service_ip[4096] = {0};
	TCHAR service_gbcode[4096] = {0};
	TCHAR service_port[4096] = {0};
	TCHAR username[4096] = {0};
	TCHAR password[4096] = {0};

	DWORD ret = GetPrivateProfileString(_T("CONNECTION"), _T("SERVER_IP"), _T(""), service_ip, 4096, server_config_ini.c_str());
	errCode = GetLastError();
	ret = GetPrivateProfileString(_T("CONNECTION"), _T("SERVER_GBCODE"), _T(""), service_gbcode, 4096, server_config_ini.c_str());
	errCode = GetLastError();
	ret = GetPrivateProfileString(_T("CONNECTION"), _T("SERVER_PORT"), _T(""), service_port, 4096, server_config_ini.c_str());
	errCode = GetLastError();
	ret = GetPrivateProfileString(_T("CONNECTION"), _T("SERVER_USERNAME"), _T(""), username, 4096, server_config_ini.c_str());
	errCode = GetLastError();
	ret = GetPrivateProfileString(_T("CONNECTION"), _T("SERVER_PASSWORD"), _T(""), password, 4096, server_config_ini.c_str());
	errCode = GetLastError();

	m_cServerIp.SetWindowText(service_ip);
	m_cServerPort.SetWindowText(service_port);
	m_ccServerGbcode.SetWindowText(service_gbcode);
	m_cServerUsername.SetWindowText(username);
	m_cServerPassword.SetWindowText(password);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGB28181ServerPlatformDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGB28181ServerPlatformDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGB28181ServerPlatformDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGB28181ServerPlatformDlg::OnBnClickedBtnStart()
{
	CString str_server_ip;
	CString str_server_port;
	CString str_server_gbcode;
	CString str_server_username;
	CString str_server_password;

	m_cServerIp.GetWindowText(str_server_ip);
	m_cServerPort.GetWindowText(str_server_port);
	m_ccServerGbcode.GetWindowText(str_server_gbcode);
	m_cServerUsername.GetWindowText(str_server_username);
	m_cServerPassword.GetWindowText(str_server_password);

	// 调用接口启动
	USES_CONVERSION;
	int errCode = object_->Start(T2A(str_server_ip.GetBuffer(0)), _ttoi(str_server_port.GetBuffer(0)), T2A(str_server_gbcode.GetBuffer(0)), T2A(str_server_password.GetBuffer(0)));
	if (errCode != 0)
	{
		// 启动失败
		return ;
	}

	// 启动成功
}

void CGB28181ServerPlatformDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	object_->Stop();
}
