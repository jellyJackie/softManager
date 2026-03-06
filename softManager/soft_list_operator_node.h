#pragma once

#include <string>

#include <UIlib.h>

#include <curl/curl.h>

class Scheme;

class UpdateButtonUI : public DuiLib::CButtonUI, public DuiLib::INotifyUI
{
public:
	void SetDownloadUrl(const char* url);

	void SetKeyName(const wchar_t* value);

	void SetActions(const std::vector<std::map<std::wstring, std::wstring>>& actions);
protected:
	void DoInit() override;

	void NotifyClickedUpdateButton(DuiLib::TNotifyUI& msg);

	void Notify(DuiLib::TNotifyUI& msg) override;
private:
	static int OnDownloadProgress(void* user_ptr, const char* url,
								  double rate);

	static void OnDownloadFinished(void* user_ptr, const char* url,
								   const wchar_t* file_path,
								   CURLcode code, int http_code);

	static void OnDownloadNotify(void* user_ptr, const char* url, const char* msg);

	std::string download_url_;
	std::wstring key_name_;

	std::vector<std::map<std::wstring, std::wstring>> actions_;
};











class SoftListOperatorNode : public DuiLib::CHorizontalLayoutUI, public DuiLib::INotifyUI {
public:
	~SoftListOperatorNode() override;

	void SetScheme(Scheme* scheme);

	void SetMessage(const wchar_t* value);

	void SetKeyName(const wchar_t* value);

	void SetUninstallPath(const wchar_t* value);

	void SetLocalVersion(const char* value);

	void SetUpdateInfo(const char* last_version, const char* download_url,
					   const std::vector<std::map<std::wstring, std::wstring>>& actions, bool cracked);

	void SetUpdateText(const wchar_t* value) const;

	void UpdateMessage(const wchar_t* value);

	void UpdateUpgradeInfo(const char* last_version, const char* download_url);
protected:
	void Init() override;

	void Notify(DuiLib::TNotifyUI& msg) override;
private:
	DuiLib::CButtonUI* CreateUninstallButton() const;

	DuiLib::CButtonUI* CreateUpdateButton() const;

	DuiLib::CLabelUI* CreateInfoLabel() const;

	void Uninstall(std::wstring_view cmd);

	void UpdateState() const;

	DuiLib::CButtonUI* btn_uninstall_ = nullptr;
	DuiLib::CButtonUI* btn_update_ = nullptr;

	DuiLib::CTabLayoutUI* tab_layout_ = nullptr;

	Scheme* scheme_ = nullptr;

	std::wstring message_;
	std::wstring key_name_;
	std::wstring uninstall_path_;

	std::string local_version_;
	std::string last_version_;
	std::string download_url_;

	bool cracked_ = false;

	std::vector<std::map<std::wstring, std::wstring>> actions_;
};
