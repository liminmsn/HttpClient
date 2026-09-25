#pragma once
#include <string>
#include <map>
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

namespace HttpClient {

	struct UrlInfo
	{
		std::wstring host;
		std::wstring path;
		INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;
		bool https = false;
	};

	bool ParseUrl(const std::wstring& url, UrlInfo& info);

	class HttpClient
	{
	private:
		HINTERNET session = nullptr;
		HINTERNET connection = nullptr;
		HINTERNET request = nullptr;

		UrlInfo info{};
		std::wstring headers;
	public:
		HttpClient& Get();
		HttpClient& Post(const std::string& body);
		HttpClient& Put(const std::string& body);
		HttpClient& Delete();
		HttpClient& Request(const std::wstring& method, const std::string& body = {});
		HttpClient& SetHeader(const std::wstring& name, const std::wstring& value);
		HttpClient& SetHeaders(const std::map<std::wstring, std::wstring>& headers);
		DWORD GetStateCode() const;

		std::string Send() const;

		bool Init(LPCWSTR host_name, const std::wstring& url);

		void SetTimeouts(
			IN int nResolveTimeout,
			IN int nConnectTimeout,
			IN int nSendTimeout,
			IN int nReceiveTimeout
		) const;
	};
}