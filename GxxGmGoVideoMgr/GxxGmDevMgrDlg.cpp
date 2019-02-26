// GxxGmDevMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GxxGmGoVideoMgr.h"
#include "GxxGmDevMgrDlg.h"

#include "bgMfcExcelModule.h"


// CGxxGmDevMgrDlg 对话框

IMPLEMENT_DYNAMIC(CGxxGmDevMgrDlg, CDialog)

CGxxGmDevMgrDlg::CGxxGmDevMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGxxGmDevMgrDlg::IDD, pParent)
	, current_select_dgw_id_(-1)
{

}

CGxxGmDevMgrDlg::~CGxxGmDevMgrDlg()
{
}

void CGxxGmDevMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DGWS, m_cDGWList);
	DDX_Control(pDX, IDC_LIST_DEVS, m_cDevList);
	DDX_Control(pDX, IDC_EDIT_DEVID, m_cDevID);
	DDX_Control(pDX, IDC_EDIT_DEVNAME, m_cDevName);
	DDX_Control(pDX, IDC_COMBO_MODELID, m_cModelID);
	DDX_Control(pDX, IDC_COMBO_CATEGORYID, m_cCategoryID);
	DDX_Control(pDX, IDC_EDIT_CONNECTION_INFO, m_cConnInfo);
	DDX_Control(pDX, IDC_EDIT_CFG_VERSION, m_cCfgVersion);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_cUsername);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_cPassword);
	DDX_Control(pDX, IDC_EDIT_DEVICE_CODE, m_cDevCode);
	DDX_Control(pDX, IDC_EDIT_EXT_INFO, m_cExtInfo);
	DDX_Control(pDX, IDC_EDIT_DEV_GBCODE, m_cDevGBCdeo);
	DDX_Control(pDX, IDC_EDIT_DEV_NAME_ABBR, m_cNameAbbr);
	DDX_Control(pDX, IDC_EDIT_DEV_VERSION, m_cDevVersion);
	DDX_Control(pDX, IDC_COMBO_DGW_ID, m_cDGWs);
}


BEGIN_MESSAGE_MAP(CGxxGmDevMgrDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_REFRESH_SERVICES, &CGxxGmDevMgrDlg::OnBnClickedBtnRefreshServices)
	ON_BN_CLICKED(IDC_BTN_REFRESH_DEVICES, &CGxxGmDevMgrDlg::OnBnClickedBtnRefreshDevices)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DGWS, &CGxxGmDevMgrDlg::OnNMClickListDgws)
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CGxxGmDevMgrDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_MODIFY, &CGxxGmDevMgrDlg::OnBnClickedBtnModify)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DEVS, &CGxxGmDevMgrDlg::OnLvnKeydownListDevs)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEVS, &CGxxGmDevMgrDlg::OnNMClickListDevs)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CGxxGmDevMgrDlg::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_EXPORT_DEV_LIST, &CGxxGmDevMgrDlg::OnBnClickedBtnExportDevList)
	ON_BN_CLICKED(IDC_BTN_IMPORT_DEV_LIST, &CGxxGmDevMgrDlg::OnBnClickedBtnImportDevList)
END_MESSAGE_MAP()


