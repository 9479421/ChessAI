#pragma once

#include<winsock2.h>
#include"QPacket.h"
#define BUFFER_SIZE 409600
class QClientSocket
{
public:
	static QClientSocket* getInstance();
	static QClientSocket* m_instance;
public:
	bool initSocket(std::string ip, int port);
	bool ConnectServer();
	void SendCommand(int cmd, unsigned char* data, int size); //向服务器发送Packet
	int DealCommand(); //处理接收到的消息

	QPacket getPacket();
private:
	SOCKET m_sock;
	QPacket m_packet;

	std::string m_ip;
	int m_port;
};

