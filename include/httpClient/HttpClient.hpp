#pragma once
#include <string>
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
class HttpClient
{
private:
	HINTERNET session;
	HINTERNET connection;
	HINTERNET request;
public:
	HttpClient& Init(LPCWSTR host_name, LPCWSTR server, const int port);
	HttpClient& SetTimeouts(IN int nResolveTimeout, IN int nConnectTimeout, IN int nSendTimeout, IN int nReceiveTimeout);
	HttpClient& Get(const std::wstring& url);
	std::string Send() const;
};