// CGxxGmDevMgrDlg 消息处理程序
BOOL CGxxGmDevMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cDGWList.SetExtendedStyle(m_cDGWList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cDGWList.InsertColumn(0, _T("服务ID"), LVCFMT_LEFT, 60);
	m_cDGWList.InsertColumn(1, _T("服务名称"), LVCFMT_LEFT, 80);
	m_cDGWList.InsertColumn(2, _T("服务地址"), LVCFMT_LEFT, 140);

	m_cDevList.SetExtendedStyle(m_cDevList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_cDevList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 80);
	m_cDevList.InsertColumn(1, _T("设备内部ID"), LVCFMT_LEFT, 100);
	m_cDevList.InsertColumn(2, _T("设备名称"), LVCFMT_LEFT, 120);
	m_cDevList.InsertColumn(3, _T("设备国标ID"), LVCFMT_LEFT, 160);
	m_cDevList.InsertColumn(4, _T("设备厂商"), LVCFMT_LEFT, 80);
	//m_cDevList.InsertColumn(4, _T("设备版本"), LVCFMT_LEFT, 80);

	// 厂商类型
	//m_cModelID.AddString(_T("30001"));	// HIK-DEV
	//m_cModelID.AddString(_T("70001"));	// ZBen-IPC
	//m_cModelID.AddString(_T("100001"));	// DAH
	//m_cModelID.AddString(_T("150001"));	// GXXIPC
	//m_cModelID.AddString(_T("160001"));	// HIK_CVR
	//m_cModelID.AddString(_T("170001"));	// JG-IPC
	m_cModelID.AddString(_T("180001"));	// GXXLTE
	//m_cModelID.AddString(_T("190001"));	// PACOM-IPC
	//m_cModelID.AddString(_T("210001"));	// XIHUA
	//m_cModelID.AddString(_T("220001"));	// UNIVIEW_NVR
	//m_cModelID.AddString(_T("230001"));	// UNIVIEW-IPC
	//m_cModelID.AddString(_T("240001"));	// ROCOSEE-NVR
	//m_cModelID.AddString(_T("250001"));	// TM-NVR
	//m_cModelID.AddString(_T("260001"));	// QiYang-DVR
	//m_cModelID.AddString(_T("270001"));	// JingYang-IPC
	//m_cModelID.AddString(_T("280001"));	// HuaWei-IPC
	//m_cModelID.AddString(_T("290001"));	// Tiandy-IPC
	//m_cModelID.AddString(_T("300001"));	// GXX-APP
	//m_cModelID.AddString(_T("310001"));	// YX-DVR
	//m_cModelID.AddString(_T("320001"));	// QH-DVR
	//m_cModelID.AddString(_T("330001"));	// UV-DEV-2017
	//m_cModelID.AddString(_T("340001"));	// HONEYWELL-NVR
	//m_cModelID.AddString(_T("350001"));	// HIKEHOME
	//m_cModelID.AddString(_T("360001"));	// VSK_DEV
	//m_cModelID.AddString(_T("370001"));	// TDWY_PASSIVE
	m_cModelID.AddString(_T("1000001"));	// GB28181
	//m_cModelID.AddString(_T("1010001"));	// ONVIF
	m_cModelID.SetCurSel(1);

	// 设备门类
	m_cCategoryID.AddString(_T("1"));
	m_cCategoryID.SetCurSel(0);

	// 默认选项
	//m_cConnInfo.SetWindowText(_T("0.0.0.0"));
	m_cCfgVersion.SetWindowText(_T("2019"));
	m_cUsername.SetWindowText(_T("admin"));
	m_cPassword.SetWindowText(_T("admin"));
	m_cExtInfo.SetWindowText(_T("null"));
	m_cNameAbbr.SetWindowText(_T("null"));
	m_cDevVersion.SetWindowText(_T("0"));
	m_cConnInfo.SetWindowText(_T("[LinkMode:1][Ip:0.0.0.0][Port:8000][Mac:][CodeID:]"));

	OnBnClickedBtnRefreshServices();

	return TRUE;
}

void CGxxGmDevMgrDlg::OnBnClickedBtnRefreshServices()
{
	USES_CONVERSION;
	int errCode = govideo_->GetDeviceGatewayList();

	m_cDGWList.DeleteAllItems();

	// 同时更新设备列表
	errCode = govideo_->GetAllDevices();

	std::vector<GOVIDEO_SERVICE_INFO>::iterator iter;
	for (iter = govideo_->govideo_services_.begin(); iter != govideo_->govideo_services_.end(); ++iter)
	{
		if (iter->service_type_id_.compare("102") == 0)
		{
			int count = m_cDGWList.GetItemCount();
			m_cDGWList.InsertItem(count, A2T(iter->service_id_.c_str()));
			m_cDGWList.SetItemText(count, 1, A2T(iter->service_name_.c_str()));

			char ipaddress[4096] = {0};
			sprintf_s(ipaddress, 4096, "%s:%s", iter->service_ip_.c_str(), iter->service_port_.c_str());
			m_cDGWList.SetItemText(count, 2, A2T(ipaddress));

			m_cDGWs.AddString(A2T(iter->service_id_.c_str()));
		}
		
	}
}

