
// GxxGmChromeWndFollowDlg.cpp : ʵ���ļ�
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
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CGxxGmChromeWndFollowDlg ��Ϣ�������



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
	int errCode = 0;

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
		page_title = json_object->get("page_title").toString();
		Poco::Int32 pos_left = json_object->get("player_position_left");
		Poco::Int32 pos_top = json_object->get("player_position_top");
		Poco::Int32 pos_right = json_object->get("player_position_right");
		Poco::Int32 pos_bottom = json_object->get("player_position_bottom");

		x = pos_left;
		y = pos_top;
		width = pos_right - pos_left;
		height = pos_bottom - pos_top;

		// ���Ҹ����ڣ��õ�������Ȼ�������Ӵ���
		this->MoveWindow(x, y, width, height);
	}
	catch (Poco::JSON::JSONException &ex)
	{
		ex.code();
		ex.name();
	}

	// ��װȫ����Ϣ���ӣ�Ŀǰ���񹳲����������ڵ��ƶ���Ϣ
	//errCode = InstallHooks(this->m_hWnd);

	// �������ڸ����̣߳����ڸ���ָ�������������
	CreateThread(NULL, 0, WndFollowThread, this, 0, NULL);

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


BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	CGxxGmChromeWndFollowDlg *dlg = (CGxxGmChromeWndFollowDlg *)lParam;

	RECT window_rect;
	GetWindowRect(hwnd, &window_rect);

	// chrome �Ӵ��ڱ��⣺Chrome Legacy Window
	// ģ��Ŀؼ�������Ӵ�������Ϊ(8,8) (208, 208)

	// ����һ�����ڣ��ŵ��߳�����ִ��
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
		// ö���Ӵ���
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

	// ���Ȼ�ȡ���ڵĳ���
	RECT rect;
	this->GetClientRect(&rect);

	TRACE("left:%d, top:%d, right:%d, bottom:%d\n", rect.left, rect.top, rect.right, rect.bottom);

	int nx = rect.left;
	int ny = rect.top;
	int nwidth = rect.right - rect.left;
	int nheight = rect.bottom - rect.top;

	// ���������Ըı�ؼ��ĳ���
	CWnd *pcwnd = GetDlgItem(IDC_STATIC_SCREEN);
	pcwnd->MoveWindow(nx, ny, nwidth, nheight);
}
