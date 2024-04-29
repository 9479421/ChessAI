#include "QPacket.h"

QPacket::QPacket()
{
}

QPacket::QPacket(uint16_t nCmd, const unsigned char* pData, size_t nSize)
{
	this->sHead = 0xFEFF;
	this->nLength = nSize + 4;
	this->sCmd = nCmd;
	if (nSize > 0)
	{
		this->strData.resize(nSize);
		memcpy((void*)this->strData.c_str(), pData, nSize);
	}
	else {
		this->strData.clear();
	}
	this->sSum = 0;
	for (size_t i = 0; i < this->strData.size(); i++)
	{
		this->sSum += (unsigned char)(this->strData[i]) & 0XFF;
	}
}

QPacket::QPacket(const unsigned char* pData, size_t& nSize)
{
	size_t i = 0;
	for (; i < nSize; i++)
	{
		if (*(uint16_t*)(pData + i) == 0xFEFF)
		{
			sHead = *(uint16_t*)(pData + i);
			i += 2;
			break;
		}
	}
	if (i + 4 + 2 + 2 > nSize)  //没找到包头，或者数据没有完全接收到
	{
		nSize = 0;
		return;
	}
	this->nLength = *(uint32_t*)(pData + i);
	i += 4;
	if (this->nLength + i > nSize)  //数据没有完全接收到
	{
		nSize = 0;
		return;
	}
	this->sCmd = *(uint16_t*)(pData + i);
	i += 2;
	if (this->nLength > 4) //说明有strData内容
	{
		strData.resize(this->nLength - 4);
		memcpy((void*)strData.c_str(), pData + i, this->nLength - 4);
		i += nLength - 4; //加上strData的长度，到和校验的位置
	}
	sSum = *(uint16_t*)(pData + i);
	i += 2;

	uint16_t sum = 0;
	for (size_t i = 0; i < this->strData.size(); i++)
	{
		sum += (unsigned char)(this->strData[i]) & 0xFF;
	}
	if (sum == this->sSum) //这里说明解包成功了
	{
		nSize = i;
		return;
	}
	nSize = 0;
}

const unsigned char* QPacket::Data()
{
	dataPtr = std::shared_ptr<unsigned char>(new unsigned char[nLength + 6], [](unsigned char* p) {delete[]p; });
	unsigned char* pData = dataPtr.get();
	//unsigned char* pData = (unsigned char*)malloc(nLength+6);
	*(uint16_t*)pData = sHead;
	*(uint32_t*)(pData + 2) = nLength;
	*(uint16_t*)(pData + 2 + 4) = sCmd;
	memcpy(pData + 2 + 4 + 2, strData.c_str(), strData.size());
	*(uint16_t*)(pData + 2 + 4 + 2 + strData.size()) = sSum;

	//for (size_t i = 0; i < nLength + 6; i++)
	//{
	//	printf("data:  %d %x\n", pData[i], pData[i]);
	//}
	return pData;
}


int QPacket::Size()
{
	return nLength + 6;
}

uint16_t QPacket::getCmd()
{
	return sCmd;
}

std::string QPacket::getStrData()
{
	return strData;
}