void CGxxGmDevMgrDlg::OnBnClickedBtnRefreshDevices()
{
	// 这里会显示所有注册的设备
	// 先获取所有设备列表
	int errCode = govideo_->GetAllDevices();
	if (errCode != 0)
	{
		return ;
	}

	//// 再获取在线状态
	//errCode = govideo_->GetAllDeviceStatus();
	//if (errCode != 0)
	//{
	//	return ;
	//}

	m_cDevList.DeleteAllItems();
	
	USES_CONVERSION;
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = govideo_->devices_.begin(); iter != govideo_->devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *device_info = *iter;
		if ((current_select_dgw_id_ == -1) || (current_select_dgw_id_ == device_info->dgw_server_id_))
		{
			int count = m_cDevList.GetItemCount();

			TCHAR str_val[64] = {0};
			_stprintf_s(str_val, 64, _T("%d"), count);
			m_cDevList.InsertItem(count, str_val);

			_stprintf_s(str_val, 64, _T("%d"), (*iter)->device_id_);
			m_cDevList.SetItemText(count, 1, str_val);

			m_cDevList.SetItemText(count, 2, A2T((*iter)->device_name_.c_str()));

			m_cDevList.SetItemText(count, 3, A2T((*iter)->gb28181_code_.c_str()));

			_stprintf_s(str_val, 64, _T("%d"), (*iter)->model_id_);
			m_cDevList.SetItemText(count, 4, str_val);
		}
	}
}

void CGxxGmDevMgrDlg::OnNMClickListDgws(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// 检查当前选中的是哪一项
	CString service_id_str = m_cDGWList.GetItemText(pNMItemActivate->iItem, 0);
	if (service_id_str.IsEmpty())
		return ;

	current_select_dgw_id_ = _ttoi(service_id_str.GetBuffer(0));

	m_cDevList.DeleteAllItems();

	// 根据DGW-ID，将所属的设备都列出来
	USES_CONVERSION;
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = govideo_->devices_.begin(); iter != govideo_->devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *device_info = *iter;

		if (device_info->dgw_server_id_ == current_select_dgw_id_)
		{
			int count = m_cDevList.GetItemCount();

			TCHAR str_val[64] = {0};
			_stprintf_s(str_val, 64, _T("%d"), count);
			m_cDevList.InsertItem(count, str_val);

			_stprintf_s(str_val, 64, _T("%d"), (*iter)->device_id_);
			m_cDevList.SetItemText(count, 1, str_val);

			m_cDevList.SetItemText(count, 2, A2T((*iter)->device_name_.c_str()));

			m_cDevList.SetItemText(count, 3, A2T((*iter)->gb28181_code_.c_str()));

			_stprintf_s(str_val, 64, _T("%d"), (*iter)->model_id_);
			m_cDevList.SetItemText(count, 4, str_val);
		}
	}

	// 然后将下面的DGWID下拉框调整为选中的DGW
	m_cDGWs.SetWindowText(service_id_str);

	*pResult = 0;
}

