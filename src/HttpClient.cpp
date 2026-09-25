#include "HttpClient/HttpClient.hpp"

namespace HttpClient {
	bool ParseUrl(const std::wstring& url, UrlInfo& info)
	{
		URL_COMPONENTS components{};
		components.dwStructSize = sizeof(components);

		wchar_t host[256]{};
		wchar_t path[2048]{};

		components.lpszHostName = host;
		components.dwHostNameLength = _countof(host);

		components.lpszUrlPath = path;
		components.dwUrlPathLength = _countof(path);

		if (!WinHttpCrackUrl(url.c_str(), static_cast<DWORD>(url.length()), 0, &components))
		{
			return false;
		}

		info.host.assign(components.lpszHostName, components.dwHostNameLength);
		info.path.assign(components.lpszUrlPath, components.dwUrlPathLength);

		if (components.dwExtraInfoLength > 0)
		{
			info.path.append(components.lpszExtraInfo, components.dwExtraInfoLength);
		}

		info.port = components.nPort;
		info.https = components.nScheme == INTERNET_SCHEME_HTTPS;

		return true;
	}

	bool HttpClient::Init(LPCWSTR host_name, const std::wstring& url)
	{
		session = WinHttpOpen(
			host_name,
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0
		);
		if (!ParseUrl(url, info))
			return false;

		connection = WinHttpConnect(session, info.host.c_str(), info.port, 0);

		return true;
	}

	HttpClient& HttpClient::SetHeaders(const std::map<std::wstring, std::wstring>& headers)
	{
		for (const auto& [name, value] : headers)
		{
			SetHeader(name, value);
		}

		return *this;
	}

	HttpClient& HttpClient::SetHeader(
		const std::wstring& name,
		const std::wstring& value)
	{
		headers += name;
		headers += L": ";
		headers += value;
		headers += L"\r\n";

		return *this;
	}

	HttpClient& HttpClient::Get()
	{
		return Request(L"GET");
	}

	HttpClient& HttpClient::Post(const std::string& body)
	{
		return Request(L"POST", body);
	}

	HttpClient& HttpClient::Put(const std::string& body)
	{
		return Request(L"PUT", body);
	}

	HttpClient& HttpClient::Delete()
	{
		return Request(L"DELETE");
	}

	std::string HttpClient::Send() const
	{
		std::string response;
		DWORD available = 0;

		while (WinHttpQueryDataAvailable(request, &available))
		{
			if (available == 0)
				break;

			std::string buffer(available, '\0');

			DWORD bytesRead = 0;

			if (!WinHttpReadData(
				request,
				buffer.data(),
				available,
				&bytesRead))
			{
				break;
			}

			response.append(buffer.data(), bytesRead);
		}

		return response;
	}

	void HttpClient::SetTimeouts(IN int nResolveTimeout, IN int nConnectTimeout, IN int nSendTimeout, IN int nReceiveTimeout) const
	{
		if (this->request) {
			WinHttpSetTimeouts(request, nResolveTimeout, nConnectTimeout, nSendTimeout, nReceiveTimeout);
		}
	}

	DWORD HttpClient::GetStateCode() const {
		DWORD statusCode = 0;
		DWORD size = sizeof(statusCode);

		WinHttpQueryHeaders(
			request,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&statusCode,
			&size,
			WINHTTP_NO_HEADER_INDEX
		);
		return statusCode;
	}

	HttpClient& HttpClient::Request(const std::wstring& method, const std::string& body)
	{
		DWORD flags = info.https ? WINHTTP_FLAG_SECURE : 0;

		request = WinHttpOpenRequest(
			connection,
			method.c_str(),
			info.path.c_str(),
			nullptr,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			flags
		);

		if (!request)
			return *this;

		BOOL result = WinHttpSendRequest(
			request,
			headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headers.c_str(),
			headers.empty() ? 0 : static_cast<DWORD>(-1L),
			body.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)body.data(),
			static_cast<DWORD>(body.size()),
			static_cast<DWORD>(body.size()),
			0
		);

		if (!result)
			return *this;

		WinHttpReceiveResponse(request, nullptr);

		return *this;
	}
}