#pragma once

#include <cstdint>
#include<string>
#include<string.h>
#include<memory>

#pragma pack(push)
#pragma pack(1) //内存1字节对齐
class QPacket
{
public:
	QPacket();
	QPacket(uint16_t nCmd, const unsigned char* pData, size_t nSize); //组包
	QPacket(const unsigned char* pData, size_t& nSize); //解包
	const unsigned char* Data();

	int Size();
	uint16_t getCmd();
	std::string getStrData();
private:
	uint16_t sHead; //固定位FE FF
	uint32_t nLength; //下面的包长
	uint16_t sCmd; //命令
	std::string strData;
	uint16_t sSum; //和校验
public:
	//智能指针
	std::shared_ptr<unsigned char> dataPtr;
};
#pragma pack(pop)