void CGxxGmDevMgrDlg::OnBnClickedBtnRegister()
{
	// 注册设备,从控件中拿出数据
	CString device_id;
	m_cDevID.GetWindowText(device_id);

	CString device_name;
	m_cDevName.GetWindowText(device_name);

	CString model_id;
	m_cModelID.GetWindowText(model_id);

	CString category_id;
	m_cCategoryID.GetWindowText(category_id);

	CString conn_info;
	m_cConnInfo.GetWindowText(conn_info);

	CString cfg_version;
	m_cCfgVersion.GetWindowText(cfg_version);

	CString username;
	m_cUsername.GetWindowText(username);

	CString password;
	m_cPassword.GetWindowText(password);

	CString dev_code;
	m_cDevCode.GetWindowText(dev_code);

	CString dev_ext_info;
	m_cExtInfo.GetWindowText(dev_ext_info);

	CString gbcode;
	m_cDevGBCdeo.GetWindowText(gbcode);

	CString name_abbr;
	m_cNameAbbr.GetWindowText(name_abbr);

	CString dev_version;
	m_cDevVersion.GetWindowText(dev_version);

	CString dgw_id;
	m_cDGWs.GetWindowText(dgw_id);

	USES_CONVERSION;
	GOVIDEO_DEVICE_INFO device_info;
	device_info.device_id_ = 0;
	device_info.device_name_ = T2A(device_name.GetBuffer(0));
	device_info.model_id_ = _ttoi(model_id.GetBuffer(0));
	device_info.category_id_ = _ttoi(category_id.GetBuffer(0));
	device_info.device_ip_ = T2A(conn_info.GetBuffer(0));
	device_info.version_ = _ttoi(cfg_version.GetBuffer(0));
	device_info.device_username_ = T2A(username.GetBuffer(0));
	device_info.device_password_ = T2A(password.GetBuffer(0));
	device_info.device_code_ = T2A(dev_code.GetBuffer(0));
	device_info.device_extra_info_ = T2A(dev_ext_info.GetBuffer(0));
	device_info.gb28181_code_ = T2A(gbcode.GetBuffer(0));
	device_info.device_name_abbr_ = T2A(name_abbr.GetBuffer(0));
	device_info.device_version_ = T2A(dev_version.GetBuffer(0));
	device_info.dgw_server_id_ = _ttoi(dgw_id.GetBuffer(0));

	// 调用添加设备接口
	int errCode = govideo_->RegisterDevice(device_info);
	if (errCode != 0)
		MessageBox(_T("注册设备失败！"), _T("错误"), MB_OK|MB_ICONERROR);
	else
	{
		MessageBox(_T("注册设备成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);

		m_cDevID.SetWindowText(_T(""));
		m_cDevName.SetWindowText(_T(""));
		m_cConnInfo.SetWindowText(_T(""));
		m_cCfgVersion.SetWindowText(_T(""));
		m_cUsername.SetWindowText(_T(""));
		m_cPassword.SetWindowText(_T(""));
		m_cDevCode.SetWindowText(_T(""));
		m_cExtInfo.SetWindowText(_T(""));
		m_cDevGBCdeo.SetWindowText(_T(""));
		m_cNameAbbr.SetWindowText(_T(""));
		m_cDevVersion.SetWindowText(_T(""));

		govideo_->DataUpdate();

		OnBnClickedBtnRefreshDevices();
	}
}

void CGxxGmDevMgrDlg::OnBnClickedBtnModify()
{
	// 
	CString device_id;
	m_cDevID.GetWindowText(device_id);

	CString device_name;
	m_cDevName.GetWindowText(device_name);

	CString model_id;
	m_cModelID.GetWindowText(model_id);

	CString category_id;
	m_cCategoryID.GetWindowText(category_id);

	CString conn_info;
	m_cConnInfo.GetWindowText(conn_info);

	CString cfg_version;
	m_cCfgVersion.GetWindowText(cfg_version);

	CString username;
	m_cUsername.GetWindowText(username);

	CString password;
	m_cPassword.GetWindowText(password);

	CString dev_code;
	m_cDevCode.GetWindowText(dev_code);

	CString dev_ext_info;
	m_cExtInfo.GetWindowText(dev_ext_info);

	CString gbcode;
	m_cDevGBCdeo.GetWindowText(gbcode);

	CString name_abbr;
	m_cNameAbbr.GetWindowText(name_abbr);

	CString dev_version;
	m_cDevVersion.GetWindowText(dev_version);

	CString dgw_id;
	m_cDGWs.GetWindowText(dgw_id);

	USES_CONVERSION;
	GOVIDEO_DEVICE_INFO device_info;
	device_info.device_id_ = _ttoi(device_id.GetBuffer(0));
	device_info.device_name_ = T2A(device_name.GetBuffer(0));
	device_info.model_id_ = _ttoi(model_id.GetBuffer(0));
	device_info.category_id_ = _ttoi(category_id.GetBuffer(0));
	device_info.device_ip_ = T2A(conn_info.GetBuffer(0));
	device_info.version_ = _ttoi(cfg_version.GetBuffer(0));
	device_info.device_username_ = T2A(username.GetBuffer(0));
	device_info.device_password_ = T2A(password.GetBuffer(0));
	device_info.device_code_ = T2A(dev_code.GetBuffer(0));
	device_info.device_extra_info_ = T2A(dev_ext_info.GetBuffer(0));
	device_info.gb28181_code_ = T2A(gbcode.GetBuffer(0));
	device_info.device_name_abbr_ = T2A(name_abbr.GetBuffer(0));
	device_info.device_version_ = T2A(dev_version.GetBuffer(0));
	device_info.dgw_server_id_ = _ttoi(dgw_id.GetBuffer(0));

	// 调用添加设备接口
	int errCode = govideo_->ModifyDevice(device_info);
	if (errCode != 0)
		MessageBox(_T("修改设备失败！"), _T("错误"), MB_OK|MB_ICONERROR);
	else
	{
		MessageBox(_T("修改设备成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);

		govideo_->DataUpdate();

		OnBnClickedBtnRefreshDevices();
	}
}

void CGxxGmDevMgrDlg::OnLvnKeydownListDevs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	USES_CONVERSION;
	// 找到删除键
	if (pLVKeyDow->wVKey == VK_DELETE)
	{
		INT_PTR ret = MessageBox(_T("确定要移除选中的设备吗？"), _T("警告"), MB_OKCANCEL|MB_ICONMASK);
		if (ret != IDOK)
			return ;

		// 找到当前选中的行
		POSITION pos = m_cDevList.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return ;

		do 
		{
			int index = m_cDevList.GetNextSelectedItem(pos);
			if (index != -1)
			{
				// 拿到设备内部id
				CString dev_id = m_cDevList.GetItemText(index, 1);

				// 先调用GoVideo的删除服务接口，成功后再调用界面移除记录
				int errCode = govideo_->UnregisterDevice(T2A(dev_id.GetBuffer(0)));
				if (errCode == 0)
					m_cDevList.DeleteItem(index);
				else
				{
					MessageBox(_T("移除设备失败！"), _T("错误"), MB_OK|MB_ICONERROR);
					return ;
				}

				govideo_->DataUpdate();
			}
			else
				break;

		} while (true);
	}

	*pResult = 0;
}

void CGxxGmDevMgrDlg::OnNMClickListDevs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int selected_item_index = pNMItemActivate->iItem;

	// 得到设备内部ID
	CString internal_id = m_cDevList.GetItemText(selected_item_index, 1);
	int device_id = _ttoi(internal_id.GetBuffer(0));

	// 寻找该设备
	USES_CONVERSION;
	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = govideo_->devices_.begin(); iter != govideo_->devices_.end(); ++iter)
	{
		GOVIDEO_DEVICE_INFO *device_info = *iter;

		if (device_info->device_id_ == device_id)
		{
			// 开始填入相关控件
			CString val;
			
			val.Format(_T("%d"), device_info->device_id_);
			m_cDevID.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_name_.c_str()));
			m_cDevName.SetWindowText(val);

			val.Format(_T("%d"), device_info->model_id_);
			m_cModelID.SetWindowText(val);

			val.Format(_T("%d"), device_info->category_id_);
			m_cCategoryID.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_ip_.c_str()));
			m_cConnInfo.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_version_.c_str()));
			m_cCfgVersion.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_username_.c_str()));
			m_cUsername.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_password_.c_str()));
			m_cPassword.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_code_.c_str()));
			m_cDevCode.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_extra_info_.c_str()));
			m_cExtInfo.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->gb28181_code_.c_str()));
			m_cDevGBCdeo.SetWindowText(val);

			val.Format(_T("%s"), A2T(device_info->device_name_abbr_.c_str()));
			m_cNameAbbr.SetWindowText(val);

			val.Format(_T("%d"), device_info->version_);
			m_cDevVersion.SetWindowText(val);

			// 这里需要处理一下，是不是考虑
			val.Format(_T("%d"), device_info->dgw_server_id_);
			int pos = m_cDGWs.FindString(0, val);
			if (pos >= 0)
				m_cDGWs.SetCurSel(pos);

			break;
		}
	}

	*pResult = 0;
}

