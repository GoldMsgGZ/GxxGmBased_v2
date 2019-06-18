
// GxxGmChromeWndFollowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmChromeWndFollow.h"
#include "GxxGmChromeWndFollowDlg.h"

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"

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


// CGxxGmChromeWndFollowDlg 对话框




CGxxGmChromeWndFollowDlg::CGxxGmChromeWndFollowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmChromeWndFollowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGxxGmChromeWndFollowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGxxGmChromeWndFollowDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CGxxGmChromeWndFollowDlg 消息处理程序

HHOOK hook_callwndproc		= NULL;
HHOOK hook_callwndprocret	= NULL;
HHOOK hook_msgfilter		= NULL;
HHOOK hook_shell			= NULL;
HHOOK hook_sysmsgfilter		= NULL;

BOOL CGxxGmChromeWndFollowDlg::OnInitDialog()
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
	// 首先获得要跟随的窗口标题，以及指定空间范围内的控件POS
	AfxGetApp()->m_lpCmdLine;
	//MessageBox(AfxGetApp()->m_lpCmdLine, _T(""), 0);

	// 首先要将命令行转换为UTF8
	std::string param;
	Poco::UnicodeConverter::toUTF8(AfxGetApp()->m_lpCmdLine, param);

	// 解析Json
	try
	{
		Poco::JSON::Parser parse;
		Poco::Dynamic::Var json_var = parse.parse(param);
		assert(json_var.type() == typeid(Poco::JSON::Object::Ptr));

		Poco::JSON::Object::Ptr json_object = json_var.extract<Poco::JSON::Object::Ptr>();
		std::string page_title = json_object->get("page_title").toString();
		Poco::Int32 pos_left = json_object->get("player_position_left");
		Poco::Int32 pos_top = json_object->get("player_position_top");
		Poco::Int32 pos_right = json_object->get("player_position_right");
		Poco::Int32 pos_bottom = json_object->get("player_position_bottom");

		int x = pos_left;
		int y = pos_top;
		int width = pos_right - pos_left;
		int height = pos_bottom - pos_top;

		// 查找父窗口，得到其句柄，然后搜索子窗口
		this->MoveWindow(x, y, width, height);
	}
	catch (Poco::JSON::JSONException &ex)
	{
		ex.code();
		ex.name();
	}

	// 这里注册一个全局消息钩子
	hook_callwndproc	= SetWindowsHookEx(WH_CALLWNDPROC,		CallWndProc,	theApp.m_hInstance, NULL);
	int errCode = GetLastError();

	hook_callwndprocret	= SetWindowsHookEx(WH_CALLWNDPROCRET,	CallWndRetProc, theApp.m_hInstance, NULL);
	errCode = GetLastError();

	hook_msgfilter		= SetWindowsHookEx(WH_MSGFILTER,		MessageProc,	theApp.m_hInstance, NULL);
	errCode = GetLastError();

	hook_shell			= SetWindowsHookEx(WH_SHELL,			ShellProc,		theApp.m_hInstance, NULL);
	errCode = GetLastError();

	hook_sysmsgfilter	= SetWindowsHookEx(WH_SYSMSGFILTER,		SysMsgProc,		theApp.m_hInstance, NULL);
	errCode = GetLastError();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGxxGmChromeWndFollowDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGxxGmChromeWndFollowDlg::OnPaint()
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
HCURSOR CGxxGmChromeWndFollowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT msg = (PCWPSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("窗口移动消息\n");
	}

	// 向下传递消息
	return CallNextHookEx(hook_callwndproc, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPRETSTRUCT msg = (PCWPRETSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("窗口移动消息\n");
	}

	// 向下传递消息
	return CallNextHookEx(hook_callwndprocret, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("窗口移动消息\n");
	}

	// 向下传递消息
	return CallNextHookEx(hook_msgfilter, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("窗口移动消息\n");
	}

	// 向下传递消息
	return CallNextHookEx(hook_shell, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("窗口移动消息\n");
	}

	switch (nCode)
	{
	case MSGF_DIALOGBOX:
		// 窗口消息
		break;
	case MSGF_MENU:
		break;
	case MSGF_SCROLLBAR:
		// 滚动条滚动消息
		break;
	}

	// 向下传递消息
	return CallNextHookEx(hook_sysmsgfilter, nCode, wParam, lParam);
}

