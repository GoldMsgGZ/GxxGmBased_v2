// GxxGmServiceStateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmGoVideoMgr.h"
#include "GxxGmServiceStateDlg.h"


// CGxxGmServiceStateDlg 对话框

IMPLEMENT_DYNAMIC(CGxxGmServiceStateDlg, CDialog)

CGxxGmServiceStateDlg::CGxxGmServiceStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmServiceStateDlg::IDD, pParent)
{

}

CGxxGmServiceStateDlg::~CGxxGmServiceStateDlg()
{
}

void CGxxGmServiceStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVICES, m_cRunningServiceList);
}


BEGIN_MESSAGE_MAP(CGxxGmServiceStateDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ONLINESERVICE, &CGxxGmServiceStateDlg::OnBnClickedBtnRefreshOnlineservice)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SERVICES, &CGxxGmServiceStateDlg::OnLvnKeydownListServices)
END_MESSAGE_MAP()


// CGxxGmServiceStateDlg 消息处理程序
BOOL CGxxGmServiceStateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cRunningServiceList.SetExtendedStyle(m_cRunningServiceList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cRunningServiceList.InsertColumn(0, _T("服务ID"), LVCFMT_LEFT, 80);
	m_cRunningServiceList.InsertColumn(1, _T("服务名称"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(2, _T("服务类型"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(3, _T("服务说明"), LVCFMT_LEFT, 80);
	m_cRunningServiceList.InsertColumn(4, _T("IP地址"), LVCFMT_LEFT, 100);
	m_cRunningServiceList.InsertColumn(5, _T("端口号"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(6, _T("用户名"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(7, _T("密码"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(8, _T("域名解析"), LVCFMT_LEFT, 60);
	m_cRunningServiceList.InsertColumn(9, _T("国标编码"), LVCFMT_LEFT, 100);
	m_cRunningServiceList.InsertColumn(10, _T("版本号"), LVCFMT_LEFT, 80);
	m_cRunningServiceList.InsertColumn(11, _T("许可信息"), LVCFMT_LEFT, 80);

	OnBnClickedBtnRefreshOnlineservice();

	return TRUE;
}

void CGxxGmServiceStateDlg::OnBnClickedBtnRefreshOnlineservice()
{
	USES_CONVERSION;
	int errCode = govideo_->GetDeviceGatewayList();

	m_cRunningServiceList.DeleteAllItems();

	std::vector<GOVIDEO_SERVICE_INFO>::iterator iter;
	for (iter = govideo_->govideo_services_.begin(); iter != govideo_->govideo_services_.end(); ++iter)
	{
		int count = m_cRunningServiceList.GetItemCount();
		m_cRunningServiceList.InsertItem(count, A2T(iter->service_id_.c_str()));
		m_cRunningServiceList.SetItemText(count, 1, A2T(iter->service_name_.c_str()));
		m_cRunningServiceList.SetItemText(count, 2, A2T(iter->service_type_id_.c_str()));
		m_cRunningServiceList.SetItemText(count, 3, A2T(iter->service_descript_.c_str()));
		m_cRunningServiceList.SetItemText(count, 4, A2T(iter->service_ip_.c_str()));
		m_cRunningServiceList.SetItemText(count, 5, A2T(iter->service_port_.c_str()));
		m_cRunningServiceList.SetItemText(count, 6, A2T(iter->username_.c_str()));
		m_cRunningServiceList.SetItemText(count, 7, A2T(iter->password_.c_str()));
		m_cRunningServiceList.SetItemText(count, 8, A2T(iter->is_dns_.c_str()));
		m_cRunningServiceList.SetItemText(count, 9, A2T(iter->gbcode_.c_str()));
		m_cRunningServiceList.SetItemText(count, 10, A2T(iter->version_.c_str()));
		m_cRunningServiceList.SetItemText(count, 11, A2T(iter->license_info_.c_str()));
	}
}

void CGxxGmServiceStateDlg::OnLvnKeydownListServices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	// 找到删除键
	if (pLVKeyDow->wVKey == VK_DELETE)
	{
		INT_PTR ret = MessageBox(_T("确定要删除选中的服务吗？"), _T("警告"), MB_OKCANCEL|MB_ICONMASK);
		if (ret != IDOK)
			return ;

		// 找到当前选中的行
		POSITION pos = m_cRunningServiceList.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return ;

		do 
		{
			int index = m_cRunningServiceList.GetNextSelectedItem(pos);
			if (index != -1)
			{
				// 先调用GoVideo的删除服务接口，成功后再调用界面移除记录		
				m_cRunningServiceList.DeleteItem(index);
			}
			else
				break;

		} while (true);
	}

	*pResult = 0;
}
