
// GxxGmPlayerDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmPlayerDemo.h"
#include "GxxGmPlayerDemoDlg.h"

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


// CGxxGmPlayerDemoDlg 对话框




CGxxGmPlayerDemoDlg::CGxxGmPlayerDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmPlayerDemoDlg::IDD, pParent)
	, play_handle_(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGxxGmPlayerDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_GOVIDEO_IP, m_cGoVideoIP);
	DDX_Control(pDX, IDC_EDIT_GOVIDEO_PORT, m_cGoVideoPort);
	DDX_Control(pDX, IDC_EDIT_URL, m_cVideoURL);
	//DDX_Control(pDX, IDC_LIST_ONLINE_DEV, m_cOnlineDevices);
	DDX_Control(pDX, IDC_LIST_STATE, m_cState);
	DDX_Control(pDX, IDC_LIST_ONLINE_DEV, m_cOnlineDevices);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_cVolume);
}

BEGIN_MESSAGE_MAP(CGxxGmPlayerDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CGxxGmPlayerDemoDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_GET_ONLINE_DEVICE, &CGxxGmPlayerDemoDlg::OnBnClickedBtnGetOnlineDevice)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ONLINE_DEV, &CGxxGmPlayerDemoDlg::OnNMDblclkListOnlineDev)
	ON_BN_CLICKED(IDC_BTN_OPEN_URL, &CGxxGmPlayerDemoDlg::OnBnClickedBtnOpenUrl)
	ON_BN_CLICKED(IDC_BTN_CLOSE_URL, &CGxxGmPlayerDemoDlg::OnBnClickedBtnCloseUrl)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CGxxGmPlayerDemoDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_OPEN_SOUND, &CGxxGmPlayerDemoDlg::OnBnClickedBtnOpenSound)
	ON_BN_CLICKED(IDC_BTN_CLOSE_SOUND, &CGxxGmPlayerDemoDlg::OnBnClickedBtnCloseSound)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOLUME, &CGxxGmPlayerDemoDlg::OnTRBNThumbPosChangingSliderVolume)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGxxGmPlayerDemoDlg 消息处理程序

