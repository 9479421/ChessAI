#pragma once

#include <cstdint>
#include<string>
#include<string.h>
#include<memory>

#pragma pack(push)
#pragma pack(1) //�ڴ�1�ֽڶ���
class QPacket
{
public:
	QPacket();
	QPacket(uint16_t nCmd, const unsigned char* pData, size_t nSize); //���
	QPacket(const unsigned char* pData, size_t& nSize); //���
	const unsigned char* Data();

	int Size();
	uint16_t getCmd();
	std::string getStrData();
private:
	uint16_t sHead; //�̶�λFE FF
	uint32_t nLength; //����İ���
	uint16_t sCmd; //����
	std::string strData;
	uint16_t sSum; //��У��
public:
	//����ָ��
	std::shared_ptr<unsigned char> dataPtr;
};
#pragma pack(pop)
