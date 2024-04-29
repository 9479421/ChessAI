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
	void SendCommand(int cmd, unsigned char* data, int size); //�����������Packet
	int DealCommand(); //������յ�����Ϣ

	QPacket getPacket();
private:
	SOCKET m_sock;
	QPacket m_packet;

	std::string m_ip;
	int m_port;
};

