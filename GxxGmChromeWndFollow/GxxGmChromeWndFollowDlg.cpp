
// GxxGmChromeWndFollowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmChromeWndFollow.h"
#include "GxxGmChromeWndFollowDlg.h"

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "../GxxGmMsgFilter/GxxGmMsgFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "GxxGmMsgFilter.lib")


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
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CGxxGmChromeWndFollowDlg 消息处理程序



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
	int errCode = 0;

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
		page_title = json_object->get("page_title").toString();
		Poco::Int32 pos_left = json_object->get("player_position_left");
		Poco::Int32 pos_top = json_object->get("player_position_top");
		Poco::Int32 pos_right = json_object->get("player_position_right");
		Poco::Int32 pos_bottom = json_object->get("player_position_bottom");

		x = pos_left;
		y = pos_top;
		width = pos_right - pos_left;
		height = pos_bottom - pos_top;

		// 查找父窗口，得到其句柄，然后搜索子窗口
		this->MoveWindow(x, y, width, height);
	}
	catch (Poco::JSON::JSONException &ex)
	{
		ex.code();
		ex.name();
	}

	// 安装全局消息钩子，目前好像钩不到其他窗口的移动消息
	//errCode = InstallHooks(this->m_hWnd);

	// 创建窗口跟随线程，用于跟随指定的浏览器窗口
	CreateThread(NULL, 0, WndFollowThread, this, 0, NULL);

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


BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	CGxxGmChromeWndFollowDlg *dlg = (CGxxGmChromeWndFollowDlg *)lParam;

	RECT window_rect;
	GetWindowRect(hwnd, &window_rect);

	// chrome 子窗口标题：Chrome Legacy Window
	// 模拟的控件相对于子窗口坐标为(8,8) (208, 208)

	// 创建一个窗口，放到线程里面执行
	int x = window_rect.left + dlg->x;
	int y = window_rect.top + dlg->y;
	int width = dlg->width;
	int height = dlg->height;

	dlg->MoveWindow(x, y, width, height);
	
	SetWindowPos(dlg->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	SetWindowPos(dlg->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	//dlg->SetX

	return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	CGxxGmChromeWndFollowDlg *dlg = (CGxxGmChromeWndFollowDlg *)lParam;

	char caption[200] = {0};
	memset(caption, 0, sizeof(caption));
	::GetWindowTextA(hwnd, caption, 200);

	if(strcmp(caption, ""))
		std::cout<<caption<<std::endl;

	// Google Chrome
	if(stricmp(caption, "GxxGmChromePlayer - Google Chrome") == 0)
	{
		// 枚举子窗口
		::EnumChildWindows(hwnd, EnumChildProc, (LPARAM)dlg);
	}

	return TRUE;

}

DWORD WINAPI CGxxGmChromeWndFollowDlg::WndFollowThread(LPVOID lpParam)
{
	CGxxGmChromeWndFollowDlg *dlg = (CGxxGmChromeWndFollowDlg *)lpParam;

	while (true)
	{
		EnumWindows(EnumWindowsProc, (LPARAM)dlg);
		//Sleep(1);
	}
	return 0;
}


void CGxxGmChromeWndFollowDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// 首先获取窗口的长宽
	RECT rect;
	this->GetClientRect(&rect);

	TRACE("left:%d, top:%d, right:%d, bottom:%d\n", rect.left, rect.top, rect.right, rect.bottom);

	int nx = rect.left;
	int ny = rect.top;
	int nwidth = rect.right - rect.left;
	int nheight = rect.bottom - rect.top;

	// 接下来尝试改变控件的长宽
	CWnd *pcwnd = GetDlgItem(IDC_STATIC_SCREEN);
	pcwnd->MoveWindow(nx, ny, nwidth, nheight);
}
