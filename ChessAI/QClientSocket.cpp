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

bool QClientSocket::initSocket(std::string ip, int port)
{
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	//成功：WSAStartup函数返回零
	if (WSAStartup(w_req, &wsadata) != 0) {
		return false;
	}
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	char* buffer = new char[BUFFER_SIZE];
	if (buffer == NULL)
	{
		printf("内存不足\n");
		return -2;
	}
	memset(buffer, 0, BUFFER_SIZE);

	size_t index = 0;
	while (true)
	{
		size_t len = recv(m_sock, buffer + index, BUFFER_SIZE - index, 0);
		if (len <= 0)
		{
			delete[]buffer;
			return -1;
		}
		index += len;
		len = index;


		//for (size_t i = 0; i < len; i++)
		//{
		//	printf("gg  %d\n", (unsigned char)buffer[i]);
		//}

		m_packet = QPacket((unsigned char*)buffer, len); //解包
		if (len > 0) //解包成功
		{
			memmove(buffer, buffer + len, index - len);
			index -= len;

			delete[] buffer;
			return m_packet.getCmd();
		}
	}

	delete[]buffer;
	return -1;
}

QPacket QClientSocket::getPacket()
{
	return m_packet;
}
