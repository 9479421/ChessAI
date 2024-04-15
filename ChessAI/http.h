#pragma once

#include<string>
#include<map>
#include<sstream>

#include<winsock2.h>
#pragma comment (lib,"ws2_32.lib")

#define HEAD_Content_Type_Form "application/x-www-form-urlencoded; charset=utf-8"
#define HEAD_Content_Type_Json "application/json; charset=utf-8"
#define HEAD_Content_Type_Xml "application/xml; charset=utf-8"





class http
{
private:
	SOCKET sock;
	std::string url;
	std::string domain;

	std::string Cookie;

	std::string UserAgent;
	std::string Accept;
	std::string AcceptEncoding;
	std::string Connection;
	std::string ContentType;
	std::string ContentLength;


	std::map<std::string, std::string> headerMap;
public:
	boolean open(std::string url);
	std::string get();
	std::string post(std::string body);

private:
	std::string call(std::string method,std::string data);
};

