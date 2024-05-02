#include "QHttp.h"

QHttp::QHttp() {
    m_session = WinHttpOpen(L"My Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
}

QHttp::~QHttp()
{
    WinHttpCloseHandle(m_session);
    WinHttpCloseHandle(m_connect);
}
bool QHttp::open(std::string url)
{
    //分割domain和path
    if (url.find("http://")==0 )
    {
        url = url.substr(7);
        isHttps = false;
    }
    if ( url.find("https://") == 0)
    {
        url = url.substr(8);
        isHttps = true;
    }
    int idx = url.find_first_of('/');
    if (idx == std::string::npos) //没找到，默认path是/
    {
        m_domain = url;
        m_path = "/";
    }
    else {
        //找到path，进行分割
        m_domain = url.substr(0, idx);
        m_path = url.substr(idx);
    }
    m_connect = WinHttpConnect(m_session, CA2W(m_domain.c_str()), INTERNET_DEFAULT_PORT, 0);
    if (m_connect)
    {
        return true;
    }
    return false;
}

bool QHttp::get()
{
    bytesStr.clear();

    HINTERNET hRequest = WinHttpOpenRequest(m_connect, L"GET", CA2W(m_path.c_str()), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, isHttps? WINHTTP_FLAG_SECURE : 0 );
    if (hRequest)
    {
        //置入协议头
        std::string headers;
        for (std::map<std::string,std::string>::iterator it = m_headersMap.begin(); it!= m_headersMap.end() ; ++it)
        {  
            if ( std::next(it) == m_headersMap.end())
            {
                headers += it->first + ":" + it->second;
            }
            else {
                headers += it->first + ":" + it->second + "\r\n";
            }
        }


        if (!headers.empty())
        {
            WinHttpAddRequestHeaders(hRequest, CA2W(headers.c_str()), headers.size(), WINHTTP_ADDREQ_FLAG_ADD);
        }

        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
        {
            if (WinHttpReceiveResponse(hRequest, 0))
            {
                DWORD dwSize = 0;
                do
                {
                    // 获取响应大小
                    WinHttpQueryDataAvailable(hRequest, &dwSize);
                    // 分配缓冲区
                    LPBYTE pBuffer = new BYTE[dwSize];
                    // 读取响应数据
                    WinHttpReadData(hRequest, pBuffer, dwSize, NULL);

                    //bytesVec.push_back(pBuffer[0]);

                    bytesStr.append((char*)pBuffer,dwSize);

                    // 释放缓冲区
                    delete[] pBuffer;


                } while (dwSize > 0);

                return true;
            }
        }
    }
    return false;
}

BYTE* QHttp::getResponse()
{
    return (BYTE*)bytesStr.data();;
}

int QHttp::getResponseSize()
{
    return bytesStr.size();
}

std::string QHttp::getResponseText()
{
    return bytesStr.data();
}

void QHttp::addHeader(std::string key, std::string value)
{
    m_headersMap.insert(std::make_pair(key,value));
}
