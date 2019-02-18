
// GxxGmGoVideoMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmGoVideoMgr.h"
#include "GxxGmGoVideoMgrDlg.h"

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


// CGxxGmGoVideoMgrDlg 对话框




CGxxGmGoVideoMgrDlg::CGxxGmGoVideoMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmGoVideoMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGxxGmGoVideoMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_cTabMain);
}

BEGIN_MESSAGE_MAP(CGxxGmGoVideoMgrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CGxxGmGoVideoMgrDlg::OnTcnSelchangeTabMain)
END_MESSAGE_MAP()


// CGxxGmGoVideoMgrDlg 消息处理程序

BOOL CGxxGmGoVideoMgrDlg::OnInitDialog()
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
	SetWindowText(_T("GoVideo5.0 设备管理工具 V1.1"));

	login_dlg_.govideo_ = &govideo_;
	service_state_dlg_.govideo_ = &govideo_;
	dev_mgr_dlg_.govideo_ = &govideo_;

	char current_program_path[4096] = {0};
	GetModuleFileNameA(NULL, current_program_path, 4096);
	std::string stl_current_program_path = current_program_path;
	int pos = stl_current_program_path.find_last_of("\\");
	std::string stl_current_dir = stl_current_program_path.substr(0, pos);
	std::string stl_ini_cfg_path = stl_current_dir + "\\GxxGmGoVideoMgrConfig.ini";

	// 加载配置管理器
	//ini_cfg_ = new Poco::Util::IniFileConfiguration();
	//ini_cfg_->load(stl_ini_cfg_path.c_str());

	login_dlg_.config_path_ = stl_ini_cfg_path;
	//login_dlg_.ini_cfg_ = ini_cfg_;
	INT_PTR ret = login_dlg_.DoModal();
	if (ret == IDCANCEL)
		exit(4);

	// 为Tab Control增加两个页面
	m_cTabMain.InsertItem(0, _T("服务状态"));
	m_cTabMain.InsertItem(1, _T("设备管理"));

	// 创建两个对话框
	service_state_dlg_.Create(IDD_CHILDDLG_SERVICESTATE, &m_cTabMain);
	dev_mgr_dlg_.Create(IDD_CHILDDLG_DEVMGR, &m_cTabMain);

	// 设定在Tab内显示的范围
	CRect rc;
	m_cTabMain.GetClientRect(rc);
	rc.top += 22;
	rc.bottom -= 2;
	rc.left += 2;
	rc.right -= 2;
	service_state_dlg_.MoveWindow(&rc);
	dev_mgr_dlg_.MoveWindow(&rc);

	// 显示初始页面
	service_state_dlg_.ShowWindow(SW_SHOW);
	dev_mgr_dlg_.ShowWindow(SW_HIDE);

	// 保存当前选择
	m_cTabMain.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGxxGmGoVideoMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGxxGmGoVideoMgrDlg::OnPaint()
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
HCURSOR CGxxGmGoVideoMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGxxGmGoVideoMgrDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 得到当前标签索引
	int current_tab_index = m_cTabMain.GetCurSel();
	switch (current_tab_index)
	{
	case 0:
		service_state_dlg_.ShowWindow(SW_SHOW);
		dev_mgr_dlg_.ShowWindow(SW_HIDE);
		break;
	case 1:
		service_state_dlg_.ShowWindow(SW_HIDE);
		dev_mgr_dlg_.ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}
