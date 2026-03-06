#include "mainWindow.h"

#include "curl_request.h"
#include "define.h"
#include "event_queue_global_manager.h"
#include "file_helper.h"
#include "helper.h"
#include "installedWnd.h"
#include "log_helper.h"
#include "kf_str.h"
#include "request_helper.h"
#include "scheme.h"
#include "softInfo.h"
#include "stringHelper.h"
#include "UpdateDetection.h"
#include "updateWnd.h"
#include "update_main_window.h"
#include "versionHelper.h"

CMainWindow::~CMainWindow() {
	UpdateInstance->GetDataCenter()->Detach(this);
}

void CMainWindow::NotifyClickMenu(DuiLib::TNotifyUI& msg) {
	if (nullptr != menu_wnd_) {
		delete menu_wnd_;
		menu_wnd_ = nullptr;
	}

	const RECT menu_pos = msg.pSender->GetPos();

	POINT pt = { menu_pos.left, menu_pos.bottom + 2 };

	ClientToScreen(m_hWnd, &pt);

	menu_wnd_ = DuiLib::CMenuWnd::CreateMenu(nullptr, TEXT("interface_menu.xml"),
											 pt, &m_pm);

	auto menu = menu_wnd_->GetMenuUI();

	menu->SetBkColor(scheme_->GetBkColor());
	menu->SetSelectedItemBkColor(scheme_->GetHotBkColor());

	menu->SetItemTextColor(scheme_->GetTextColor());
	menu->SetSelectedItemTextColor(scheme_->GetTextColor());
}

