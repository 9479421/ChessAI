#include "QClientSocket.h"


QClientSocket* QClientSocket::m_instance;

QClientSocket* QClientSocket::getInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new QClientSocket();
	}
    return m_instance;
}

QClientSocket::QClientSocket()
{
	m_buffer.resize(BUFFER_SIZE);
	memset(m_buffer.data(),0,BUFFER_SIZE);
}

QClientSocket::~QClientSocket()
{
	closesocket(m_sock);
	WSACleanup();
}

bool QClientSocket::initSocket(std::string ip, int port)
{
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	//成功：WSAStartup函数返回零
	if (WSAStartup(w_req, &wsadata) != 0) {
		return false;
	}
	m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	m_ip = ip;
	m_port = port;

	return true;
}

bool QClientSocket::ConnectServer()
{
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(m_port);  //服务器端口
	servaddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
	if (connect(m_sock, (sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		return false;
	}
	return true;
}

void QClientSocket::SendCommand(int cmd,unsigned char* data,int size)
{
	QPacket pack(cmd, data, size);
	send(m_sock, (char*)pack.Data(), pack.Size(), 0);
}

int QClientSocket::DealCommand()
{
	char* buffer = m_buffer.data();
	static int index = 0;
	while (true)
	{
		int len = recv(m_sock, buffer + index, BUFFER_SIZE - index, 0);
		if (len <= 0 && index <=0 )
		{
			return -1;
		}
		index += len;
		len = index;

		m_packet = QPacket((unsigned char*)buffer, len); //解包
		if (len > 0) //解包成功
		{
			memmove(buffer, buffer + len, index - len);
			index -= len;
			return m_packet.getCmd();
		}
	}
	return -1;
}


QPacket QClientSocket::getPacket()
{
	return m_packet;
}