void CGxxGmDevMgrDlg::OnBnClickedBtnRemove()
{
	INT_PTR ret = MessageBox(_T("确定要移除选中的设备吗？"), _T("警告"), MB_OKCANCEL|MB_ICONMASK);
	if (ret != IDOK)
		return ;

	// 找到当前选中的行
	USES_CONVERSION;
	POSITION pos = m_cDevList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return ;

	do 
	{
		int index = m_cDevList.GetNextSelectedItem(pos);
		if (index != -1)
		{
			// 拿到设备内部id
			CString dev_id = m_cDevList.GetItemText(index, 1);

			// 先调用GoVideo的删除服务接口，成功后再调用界面移除记录
			int errCode = govideo_->UnregisterDevice(T2A(dev_id.GetBuffer(0)));
			if (errCode == 0)
				m_cDevList.DeleteItem(index);
			else
			{
				MessageBox(_T("移除设备失败！"), _T("错误"), MB_OK|MB_ICONERROR);
				return ;
			}

			govideo_->DataUpdate();
		}
		else
			break;

	} while (true);
}

void CGxxGmDevMgrDlg::OnBnClickedBtnExportDevList()
{
	CFileDialog dlg(FALSE, _T("*.xls"), _T("设备信息表.xls"));
	INT_PTR ret = dlg.DoModal();
	if (ret != IDOK)
		return ;

	CString path = dlg.GetPathName();

	// 先查询所有设备列表
	govideo_->GetAllDevices();

	// 写入EXCEL
	bgMfcExcelModule excel;
	int errCode = excel.WriteInitialize(path.GetString(), L"设备表");
	if (errCode != 0)
	{
		MessageBox(_T("导出设备列表：初始化EXCEL失败！"), _T("错误"), MB_OK|MB_ICONERROR);
		return ;
	}

	std::vector<std::wstring> header;
	header.push_back(L"设备ID");
	header.push_back(L"设备名称");
	header.push_back(L"设备厂商");
	header.push_back(L"设备门类");
	header.push_back(L"连接信息");
	header.push_back(L"配置版本");
	header.push_back(L"用户名称");
	header.push_back(L"用户密码");
	header.push_back(L"设备编号");
	header.push_back(L"扩展信息");
	header.push_back(L"国标编码");
	header.push_back(L"缩写名称");
	header.push_back(L"设备版本");
	header.push_back(L"所属网关");

	errCode = excel.WriteHeader(header);
	if (errCode != 0)
	{
		MessageBox(_T("导出设备列表：向EXCEL写入表头失败！"), _T("错误"), MB_OK|MB_ICONERROR);
		return ;
	}

	std::vector<GOVIDEO_DEVICE_INFO *>::iterator iter;
	for (iter = govideo_->devices_.begin(); iter != govideo_->devices_.end(); ++iter)
	{
		std::vector<std::wstring> item;
		wchar_t value[4096] = {0};
		GOVIDEO_DEVICE_INFO *device_info = *iter;

		// 设备ID
		swprintf_s(value, 4096, L"%d", device_info->device_id_);
		item.push_back(value);

		// 设备名称
		MultiByteToWideChar(CP_ACP, 0, device_info->device_name_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 设备厂商
		swprintf_s(value, 4096, L"%d", device_info->model_id_);
		item.push_back(value);

		// 设备门类
		swprintf_s(value, 4096, L"%d", device_info->category_id_);
		item.push_back(value);

		// 连接信息
		MultiByteToWideChar(CP_ACP, 0, device_info->device_ip_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 配置版本
		MultiByteToWideChar(CP_ACP, 0, device_info->device_version_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 用户名称
		MultiByteToWideChar(CP_ACP, 0, device_info->device_username_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 用户密码
		MultiByteToWideChar(CP_ACP, 0, device_info->device_password_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 设备编号
		MultiByteToWideChar(CP_ACP, 0, device_info->device_code_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 扩展信息
		MultiByteToWideChar(CP_ACP, 0, device_info->device_extra_info_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 国标编码
		MultiByteToWideChar(CP_ACP, 0, device_info->gb28181_code_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 缩写名称
		MultiByteToWideChar(CP_ACP, 0, device_info->device_name_abbr_.c_str(), -1, value, 4096);
		item.push_back(value);

		// 设备版本
		swprintf_s(value, 4096, L"%d", device_info->version_);
		item.push_back(value);

		// 所属网关
		swprintf_s(value, 4096, L"%d", device_info->dgw_server_id_);
		item.push_back(value);

		errCode = excel.WriteLine(item);
	}

	excel.WriteFinish();

	MessageBox(_T("导出所有设备成功！"), _T("信息"), MB_OK|MB_ICONINFORMATION);
}

void CGxxGmDevMgrDlg::OnBnClickedBtnImportDevList()
{
	CFileDialog dlg(TRUE, _T("*.xls"), _T("设备信息表.xls"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Excel 2003 (*.xls)|*.xls|Excel 2007 (*.xlsx)|*.xlsx||"), this);
	INT_PTR ret = dlg.DoModal();
	if (ret != IDOK)
		return ;

	CString path = dlg.GetPathName();

	// 读取EXCEL
	bgMfcExcelModule excel;
	int errCode = excel.ReadInitialize(path.GetString(), L"设备表");
	if (errCode != 0)
	{
		return ;
	}

	std::vector<std::wstring> header;
	errCode = excel.ReadHeader(header);

	// 增加/修改/导入设备信息
	while (true)
	{
		char value[4096] = {0};
		std::vector<std::wstring> item;
		errCode = excel.ReadNextLine(item);
		if (errCode != 0)
		{
			break;
		}

		// 构建参数
		GOVIDEO_DEVICE_INFO device_info;
		// 设备ID
		device_info.device_id_ = _wtoi(item[0].c_str());

		// 设备名称
		WideCharToMultiByte(CP_UTF8, 0, item[1].c_str(), -1, value, 4096, "", NULL);
		device_info.device_name_ = value;

		// 设备厂商
		device_info.model_id_ = _wtoi(item[2].c_str());

		// 设备门类
		device_info.category_id_ = _wtoi(item[3].c_str());

		// 连接信息
		WideCharToMultiByte(CP_UTF8, 0, item[4].c_str(), -1, value, 4096, "", NULL);
		device_info.device_ip_ = value;
		
		// 配置版本
		WideCharToMultiByte(CP_UTF8, 0, item[5].c_str(), -1, value, 4096, "", NULL);
		device_info.device_version_ = value;

		// 用户名称
		WideCharToMultiByte(CP_UTF8, 0, item[6].c_str(), -1, value, 4096, "", NULL);
		device_info.device_username_ = value;

		// 用户密码
		WideCharToMultiByte(CP_UTF8, 0, item[7].c_str(), -1, value, 4096, "", NULL);
		device_info.device_password_ = value;

		// 设备编号
		WideCharToMultiByte(CP_UTF8, 0, item[8].c_str(), -1, value, 4096, "", NULL);
		device_info.device_code_ = value;

		// 扩展信息
		WideCharToMultiByte(CP_UTF8, 0, item[9].c_str(), -1, value, 4096, "", NULL);
		device_info.device_extra_info_ = value;

		// 国标编码
		WideCharToMultiByte(CP_UTF8, 0, item[10].c_str(), -1, value, 4096, "", NULL);
		device_info.gb28181_code_ = value;

		// 缩写名称
		WideCharToMultiByte(CP_UTF8, 0, item[11].c_str(), -1, value, 4096, "", NULL);
		device_info.device_name_abbr_ = value;

		// 设备版本
		device_info.version_ = _wtoi(item[12].c_str());

		// 所属网关
		device_info.dgw_server_id_ = _wtoi(item[13].c_str());
		
		// 调用更新接口
		int errCode = govideo_->ModifyDevice(device_info);
		if (errCode != 0)
		{
			//MessageBox(_T("修改设备失败！"), _T("错误"), MB_OK|MB_ICONERROR);
		}
		else
		{
			//MessageBox(_T("修改设备成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);

			govideo_->DataUpdate();

			
		}
	}

	OnBnClickedBtnRefreshDevices();
	MessageBox(_T("导入设备成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
}
