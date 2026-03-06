#pragma once

#include <memory>
#include <string>
#include <thread>
#include <curl/curl.h>

#include "WndImpl.h"

class Scheme;
class CurlDownloadRequest;

class DownloadElementUI : public DuiLib::CListContainerElementUI {
public:
	~DownloadElementUI();

	void SetUrl(const char* url);

	void SetAcceptRanges(bool value);

	void SetSize(double value);

	void SetSavePath(const wchar_t* value);
protected:
	void Init() override;

	static int OnProgressCallback(void* ptr, const char* url, double rate);

	static void OnFinishedCallback(void* ptr, const char* url,
								   const wchar_t* file_path, CURLcode code,
								   int http_code);

	static void OnNotifyCallback(void* ptr, const char* url, const char* msg);
private:
	std::string url_;

	bool accept_ranges_ = false;

	double size_ = 0.0f;

	std::wstring save_path_;

	std::thread thread_;
};











class MainWindow : public CWndImpl
{
protected:
	LPCTSTR GetSkinFile() override;

	void Init() override;

	void NotifyAddTask();

	void Notify(DuiLib::TNotifyUI& msg) override;

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
	std::shared_ptr<Scheme> scheme_;
};
