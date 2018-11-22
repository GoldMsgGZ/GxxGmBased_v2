
// GxxGmGoVideoPlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GxxGmGoVideoPlayer.h"
#include "GxxGmGoVideoPlayerDlg.h"

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


// CGxxGmGoVideoPlayerDlg �Ի���




CGxxGmGoVideoPlayerDlg::CGxxGmGoVideoPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmGoVideoPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGxxGmGoVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMS_IP, m_cCmsIp);
	DDX_Control(pDX, IDC_EDIT_CMS_PORT, m_cCmsPort);
	DDX_Control(pDX, IDC_LIST_ONLINE_DEVICES, m_cOnlineDevicesList);
	DDX_Control(pDX, IDC_LIST_STATE, m_cState);
	DDX_Control(pDX, IDC_EDIT_STREAM_URL, m_cVideoURL);
}

BEGIN_MESSAGE_MAP(CGxxGmGoVideoPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LOGIN_CMS, &CGxxGmGoVideoPlayerDlg::OnBnClickedBtnLoginCms)
	ON_BN_CLICKED(IDC_BTN_UPDATE_ONLINE_DEVICES, &CGxxGmGoVideoPlayerDlg::OnBnClickedBtnUpdateOnlineDevices)
	ON_BN_CLICKED(IDC_BTN_STOP, &CGxxGmGoVideoPlayerDlg::OnBnClickedBtnStop)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ONLINE_DEVICES, &CGxxGmGoVideoPlayerDlg::OnNMDblclkListOnlineDevices)
END_MESSAGE_MAP()


// CGxxGmGoVideoPlayerDlg ��Ϣ��������

BOOL CGxxGmGoVideoPlayerDlg::OnInitDialog()
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
	this->SetWindowText(_T("�����˹��� GoVideo������ For RTMP�� V1.0"));

	m_cOnlineDevicesList.SetExtendedStyle(m_cOnlineDevicesList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cOnlineDevicesList.InsertColumn(0, _T("�豸�ڲ�����"), LVCFMT_LEFT, 100);
	m_cOnlineDevicesList.InsertColumn(1, _T("�豸�������"), LVCFMT_LEFT, 180);

	m_cCmsIp.SetWindowText(_T("10.10.9.18"));
	m_cCmsPort.SetWindowText(_T("99"));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGxxGmGoVideoPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGxxGmGoVideoPlayerDlg::OnPaint()
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
HCURSOR CGxxGmGoVideoPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGxxGmGoVideoPlayerDlg::OnBnClickedBtnLoginCms()
{
	// ��¼��GoVideo
	TCHAR msg[4096] = {0};
	CString str_cms_ip;
	CString str_cms_port;

	m_cCmsIp.GetWindowText(str_cms_ip);
	m_cCmsPort.GetWindowText(str_cms_port);

	USES_CONVERSION;
	int errCode = govideo.Login(T2A(str_cms_ip.GetBuffer(0)), _ttoi(str_cms_port.GetBuffer(0)));
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("��¼��GoVideo-Cms[%s:%s]ʧ�ܣ������룺%d"), str_cms_ip.GetBuffer(0), str_cms_port.GetBuffer(0), errCode);
		m_cState.AddString(msg);
	}
	else
	{
		_stprintf_s(msg, _T("��¼��GoVideo-Cms[%s:%s]�ɹ��������룺%d"), str_cms_ip.GetBuffer(0), str_cms_port.GetBuffer(0), errCode);
		m_cState.AddString(msg);
	}
}

void CGxxGmGoVideoPlayerDlg::OnBnClickedBtnUpdateOnlineDevices()
{
	// ���������豸�б�
	TCHAR msg[4096] = {0};

	// ��ȡ�����豸��Ϣ
	int errCode = govideo.GetAllDevices();
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("��ȡ�����豸��Ϣʧ�ܣ������룺%d"), errCode);
		m_cState.AddString(msg);
		return ;
	}

	// ��ȡ�����豸״̬��Ϣ
	errCode = govideo.GetAllDeviceStatus();
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("��ȡ�����豸״̬��Ϣʧ�ܣ������룺%d"), errCode);
		m_cState.AddString(msg);
		return ;
	}

	// ��ɾ�����������豸��Ϣ
	m_cOnlineDevicesList.DeleteAllItems();

	// ���������豸����״̬��Ϣ
	std::vector<GOVIDEO_DEVICE_STATUS>::iterator iter;
	for (iter = govideo.devices_status_.begin(); iter != govideo.devices_status_.end(); ++iter)
	{
		GOVIDEO_DEVICE_STATUS *dev_status = &(*iter);
		if (dev_status->status_ == 0)
		{
			// �豸���ߣ����豸ID��ӡ����������ʹ��CViewCtrl��ȽϺ�һ��
			// ��Ϊ��ʱ�����ʵ��ʹ�õĲ��ǹ����룬��GoVideo���ڲ�����
			int index = m_cOnlineDevicesList.GetItemCount();

			USES_CONVERSION;
			TCHAR internal_id[4096] = {0};
			_stprintf_s(internal_id, 4096, _T("%d"), dev_status->device_id_);
			m_cOnlineDevicesList.InsertItem(index, internal_id);
			m_cOnlineDevicesList.SetItemText(index, 1, A2T(dev_status->gb28181_code_.c_str()));
		}
	}
}

void CGxxGmGoVideoPlayerDlg::OnBnClickedBtnStop()
{
	// ֹͣ����
}

void CGxxGmGoVideoPlayerDlg::OnNMDblclkListOnlineDevices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	TCHAR msg[4096] = {0};

	// �ҵ���ǰѡ�е�
	POSITION pos = m_cOnlineDevicesList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return ;

	TCHAR internal_index[4096] = {0};
	TCHAR device_gbcode[4096] = {0};
	int select_index = m_cOnlineDevicesList.GetNextSelectedItem(pos);
	m_cOnlineDevicesList.GetItemText(select_index, 0, internal_index, 4096);
	m_cOnlineDevicesList.GetItemText(select_index, 1, device_gbcode, 4096);

	int internal_id = _ttoi(internal_index);

	// ������������ȡURL
	std::string rtsp_url;
	int errCode = govideo.GetRealStream(internal_id, rtsp_url);
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("[%s]����ʧ�ܣ��������룺%d"), device_gbcode, errCode);
		m_cState.AddString(msg);
		return ;
	}

	USES_CONVERSION;
	m_cVideoURL.SetWindowText(A2T(rtsp_url.c_str()));

	*pResult = 0;
}