void CMainWindow::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK) {
		DuiLib::CDuiString strName = msg.pSender->GetName();
		if (0 == strName.CompareNoCase(L"closebtn")) {
			PostQuitMessage(0);
			return;
		}

		if (0 == strName.CompareNoCase(L"minbtn")) {
			::ShowWindow(m_hWnd, SW_MINIMIZE);
			return;
		}

		if(0 == strName.CompareNoCase(L"btn_menu")) {
			NotifyClickMenu(msg);
			return;
		}

		if (0 == strName.CompareNoCase(L"soft_updater_btn")) {
			update_wnd_ = std::make_shared<CUpdateWnd>();
			update_wnd_->Create(m_hWnd, L"", UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			update_wnd_->CenterWindow();

			CEventQueueGlobalManager::GetInstance()->PostEvent(EVENT_UPDATE_SOFT_LIST);

			update_wnd_->ShowModal();
			return;
		}

		if(0 == strName.CompareNoCase(L"soft_installed_btn")) {
			installed_wnd_ = std::make_shared<CInstalledWnd>();
			installed_wnd_->Create(m_hWnd, L"", UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			installed_wnd_->CenterWindow();
			installed_wnd_->ShowModal();
		}
	}
}

LPCTSTR CMainWindow::GetSkinFile()
{
	return L"main.xml";
}

void CMainWindow::Init()
{
	UpdateInstance->GetDataCenter()->Attach(this);

	EventQueueInstance->AppendCurrentThreadListener(EVENT_UPDATE_STATUS_LABEL,
													OnEventUpdateStatusLabel, this);
	EventQueueInstance->AppendNewThreadListener(EVENT_UPDATE_SOFT_LIST,
												OnEventUpdateSoftList, this);

	EventQueueInstance->AppendNewThreadListener(EVENT_UPDATE_SOFT_DATA,
													OnEventUpdateSoftData, this);

	wchar_t file_name[MAX_PATH];
	ZeroMemory(file_name, sizeof(wchar_t) * MAX_PATH);
	GetModuleFileName(nullptr, file_name, MAX_PATH);

	auto file_version = FileHelper::GetFileVersion(file_name);

	wchar_t title[128] = { 0 };
	swprintf_s(title, L"软件更新助手(%s)", file_version.c_str());

	m_pm.FindControl(L"soft_name_label")->SetText(title);

	HANDLE hThread = CreateThread(nullptr, 0, ThreadGetUpdateInfo, this, 0, nullptr);
	CloseHandle(hThread);
}

LRESULT CMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if(WM_USER_SHOW_UPDATE_WINDOW == uMsg) {
		update_main_wnd_ = std::make_shared<UpdateMainWindow>();
		update_main_wnd_->SetUpdateData(reinterpret_cast<char*>(wParam));
		update_main_wnd_->Create(m_hWnd, L"", UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		update_main_wnd_->CenterWindow();
		update_main_wnd_->ShowModal();
	}

	if(DuiLib::UIMSG_MENUCLICK == uMsg) {
		auto menu_cmd = reinterpret_cast<DuiLib::MenuCmd*>(wParam);

		if (0 == _wcsicmp(menu_cmd->szName, L"menu_dark_scheme")) {
			scheme_->SetDarkMode(&m_pm);
		}

		if(0 == _wcsicmp(menu_cmd->szName, L"menu_light_scheme")) {
			scheme_->SetLightMode(&m_pm);
		}

		delete menu_cmd;
		menu_cmd = nullptr;
	}

	return CWndImpl::HandleMessage(uMsg, wParam, lParam);
}

DWORD CMainWindow::ThreadGetUpdateInfo(LPVOID lParam) {
	CMainWindow* pThis = (CMainWindow*)lParam;

	RequestHelper::Report(ReportType::TYPE_OPEN, L"");

	if(!IsDebuggerPresent()) {
		Helper::UpdateStatusLabel(L"检查主程序更新");

		pThis->CheckUpdate();
	}

	EventQueueInstance->PostEvent(EVENT_UPDATE_SOFT_DATA);
	return 0;
}

void CMainWindow::UpdateDate(bool need_update, void* data, size_t update_count) {
	if (!need_update) {
		return;
	}

	auto info = (UpdateInfo*)data;

	VersionHelper remote_version(info->version.c_str());

	for (const auto& soft_it : CSoftInfo::GetInstance()->GetSoftInfo()) {
		if (!CStringHelper::IsMatch(CStringHelper::w2a(soft_it.m_strSoftName.GetString()),
									info->name)) {
			continue;			
		}

		VersionHelper local_version(CStringHelper::w2a(soft_it.m_strSoftVersion.GetString()).c_str());

		if(local_version >= remote_version) {
			break;
		}

		UpdateUpdateCount(update_count);

		break;
	}
}

void CMainWindow::UpdateUpdateCount(int value) const {
	const auto pLabel = m_pm.FindControl(L"soft_updater_btn");

	pLabel->SetText(std::to_wstring(value).c_str());
	pLabel->SetToolTip(L"可升级软件数");
}

void CMainWindow::ClearData() {
	auto pLabel = m_pm.FindControl(L"soft_updater_btn");
	pLabel->SetText(L"");
}

void CMainWindow::ClearData(void* data) {
	UpdateUpdateCount(-1);
}

DWORD CMainWindow::OnEventUpdateStatusLabel(WPARAM wParam, LPARAM lParam, LPVOID data) {
	auto pThis = static_cast<CMainWindow*>(data);

	auto status_text = reinterpret_cast<wchar_t*>(wParam);

	if(nullptr == status_text) {
		pThis->m_pm.FindControl(L"label_status")->SetText(L"");
		return 0;
	}

	pThis->m_pm.FindControl(L"label_status")->SetText(status_text);

	delete status_text;
	status_text = nullptr;

	return 0;
}

DWORD CMainWindow::OnEventUpdateSoftList(WPARAM wParam, LPARAM lParam, LPVOID data) {
	auto pThis = static_cast<CMainWindow*>(data);

	if(pThis->installed_wnd_ && IsWindow(pThis->installed_wnd_->m_hWnd)) {
		pThis->installed_wnd_->UpdateSoftInfo();
	}

	if (pThis->update_wnd_ && IsWindow(pThis->update_wnd_->m_hWnd)) {
		CEventQueueGlobalManager::GetInstance()->PostEvent(EVENT_REFRESH_UPDATE_WND_SOFT_LIST);
		KF_INFO("post event EVENT_REFRESH_UPDATE_WND_SOFT_LIST");
	}

	if(pThis && IsWindow(pThis->m_hWnd)) {
		// 更新已经安装的软件总个数
		auto pLabel = static_cast<DuiLib::CLabelUI*>(pThis->m_pm.FindControl(L"soft_installed_btn"));
		if (pLabel != nullptr) {
			DuiLib::CDuiString strText;
			strText.Format(L"%d", CSoftInfo::GetInstance()->GetSoftInfo().size());

			pLabel->SetText(strText);
		}
	}	

	return 0;
}

DWORD CMainWindow::OnEventUpdateSoftData(WPARAM wParam, LPARAM lParam, LPVOID data) {
	auto pThis = static_cast<CMainWindow*>(data);

	Helper::UpdateStatusLabel(L"正在获取软件列表...");

	if(0 != wParam) {
		CSoftInfo::GetInstance()->UpdateSoftInfo(reinterpret_cast<const wchar_t*>(wParam));
	}

	auto soft_info = CSoftInfo::GetInstance()->GetSoftInfo();

	// 展示已经安装的软件总个数
	DuiLib::CLabelUI* pLabel = static_cast<DuiLib::CLabelUI*>(pThis->m_pm.FindControl(L"soft_installed_btn"));
	if (pLabel != nullptr) {
		DuiLib::CDuiString strText;
		strText.Format(L"%d", soft_info.size());

		pLabel->SetText(strText);
	}

	if(0 != wParam) {
		auto find_soft_info = [wParam](const SoftInfo& info) {
			return info.key_name == reinterpret_cast<const wchar_t*>(wParam);
		};

		const auto it_find = std::find_if(soft_info.begin(), soft_info.end(), find_soft_info);
		if(it_find != soft_info.end()) {
			UpdateInstance->Run(*it_find);
		}

		return 0;
	}

	UpdateInstance->Run(soft_info, 10);

	return 0;
}

void CMainWindow::DownloadFinishedCallback(CURLcode code, void* ptr, std::wstring_view file_path) {
	if(!IsDebuggerPresent()) {
		wchar_t exe_path[MAX_PATH];
		ZeroMemory(exe_path, MAX_PATH * sizeof(wchar_t));
		GetModuleFileName(nullptr, exe_path, MAX_PATH);

		wchar_t bak_path[MAX_PATH];
		ZeroMemory(bak_path, MAX_PATH * sizeof(wchar_t));
		wcscpy_s(bak_path, exe_path);
		wcscat_s(bak_path, L".bak");

		MoveFile(exe_path, bak_path);

		const auto old_ver = FileHelper::GetFileVersion(bak_path);
		const auto new_ver = FileHelper::GetFileVersion(file_path.data());

		MoveFile(file_path.data(), exe_path);

		if (!DeleteFile(bak_path)) {
			MoveFileEx(bak_path, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
		}

		jsoncons::json report;
		report["old_ver"] = CStringHelper::w2u(old_ver);
		report["new_ver"] = CStringHelper::w2u(new_ver);

		std::string report_data;
		report.dump(report_data);

		RequestHelper::Report(ReportType::TYPE_UPDATE, CStringHelper::u2w(report_data).c_str());
	}

	auto pThis = static_cast<CMainWindow*>(ptr);
	if(pThis->update_main_wnd_.get() && IsWindow(pThis->update_main_wnd_->m_hWnd)) {
		pThis->update_main_wnd_->EnableRestartButton();
	}
}

bool CMainWindow::CheckUpdate() {
	std::string response_body;
	if (!RequestHelper::CheckUpdate(&response_body)) {
		return false;
	}

	jsoncons::json response = jsoncons::json::parse(response_body);
	if (response["need_update"].as_bool()) {
		std::string data;
		response["data"].dump(data);

		char* update_data = new char[data.length() + 1];
		ZeroMemory(update_data, data.length() + 1);
		strcpy(update_data, data.c_str());

		PostMessage(WM_USER_SHOW_UPDATE_WINDOW, reinterpret_cast<WPARAM>(update_data), 0);

		CurlRequest client;
		client.SetUrl(response["package_url"].as_string());
		client.SetDownloadFilePath(Helper::GetCacheFile(L""));
		client.SetDownloadFinishedCallback(DownloadFinishedCallback, this);
		client.DownloadFile();
	}

	return true;
}