BOOL CGxxGmPlayerDemoDlg::OnInitDialog()
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
	m_cOnlineDevices.SetExtendedStyle(m_cOnlineDevices.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cOnlineDevices.InsertColumn(0, _T("设备内部索引"), LVCFMT_LEFT, 100);
	m_cOnlineDevices.InsertColumn(1, _T("设备国标编码"), LVCFMT_LEFT, 180);

	m_cGoVideoIP.SetWindowText(_T("190.14.72.2"));
	m_cGoVideoPort.SetWindowText(_T("99"));

	TCHAR msg[4096] = {0};
	EnumGSPlayErrorCode err = GSPlay_Init();
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		_stprintf_s(msg, _T("初始化高新兴播放组件GSPlaySDK失败！错误码：%d"), err);
		m_cState.AddString(msg);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGxxGmPlayerDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGxxGmPlayerDemoDlg::OnPaint()
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
HCURSOR CGxxGmPlayerDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CGxxGmPlayerDemoDlg::_PFuncRawDataCB(StruGSPlayFrameData* pstMediaFrameData, void* pUser)
{
	// 原始数据回调，收到数据后就立即回调
	// 其实这里收到的就是PS流
	CGxxGmPlayerDemoDlg *dlg = (CGxxGmPlayerDemoDlg *)pUser;

	//SYSTEMTIME st;
	//GetLocalTime(&st);

	//// 媒体类型
	//std::string str_media_type;
	//switch (pstMediaFrameData->MediaDescInfo.eMediaType)
	//{
	//case GS_MEDIA_DATA_TYPE_VIDEOENCODE:
	//	// 编码视频流
	//	str_media_type = "编码视频流";
	//	break;
	//case GS_MEDIA_DATA_TYPE_VIDEODECODE:
	//	// 解码视频流
	//	str_media_type = "解码视频流";
	//	break;
	//case GS_MEDIA_DATA_TYPE_AUDIO:
	//	// 音频流
	//	str_media_type = "音频流";
	//	break;
	//case GS_MEDIA_DATA_TYPE_SEI:
	//	// 增强信息
	//	str_media_type = "增强信息";
	//	break;
	//case GS_MEDIA_DATA_TYPE_SYSHEADER:
	//	// 信息头
	//	str_media_type = "信息头";
	//	break;
	//case GS_MEDIA_DATA_TYPE_COMPLEX:
	//	// 复合数据
	//	str_media_type = "复合数据";
	//	break;
	//}

	//char dbgmsg[4096] = {0};
	//sprintf_s(dbgmsg, 4096, "[%d%02d%02d-%02d%02d%02d-%03d]收到媒体数据：媒体类型：%s，编码ID：%d\n"
	//	, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
	//	, str_media_type.c_str()
	//	, pstMediaFrameData->MediaDescInfo.eCodeID);

	//TRACE(dbgmsg);
}

void CGxxGmPlayerDemoDlg::_PFuncDisplayCB(StruGSPlayFrameData* pstMediaFrameData, void* pUser)
{
	// 渲染回调，数据是解码后的数据
	CGxxGmPlayerDemoDlg *dlg = (CGxxGmPlayerDemoDlg *)pUser;

	SYSTEMTIME st;
	GetLocalTime(&st);

	// 媒体类型
	std::string str_media_type;
	switch (pstMediaFrameData->MediaDescInfo.eMediaType)
	{
	case GS_MEDIA_DATA_TYPE_VIDEOENCODE:
		// 编码视频流
		str_media_type = "编码视频流";
		break;
	case GS_MEDIA_DATA_TYPE_VIDEODECODE:
		// 解码视频流
		str_media_type = "解码视频流";
		break;
	case GS_MEDIA_DATA_TYPE_AUDIO:
		// 音频流
		str_media_type = "音频流";
		break;
	case GS_MEDIA_DATA_TYPE_SEI:
		// 增强信息
		str_media_type = "增强信息";
		break;
	case GS_MEDIA_DATA_TYPE_SYSHEADER:
		// 信息头
		str_media_type = "信息头";
		break;
	case GS_MEDIA_DATA_TYPE_COMPLEX:
		// 复合数据
		str_media_type = "复合数据";
		break;
	}

	char dbgmsg[4096] = {0};
	sprintf_s(dbgmsg, 4096, "[%d%02d%02d-%02d%02d%02d-%03d]收到媒体数据：媒体类型：%s，编码ID：%d\n"
		, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
		, str_media_type.c_str()
		, pstMediaFrameData->MediaDescInfo.eCodeID);

	TRACE(dbgmsg);
}

void CGxxGmPlayerDemoDlg::_PFuncDrawCB(PDC pdc, void* pUser)
{
	CGxxGmPlayerDemoDlg *dlg = (CGxxGmPlayerDemoDlg *)pUser;
	TRACE("\n DrawCallback \n");
}

void CGxxGmPlayerDemoDlg::_PFuncMediaSatusCB(EnumMediaStatus eStatus, void* pStatusData, void* pUser)
{
	// 实时模式下不可用
	CGxxGmPlayerDemoDlg *dlg = (CGxxGmPlayerDemoDlg *)pUser;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CGxxGmPlayerDemoDlg::OnBnClickedBtnLogin()
{
	TCHAR msg[4096] = {0};
	CString str_govideo_ip;
	CString str_govideo_port;

	m_cGoVideoIP.GetWindowText(str_govideo_ip);
	m_cGoVideoPort.GetWindowText(str_govideo_port);

	USES_CONVERSION;
	int errCode = govideo.Login(T2A(str_govideo_ip.GetBuffer(0)), _ttoi(str_govideo_port.GetBuffer(0)));
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("登录到GoVideo[%s:%s]失败！错误码：%d"), str_govideo_ip.GetBuffer(0), str_govideo_port.GetBuffer(0), errCode);
		m_cState.AddString(msg);
	}
	else
	{
		_stprintf_s(msg, _T("登录到GoVideo[%s:%s]成功！错误码：%d"), str_govideo_ip.GetBuffer(0), str_govideo_port.GetBuffer(0), errCode);
		m_cState.AddString(msg);
	}
}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnGetOnlineDevice()
{
	TCHAR msg[4096] = {0};

	// 获取所有设备信息
	int errCode = govideo.GetAllDevices();
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("获取所有设备信息失败！错误码：%d"), errCode);
		m_cState.AddString(msg);
		return ;
	}

	// 获取所有设备状态信息
	errCode = govideo.GetAllDeviceStatus();
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("获取所有设备状态信息失败！错误码：%d"), errCode);
		m_cState.AddString(msg);
		return ;
	}

	// 先删除所有在线设备信息
	m_cOnlineDevices.DeleteAllItems();

	// 遍历所有设备在线状态信息
	std::vector<GOVIDEO_DEVICE_STATUS>::iterator iter;
	for (iter = govideo.devices_status_.begin(); iter != govideo.devices_status_.end(); ++iter)
	{
		GOVIDEO_DEVICE_STATUS *dev_status = &(*iter);
		if (dev_status->status_ == 0)
		{
			// 设备在线，将设备ID打印出来，可能使用CViewCtrl会比较好一点
			// 因为到时候点流实际使用的不是国标码，是GoVideo的内部编码
			int index = m_cOnlineDevices.GetItemCount();

			USES_CONVERSION;
			TCHAR internal_id[4096] = {0};
			_stprintf_s(internal_id, 4096, _T("%d"), dev_status->device_id_);
			m_cOnlineDevices.InsertItem(index, internal_id);
			m_cOnlineDevices.SetItemText(index, 1, A2T(dev_status->gb28181_code_.c_str()));
		}
	}
}

