#pragma once

#include <functional>
#include <curl/curl.h>

typedef std::function<int(void*, const char*, double, double, int, long long, long long)> ptrDownloadProgressFunction;
typedef std::function<void(void*, const char*, const wchar_t*, CURLcode, int)> ptrDownloadFinishedCallback;

typedef int (*ptrDownloadProgressSingleThreadFunction)(void* clientp, const char* sign, double dltotal,
													   double dlnow, double speed);

struct DownloadNode
{
	long long start_pos;
	long long record_start_pos;
	long long end_pos;
	long long total_download_size;
	CURL* curl;
	FILE* file;
	uint8_t index;
	ptrDownloadProgressFunction download_progress_callback_ = nullptr;
	void* download_progress_callback_data_ = nullptr;
	const char* download_progress_callback_sign_ = nullptr;
};

class CurlDownloadRequest
{
public:
	void SetUrl(const char* url);

	void SetDownloadProgressCallback(const ptrDownloadProgressFunction& callback,
									 void* data, const char* sign);

	void SetDownloadSingleProgressCallback(const ptrDownloadProgressSingleThreadFunction& callback,
										   void* data, const char* sign);

	void SetDownloadFinishedCallback(const ptrDownloadFinishedCallback& callback, void* data,
									 const char* sign);

	double GetContentLength(bool* accept_ranges, std::wstring* ptr_file_name,
							int retry_count = 3) const;

	void MultipleDownloadFile(FILE* file, double content_length, uint16_t thread_count);

	// 多线程下载
	long DownloadFile(double content_length, std::wstring_view target_file_path, unsigned thread_count);

	// 单线程下载
	bool DownloadSingleThreadFile(const wchar_t* target_file_path);

	void StopDownload();

	bool IsStop() const;
protected:
	static size_t WriteDownloadFunction(void* buffer, size_t size, size_t nmemb, void* user_ptr);

	static size_t WriteSingleThreadDownloadFunction(void* buffer, size_t size, size_t nmemb, void* user_ptr);

	static int ProgressFunction(void* ptr, double total_to_download, double now_downloaded,
								double total_to_upload, double now_uploaded);

	static int SingleProcessProgressFunction(void* ptr, double total_to_download, double now_downloaded,
											 double total_to_upload, double now_uploaded);
private:
	void Download(DownloadNode* node);
	
	std::string url_;

	ptrDownloadProgressFunction download_progress_callback_ = nullptr;
	void* download_progress_callback_data_ = nullptr;
	const char* download_progress_callback_sign_ = nullptr;

	ptrDownloadProgressSingleThreadFunction download_progress_single_thread_callback_ = nullptr;
	void* download_progress_single_thread_callback_data_ = nullptr;
	std::string download_progress_single_thread_sign_;

	ptrDownloadFinishedCallback download_finished_callback_ = nullptr;
	void* download_finished_callback_data_ = nullptr;
	std::string download_finished_callback_sign_;

	inline static bool stop_download_ = false;

	CURLcode download_result_code_ = CURLE_OK;

	CURL* single_curl_ = nullptr;
};
