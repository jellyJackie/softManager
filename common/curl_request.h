#pragma once

#include <string>
#include <curl/curl.h>

class HeaderData;

typedef int (*ptrProgressFunction)(void* ptr, double total_to_download, double now_downloaded,
								   double total_to_upload, double now_uploaded);

typedef void (*ptrFinishedCallback)(CURLcode code, void* ptr, std::wstring_view file_path);

class CurlRequest
{
public:
	CurlRequest();

	~CurlRequest();

	void SetUrl(std::string_view url) const;

	void SetDownloadFilePath(std::wstring_view dest_file_path);

	void SetDownloadProgressCallback(ptrProgressFunction callback, void* data);

	void SetDownloadFinishedCallback(ptrFinishedCallback callback, void* data);

	long DownloadFile();

	long ReDownloadFile();

	bool DoPost(const char* request_body, std::string* ptr_response_body) const;
private:
	ptrProgressFunction download_progress_callback_ = nullptr;
	ptrFinishedCallback download_finished_callback_ = nullptr;
	void* data_ = nullptr;
	HeaderData* header_data_ = nullptr;
	CURL* curl_ = nullptr;
};
