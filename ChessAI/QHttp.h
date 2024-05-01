#pragma once
#include"pch.h"
#include<string>

#include<vector>
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
	BYTE* getResponse();
	int getResponseSize();
	std::string getResponseText();


private:
	HINTERNET m_session;
	HINTERNET m_connect;

	std::string m_domain;
	std::string m_path;

public:
	std::vector<BYTE> bytesVec;
	std::string bytesStr;
};

