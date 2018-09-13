
// GxxGmCDBurnDemoExDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GxxGmCDBurnDemoEx.h"
#include "GxxGmCDBurnDemoExDlg.h"

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


// CGxxGmCDBurnDemoExDlg �Ի���




CGxxGmCDBurnDemoExDlg::CGxxGmCDBurnDemoExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmCDBurnDemoExDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGxxGmCDBurnDemoExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CDRECORDOCXCTRL1, m_cPlugin);
	DDX_Control(pDX, IDC_LIST_STATE, m_cState);
}

BEGIN_MESSAGE_MAP(CGxxGmCDBurnDemoExDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CGxxGmCDBurnDemoExDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_INIT, &CGxxGmCDBurnDemoExDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_INSTALL_CALLBACK, &CGxxGmCDBurnDemoExDlg::OnBnClickedBtnInstallCallback)
	ON_BN_CLICKED(IDC_BTN_FIND_DEVICE, &CGxxGmCDBurnDemoExDlg::OnBnClickedBtnFindDevice)
	ON_BN_CLICKED(IDC_BTN_BURN, &CGxxGmCDBurnDemoExDlg::OnBnClickedBtnBurn)
	ON_BN_CLICKED(IDC_BTN_CHECK_CD, &CGxxGmCDBurnDemoExDlg::OnBnClickedBtnCheckCd)
END_MESSAGE_MAP()


// CGxxGmCDBurnDemoExDlg ��Ϣ��������

BOOL CGxxGmCDBurnDemoExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

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

	// TODO: �ڴ����Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGxxGmCDBurnDemoExDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGxxGmCDBurnDemoExDlg::OnPaint()
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
HCURSOR CGxxGmCDBurnDemoExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGxxGmCDBurnDemoExDlg::OnBnClickedButton1()
{
	
}

void CGxxGmCDBurnDemoExDlg::OnBnClickedBtnInit()
{
	const TCHAR *command = _T("{\"action\":\"init\"}");
	CString ret = m_cPlugin.BurnManager(command);
	OutputDebugString(ret.GetString());
	m_cState.AddString(ret);
}

void CGxxGmCDBurnDemoExDlg::OnBnClickedBtnInstallCallback()
{
	IDispatch *idspatch_fun = NULL;
	long lret = m_cPlugin.JSCallBack(idspatch_fun);
}

void CGxxGmCDBurnDemoExDlg::OnBnClickedBtnFindDevice()
{
	const TCHAR *command = _T("{\"action\":\"BurnGetAvailableDisks\"}");
	CString ret = m_cPlugin.BurnManager(command);
	OutputDebugString(ret.GetString());
	m_cState.AddString(ret);
}

void CGxxGmCDBurnDemoExDlg::OnBnClickedBtnBurn()
{
	return ;
	const TCHAR *command = _T("{\"action\": \"BurnFile\", \"nConnectTimeout\":3, \"nDownloadTimeout\":10, \"param\": [{\"title\": \"Test1\", \"downloadLink\":[\"http://10.10.16.59/live/15m.mp4;name=15m.mp4\"] }]}");
	CString ret = m_cPlugin.BurnManager(command);
	OutputDebugString(ret.GetString());
	m_cState.AddString(ret);
}

void CGxxGmCDBurnDemoExDlg::OnBnClickedBtnCheckCd()
{
	const TCHAR *command = _T("{\"action\":\"BurnEraseCD\"}");
	CString ret = m_cPlugin.BurnManager(command);
	OutputDebugString(ret.GetString());
	m_cState.AddString(ret);
}