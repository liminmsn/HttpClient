#include "HttpClient/HttpClient.hpp"

HttpClient& HttpClient::Init(LPCWSTR host_name, LPCWSTR server, const int port) {
	// 1. 创建 WinHTTP Session
	session = WinHttpOpen(
		host_name,
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0
	);
	// 2. 连接服务器
	connection = WinHttpConnect(session, server, port, 0);
	return *this;
}

HttpClient& HttpClient::SetTimeouts(IN int  nResolveTimeout, IN int  nConnectTimeout, IN int  nSendTimeout, IN int  nReceiveTimeout)
{
	if (this->request) {
		WinHttpSetTimeouts(request, nResolveTimeout, nConnectTimeout, nSendTimeout, nReceiveTimeout);
	}
	return *this;
}

HttpClient& HttpClient::Get(const std::wstring& url)
{
	HINTERNET request = WinHttpOpenRequest(
		connection,
		L"GET",
		L"/",
		nullptr,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE
	);
	return *this;
}

std::string HttpClient::Send() const {
	std::string response;
	DWORD available = 0;
	while (WinHttpQueryDataAvailable(request, &available))
	{
		if (available == 0) break;

		std::string buffer(available, '\0');

		DWORD bytesRead = 0;
		if (!WinHttpReadData(request, buffer.data(), available, &bytesRead))
		{
			break;
		}

		response.append(buffer.data(), bytesRead);
	}
	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);
	WinHttpCloseHandle(session);

	return response;
}