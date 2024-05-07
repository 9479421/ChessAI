#pragma once
#include"pch.h"
#include<string>

#include<vector>
#include<map>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

class QHttp
{	
public:
	QHttp();
	~QHttp();
	bool open(std::string url);
	bool get();
	bool post(std::string data);
	void setTimeout(int          nResolveTimeout,
		int          nConnectTimeout,
		int          nSendTimeout,
		int          nReceiveTimeout);
	BYTE* getResponse();
	int getResponseSize();
	std::string getResponseText();

	void addHeader(std::string key, std::string value);
private:
	HINTERNET m_connect;
	HINTERNET m_session;

	std::string m_domain;
	std::string m_path;

	std::map<std::string, std::string> m_headersMap;

	bool isHttps;
public:
	std::vector<BYTE> bytesVec;
	std::string bytesStr;
};

