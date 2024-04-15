#include "pch.h"
#include "http.h"

BOOL GetIpByDomainName(char* szHost, char* szIp)
{
    HOSTENT* pHostEnt;
    int             nAdapter = 0;
    struct       sockaddr_in   sAddr;

    pHostEnt = gethostbyname(szHost);
    if (pHostEnt)
    {
        if (pHostEnt->h_addr_list[nAdapter])
        {
            memcpy(&sAddr.sin_addr.s_addr, pHostEnt->h_addr_list[nAdapter], pHostEnt->h_length);
            sprintf(szIp, "%s", inet_ntoa(sAddr.sin_addr));
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

bool containsLetter(const std::string& str) {
    for (char c : str) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            return true; // 如果找到字母，返回true
        }
    }
    return false; // 如果没有找到字母，返回false
}

boolean http::open(std::string url)
{
    this->url = url;
    //获取domain 
    int idx1 = url.find("://") + 3;
    int idx2 = url.find("/", idx1);

    if (idx2 == std::string::npos)
    {
        domain = url.substr(idx1);
    }
    else {
        domain = url.substr(idx1, idx2 - idx1);
    }
    //domain = "192.168.89.128:9006"; //www.chessdb.cn

    //初始化套接字
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return false;
    }

  

    SOCKADDR_IN serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //判断url是域名还是ip:port
    if (containsLetter(domain))
    {
        struct hostent* host;
        host = gethostbyname(domain.c_str());
        serv_addr.sin_port = htons(80);
        serv_addr.sin_addr.S_un.S_addr = *((unsigned long*)host->h_addr);
    }
    else {


        int idx = domain.find(":");
        std::string ip;
        std::string port;
        ip = domain.substr(0, idx);
        port = domain.substr(idx+1);


        //inet_addr();

        serv_addr.sin_port = htons(atoi(port.c_str()));
        serv_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    }




    


    //memcpy(&serv_addr.sin_addr, gethostbyname("www.chessdb.cn")->h_addr, 4); //获取域名对应的第一个ip
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        return false;
    }
    //设置延时
    int timeout = 3000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));


    return true;
}

std::string http::get()
{
    return  call("GET", "");
}

std::string http::call(std::string method, std::string data)
{
    std::string responseBodyStr;



    std::stringstream request;
    request << method << " " << this->url << " HTTP/1.1\r\n";
    request << "Host: " << domain << "\r\n";
    //request << "Connection: keep-alive"  << "\r\n";
    //request << "Cache-Control: max-age=0"  << "\r\n";
    //request << "Upgrade-Insecure-Requests: 1"  << "\r\n";
    request << "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36"  << "\r\n";
    //request << "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7"  << "\r\n";
    //request << "Accept-Encoding: gzip, deflate"  << "\r\n";
    //request << "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8"  << "\r\n";


    if (data!="")
    {
        request << "Content-Length:" << data.length() << "\r\n";
    }
    if (this->Cookie != "")
    {
        request << this->Cookie + "\r\n";
    }
    request << "\r\n";
    request << data;
    
    std::string requestStr = request.str();
    if (send(sock, requestStr.c_str(), requestStr.size(), 0) <= 0 ) {
        return "";
    }

    char buf[1024]{0};
    std::string recvStr;
    int nDataLength;
    while ((nDataLength = recv(sock, buf, sizeof(buf) - 1, 0)) > 0) {
        //printf("length:%d\n", nDataLength);
        recvStr += buf;
        //printf("%s", buf);
        if (nDataLength < sizeof(buf) - 1)
        {
            break;
        }
        memset(buf, 0, 1024);
    }
    //printf("%s\n", recvStr.c_str());


    try {
        std::string headerStr;
        std::string bodyStr;
        int splitIndex = recvStr.find("\r\n\r\n");
        headerStr = recvStr.substr(0, splitIndex);
        bodyStr = recvStr.substr(splitIndex + 4);

        int idx = bodyStr.find("\r\n");
        if (idx == std::string::npos)
        {
            responseBodyStr = bodyStr;
        }
        else { //说明是特殊情况，gzip下的先输出长度
            responseBodyStr = bodyStr.substr(idx + 2);
        }
       
    }
    catch (std::exception e) {
        
    }


    //printf("header:\n%s\n", headerStr.c_str());
    //printf("bodyStr:\n%s\n", bodyStr.c_str());

    printf("%s\n", responseBodyStr.c_str());


    closesocket(sock);
    WSACleanup();

    return responseBodyStr;
}