void CGxxGmPlayerDemoDlg::OnNMDblclkListOnlineDev(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	TCHAR msg[4096] = {0};
	
	// 找到当前选中的
	POSITION pos = m_cOnlineDevices.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return ;

	// 停止播放
	OnBnClickedBtnCloseUrl();

	TCHAR internal_index[4096] = {0};
	TCHAR device_gbcode[4096] = {0};
	int select_index = m_cOnlineDevices.GetNextSelectedItem(pos);
	m_cOnlineDevices.GetItemText(select_index, 0, internal_index, 4096);
	m_cOnlineDevices.GetItemText(select_index, 1, device_gbcode, 4096);

	int internal_id = _ttoi(internal_index);

	// 发起点流命令，获取URL
	std::string rtsp_url;
	int errCode = govideo.GetRealStream(internal_id, rtsp_url);
	if (errCode != 0)
	{
		_stprintf_s(msg, _T("[%s]点流失败！！错误码：%d"), device_gbcode, errCode);
		m_cState.AddString(msg);
		return ;
	}
	
	USES_CONVERSION;
	m_cVideoURL.SetWindowText(A2T(rtsp_url.c_str()));

	// 打开
	OnBnClickedBtnOpenUrl();

	// 播放
	OnBnClickedBtnPlay();

	*pResult = 0;
}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnOpenUrl()
{
	TCHAR msg[4096] = {0};
	
	StruDeviceCaps stDeviceCaps;
	EnumGSPlayErrorCode err = GSPlay_GetCapablity(&stDeviceCaps);
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		_stprintf_s(msg, _T("获取播放器能力失败！错误码：%d"), err);
		m_cState.AddString(msg);
		return ;
	}

	CWnd *pcwnd = GetDlgItem(IDC_STATIC_SCREEN);
	HWND hWnd = pcwnd->GetSafeHwnd();

	USES_CONVERSION;
	CString rtsp_url;
	m_cVideoURL.GetWindowText(rtsp_url);

	StruGSMediaOpenInfo stGSMediaOpenInfo;
	stGSMediaOpenInfo.eMode = EnumGSChannelMode::GSPlayMode_Real;
	stGSMediaOpenInfo.eChannelType = EnumGSChannelType::CT_Both;
	stGSMediaOpenInfo.hWnd = (Int64)hWnd;
	stGSMediaOpenInfo.pszUrl = T2A(rtsp_url.GetBuffer(0));
	stGSMediaOpenInfo.pszUser = NULL;
	stGSMediaOpenInfo.eDelayMode = EnumGSDelayMode::DM_Balance;
	stGSMediaOpenInfo.pPlayParam = this;

	err = GSPlay_Open(stGSMediaOpenInfo, &play_handle_);
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		_stprintf_s(msg, _T("打开视频流[%s]失败！错误码：%d"), rtsp_url.GetBuffer(0), err);
		m_cState.AddString(msg);
	}
	else
	{
		_stprintf_s(msg, _T("打开视频流[%s]成功！错误码：%d"), rtsp_url.GetBuffer(0), err);
		m_cState.AddString(msg);

		// 安装回调
		GSPlay_SetMediaStatusCallback(play_handle_, _PFuncMediaSatusCB, this);
		GSPlay_SetRawDataCallback(play_handle_, _PFuncRawDataCB, this);
		GSPlay_SetDisplayCallback(play_handle_, _PFuncDisplayCB, this);
		GSPlay_SetDrawCallback(play_handle_, _PFuncDrawCB, this);
	}
}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnCloseUrl()
{
	OnBnClickedBtnCloseSound();
	GSPlay_RefreshView(play_handle_);
	GSPlay_Close(play_handle_);

	//获取控件区域
	CRect rect;
	GetDlgItem(IDC_STATIC_SCREEN)->GetWindowRect(&rect);

	//屏幕区域对客户区域的转换
	ScreenToClient(&rect);

	//刷新
	InvalidateRect(rect);

}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnPlay()
{
	TCHAR msg[4096] = {0};
	
	EnumGSPlayErrorCode err = GSPlay_Play(play_handle_);
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		_stprintf_s(msg, _T("播放视频流失败！错误码：%d"), err);
		m_cState.AddString(msg);
	}
	else
	{
		_stprintf_s(msg, _T("播放视频流成功！错误码：%d"), err);
		m_cState.AddString(msg);
	}
}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnOpenSound()
{
	TCHAR msg[4096] = {0};

	EnumGSPlayErrorCode err = GSPlay_PlaySound(play_handle_);
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		//_stprintf_s(msg, _T("播放音频流失败！错误码：%d"), err);
		m_cState.AddString(msg);
	}
	else
	{
		//_stprintf_s(msg, _T("播放音频流成功！错误码：%d"), err);
		m_cState.AddString(msg);
	}
}

void CGxxGmPlayerDemoDlg::OnBnClickedBtnCloseSound()
{
	TCHAR msg[4096] = {0};

	EnumGSPlayErrorCode err = GSPlay_StopSound(play_handle_);
	if (err != EnumGSPlayErrorCode::GSPLAY_ERRCODE_SUCCESS)
	{
		_stprintf_s(msg, _T("停止播放音频流失败！错误码：%d"), err);
		//m_cState.AddString(msg);
	}
	else
	{
		_stprintf_s(msg, _T("停止播放音频流成功！错误码：%d"), err);
		//m_cState.AddString(msg);
	}
}

void CGxxGmPlayerDemoDlg::OnTRBNThumbPosChangingSliderVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	*pResult = 0;
}

void CGxxGmPlayerDemoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	GSPlay_UnInit();
	CDialog::OnClose();
}
