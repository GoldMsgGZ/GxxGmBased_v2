
// GxxGmChromeWndFollowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GxxGmChromeWndFollow.h"
#include "GxxGmChromeWndFollowDlg.h"

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CGxxGmChromeWndFollowDlg �Ի���




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


// CGxxGmChromeWndFollowDlg ��Ϣ�������

HHOOK hook_callwndproc		= NULL;
HHOOK hook_callwndprocret	= NULL;
HHOOK hook_msgfilter		= NULL;
HHOOK hook_shell			= NULL;
HHOOK hook_sysmsgfilter		= NULL;

BOOL CGxxGmChromeWndFollowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// ���Ȼ��Ҫ����Ĵ��ڱ��⣬�Լ�ָ���ռ䷶Χ�ڵĿؼ�POS
	AfxGetApp()->m_lpCmdLine;
	//MessageBox(AfxGetApp()->m_lpCmdLine, _T(""), 0);

	// ����Ҫ��������ת��ΪUTF8
	std::string param;
	Poco::UnicodeConverter::toUTF8(AfxGetApp()->m_lpCmdLine, param);

	// ����Json
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

		// ���Ҹ����ڣ��õ�������Ȼ�������Ӵ���
		this->MoveWindow(x, y, width, height);
	}
	catch (Poco::JSON::JSONException &ex)
	{
		ex.code();
		ex.name();
	}

	// ����ע��һ��ȫ����Ϣ����
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGxxGmChromeWndFollowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGxxGmChromeWndFollowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT msg = (PCWPSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("�����ƶ���Ϣ\n");
	}

	// ���´�����Ϣ
	return CallNextHookEx(hook_callwndproc, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPRETSTRUCT msg = (PCWPRETSTRUCT)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("�����ƶ���Ϣ\n");
	}

	// ���´�����Ϣ
	return CallNextHookEx(hook_callwndprocret, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("�����ƶ���Ϣ\n");
	}

	// ���´�����Ϣ
	return CallNextHookEx(hook_msgfilter, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("�����ƶ���Ϣ\n");
	}

	// ���´�����Ϣ
	return CallNextHookEx(hook_shell, nCode, wParam, lParam);
}

LRESULT CALLBACK CGxxGmChromeWndFollowDlg::SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG msg = (PMSG)lParam;
	if (msg->message == WM_MOVE)
	{
		TRACE("�����ƶ���Ϣ\n");
	}

	switch (nCode)
	{
	case MSGF_DIALOGBOX:
		// ������Ϣ
		break;
	case MSGF_MENU:
		break;
	case MSGF_SCROLLBAR:
		// ������������Ϣ
		break;
	}

	// ���´�����Ϣ
	return CallNextHookEx(hook_sysmsgfilter, nCode, wParam, lParam);
}

