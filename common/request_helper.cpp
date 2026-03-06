#include "request_helper.h"

#include <jsoncons/json.hpp>

#include "curl_request.h"
#include "file_helper.h"
#include "helper.h"
#include "kf_str.h"
#include "stringHelper.h"

bool RequestHelper::CheckUpdate(const char* request_body, std::string* response_body) {
	const auto url = KfString::Format("%s/checkSoftUpdate", GetHost().c_str());

	CurlRequest client;
	client.SetUrl(url.GetString());
	return client.DoPost(request_body, response_body);
}

bool RequestHelper::CheckUpdate(std::string* response_body) {
	const auto url = KfString::Format("%s/checkMainUpdate", GetHost().c_str());

	jsoncons::json request;

	wchar_t file_name[MAX_PATH];
	ZeroMemory(file_name, sizeof(wchar_t) * MAX_PATH);
	GetModuleFileName(nullptr, file_name, MAX_PATH);

	request["version"] = KfString(FileHelper::GetFileVersion(file_name).c_str()).GetString();
	
	std::string request_body;
	request.dump(request_body);

	CurlRequest client;
	client.SetUrl(url.GetString());
	return client.DoPost(request_body.c_str(), response_body);
}

bool RequestHelper::Report(ReportType type, const wchar_t* comment) {
	const auto url = KfString::Format("%s/report", GetHost().c_str());

	wchar_t file_name[MAX_PATH];
	ZeroMemory(file_name, sizeof(wchar_t) * MAX_PATH);
	GetModuleFileName(nullptr, file_name, MAX_PATH);

	jsoncons::json request;

	request["type"] = static_cast<int>(type);
	request["comment"] = CStringHelper::w2u(comment);
	request["mid"] = Helper::GetCpuId();
	request["version"] = KfString(FileHelper::GetFileVersion(file_name).c_str()).GetString();

	std::string request_body;
	request.dump(request_body);

	std::string response_body;

	CurlRequest client;
	client.SetUrl(url.GetString());
	return client.DoPost(request_body.c_str(), &response_body);
}

std::string RequestHelper::GetHost() {
	return KfString::Format("%s:5443", ip_.c_str()).GetString();
}
