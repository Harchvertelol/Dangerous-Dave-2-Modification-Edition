#include "SPacket.h"
#include "zlib.h"
#include "CEncryption.h"

uchar CEncryption::expKey[4 * Nb * (Nr + 1)] = {};

namespace irr
{
namespace net
{

SOutPacket::SOutPacket() {}

SOutPacket::SOutPacket(const c8* buff,const u32 buff_size)
{
	this->buff.set_used(buff_size);
	memcpy(this->buff.pointer(),buff,buff_size);
}

void SOutPacket::clearData()
{
	buff.clear();
}

c8* SOutPacket::getData()
{
	return (c8*)buff.pointer();
}

const c8* SOutPacket::getConstData()
{
	return (const c8*)buff.const_pointer();
}

u32 SOutPacket::getSize()
{
	return buff.size();
}

void SOutPacket::enlargeBuffer(const u32 size)
{
	const u32 newSize = buff.size() + size;
	if(buff.allocated_size() < newSize)
		buff.reallocate(newSize * 2);

	buff.set_used(newSize);
}

SOutPacket& SOutPacket::operator << (const c8 data)
{
	buff.push_back(data);
	return *this;
}

SOutPacket& SOutPacket::operator << (const u16 data)
{
	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &data, 2);
	return *this;
}

SOutPacket& SOutPacket::operator << (const u32 data)
{
	enlargeBuffer(4);
	memcpy(buff.pointer() + buff.size() - 4, &data, 4);
	return *this;
}

SOutPacket& SOutPacket::operator << (const u8 data)
{
	buff.push_back(data);
	return *this;
}

SOutPacket& SOutPacket::operator << (const s16 data)
{
	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &data, 2);
	return *this;
}

SOutPacket& SOutPacket::operator << (const s32 data)
{
	enlargeBuffer(4);
	memcpy(buff.pointer() + buff.size() - 4, &data, 4);
	return *this;
}

SOutPacket& SOutPacket::operator << (const f32 data)
{
	enlargeBuffer(4);
	memcpy(buff.pointer() + buff.size() - 4, &data, 4);
	return *this;
}

SOutPacket& SOutPacket::operator << (const f64 data)
{
	enlargeBuffer(8);
	memcpy(buff.pointer() + buff.size() - 8, &data, 8);
	return *this;
}

SOutPacket& SOutPacket::operator << (const core::vector3df& data)
{
	enlargeBuffer(12);
	memcpy(buff.pointer() + buff.size() - 12, &data.X, 12);
	return *this;
}

SOutPacket& SOutPacket::operator << (const c8* string)
{
	u16 tmp = (u16)strlen(string);

	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &tmp, 2);
	
	enlargeBuffer(tmp);
	memcpy(buff.pointer() + buff.size() - tmp, string, tmp);
	return *this;
}

SOutPacket& SOutPacket::operator << (const core::stringc& string)
{
	u16 tmp = string.size();

	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &tmp, 2);
	
	enlargeBuffer(tmp);
	memcpy(buff.pointer() + buff.size() - tmp, &string[0], tmp);
	return *this;
}

SOutPacket& SOutPacket::operator << (const std::string& string)
{
	u16 tmp = (u16)string.size();

	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &tmp, 2);
	
	enlargeBuffer(tmp);
	memcpy(buff.pointer() + buff.size() - tmp, &string[0], tmp);
	return *this;
}

SOutPacket& SOutPacket::operator << (const core::stringw& string)
{
	const u16 tmp = string.size();

	enlargeBuffer(2);
	memcpy(buff.pointer() + buff.size() - 2, &tmp, 2);

	enlargeBuffer(1);
	const u8 wCharSize = sizeof(wchar_t);
	memcpy(buff.pointer() + buff.size() - 1, &wCharSize, 1);

	const u32 wcharByteSize = (tmp * sizeof(wchar_t));
	enlargeBuffer(wcharByteSize);
	memcpy(buff.pointer() + buff.size() - wcharByteSize, &string[0], wcharByteSize);
	return *this;
}

SOutPacket& SOutPacket::operator << (const SOutPacket& other)
{
	enlargeBuffer(other.buff.size());
	memcpy(buff.pointer() + buff.size() - other.buff.size(), other.buff.const_pointer(), other.buff.size());

	return *this;
}

void SOutPacket::compressPacket()
{
	const u32 compBound = compressBound(buff.size()) + 4;
	core::array<c8> newBuff(compBound);
	
	const u32 currSize = buff.size();
	memcpy(newBuff.pointer(), &currSize, 4);

	uLongf destLen = compBound;
	compress((Bytef*)newBuff.pointer() + 4, &destLen, (Bytef*)buff.pointer(), buff.size());
	newBuff.set_used(destLen + 4);

	buff = newBuff;
}

void SOutPacket::deCompressPacket()
{
	u32 newSize = 0;
	memcpy((void*)&newSize, buff.pointer(), 4);

	core::array<c8> newBuff;
	newBuff.set_used(newSize);

	uLongf destLen = newSize;
	uncompress((Bytef*)newBuff.pointer(), &destLen, (Bytef*)buff.pointer() + 4, buff.size() - 4);
	newBuff.set_used(destLen);

	buff = newBuff;
}

void SOutPacket::encryptPacket(const c8 key[16])
{
	CEncryption::SetEncryptionKey((u8*)&key[0]);
	core::array<c8> tmpbuff;
	const u32 newSize = buff.size() + (16 - (buff.size() % 16));
	tmpbuff.set_used(newSize);
	buff.set_used(newSize);
	
	for(u32 i = 0;i < newSize;i+=16)
		CEncryption::Encrypt((u8*)buff.pointer() + i, (u8*)tmpbuff.pointer() + i);

	buff = tmpbuff;
}

void SOutPacket::decryptPacket(const c8 key[16])
{
	CEncryption::SetEncryptionKey((u8*)&key[0]);
	const u32 newSize = buff.size();
	core::array<c8> tmpbuff;
	tmpbuff.set_used(newSize);
	
	for(u32 i = 0;i < newSize;i+=16)
		CEncryption::Decrypt((u8*)buff.pointer() + i, (u8*)tmpbuff.pointer() + i);

	buff = tmpbuff;
}

SInPacket::SInPacket(const c8* buff, const u32 size) : pos(0), playerid(0), valid(true)
{
	SInPacket::buff.set_used(size);
	memcpy(SInPacket::buff.pointer(), buff, size);
}


void SInPacket::setNewData(const c8* buffer, const u32 size)
{
	buff.set_used(size);
	memcpy(buff.pointer(), buffer, size);
	pos = 0;
}

void SInPacket::setPlayerId(u16 playernumber)
{
	playerid = playernumber;
}

void SInPacket::operator >> (c8 &data)
{
	memcpy(&data,getData()+pos,1);
	pos++;
}

void SInPacket::operator >> (f32 &data)
{
	memcpy(&data,getData()+pos,4);
	pos+=4;
}

void SInPacket::operator >> (f64 &data)
{
	memcpy(&data,getData()+pos,8);
	pos+=8;
}

void SInPacket::operator >> (u16 &data)
{
	memcpy(&data,getData()+pos,2);
	pos+=2;
}

void SInPacket::operator >> (u32 &data)
{
	memcpy(&data,getData()+pos,4);
	pos+=4;
}

void SInPacket::operator >> (u8 &data)
{
	memcpy(&data,getData()+pos,1);
	pos++;
}

void SInPacket::operator >> (s16 &data)
{
	memcpy(&data,getData()+pos,2);
	pos+=2;
}

void SInPacket::operator >> (s32 &data)
{
	memcpy(&data,getData()+pos,4);
	pos+=4;
}

void SInPacket::operator >> (core::vector3df &data)
{
	memcpy(&data.X,getData()+pos,12);
	pos+=12;
}

void SInPacket::operator >> (char *string)
{
	u16 sz;
	memcpy(&sz,getData()+pos,2);
	pos+=2;

	memcpy(string,getData()+pos,sz);
	pos += sz;

	string[sz] = '\0';
}

void SInPacket::operator >> (core::stringc& string)
{
	u16 sz;
	memcpy(&sz, getData() + pos, 2);
	pos+=2;

	char* tempBuff = new char[sz + 1];
	memcpy(tempBuff, getData() + pos, sz);
	tempBuff[sz] = '\0';
	string = tempBuff;

	delete [] tempBuff;
	
	pos += sz;
}

void SInPacket::operator >> (std::string& string)
{
	u16 sz;
	memcpy(&sz, getData() + pos, 2);
	pos+=2;

	char* tempBuff = new char[sz + 1];
	memcpy(tempBuff, getData() + pos, sz);
	tempBuff[sz] = '\0';
	string = tempBuff;

	delete [] tempBuff;
	
	pos += sz;
}

void SInPacket::operator >> (core::stringw& string)
{
	u16 sz;
	memcpy(&sz,getData()+pos,2);
	pos+=2;

	u8 wCharSize;
	memcpy(&wCharSize,getData()+pos,1);
	pos+=1;

	wchar_t* tempBuff = new wchar_t[sz + 1];
	const u32 wcharByteSize = (sz * sizeof(wchar_t));

	if(wCharSize != sizeof(wchar_t))
	{
		if(wCharSize > sizeof(wchar_t))
		{
			for(u32 i = 0;i < sz;i++)
			{
				tempBuff[i] = 0;
				memcpy(&tempBuff[i],getData()+pos,sizeof(wchar_t));

				pos += wCharSize;
			}
		}
		else
		{
			for(u32 i = 0;i < sz;i++)
			{
				tempBuff[i] = 0;
				memcpy(&tempBuff[i],getData()+pos,wCharSize);

				pos += wCharSize;
			}
		}
	}
	else
	{
		memcpy(tempBuff, getData() + pos, wcharByteSize);
	}

	tempBuff[sz] = '\0';
	string = tempBuff;

	delete [] tempBuff;
	
	pos += wcharByteSize;
}

u16 SInPacket::getPlayerId() const
{
	return playerid;
}

void SInPacket::resetPos()
{
	pos = 0;
}

c8* SInPacket::getData()
{
	return (c8*)buff.pointer();
}

const c8* SInPacket::getConstData()
{
	return (const c8*)buff.const_pointer();
}

void SInPacket::compressPacket()
{
	const u32 compBound = compressBound(buff.size()) + 4;
	core::array<c8> newBuff(compBound);

	const u32 currSize = buff.size();
	memcpy(newBuff.pointer(), &currSize, 4);

	uLongf destLen = compBound;
	compress((Bytef*)newBuff.pointer() + 4, &destLen, (Bytef*)buff.pointer(), buff.size());
	newBuff.set_used(destLen + 4);

	buff = newBuff;
}

void SInPacket::deCompressPacket()
{
	u32 newSize = 0;
	memcpy((void*)&newSize, buff.pointer(), 4);

	core::array<c8> newBuff;
	newBuff.set_used(newSize);

	uLongf destLen = newSize;
	int ret = uncompress((Bytef*)newBuff.pointer(), &destLen, (Bytef*)buff.pointer() + 4, buff.size() - 4);
	
	if (ret != Z_OK)
	{
		valid = false;
		newBuff.set_used(0);
		newBuff.clear();
	}
	else
	{
		newBuff.set_used(destLen);
		buff = newBuff;
	}
}

void SInPacket::encryptPacket(const c8 key[16])
{
	CEncryption::SetEncryptionKey((u8*)&key[0]);
	core::array<c8> tmpbuff;
	const u32 newSize = buff.size() + (16 - (buff.size() % 16));
	tmpbuff.set_used(newSize);
	buff.set_used(newSize);
	
	for(u32 i = 0;i < newSize;i+=16)
		CEncryption::Encrypt((u8*)buff.pointer() + i, (u8*)tmpbuff.pointer() + i);

	buff = tmpbuff;
}

void SInPacket::decryptPacket(const c8 key[16])
{
	if (buff.size() % 16 != 0)
	{
		valid = false;
		return;
	}

	CEncryption::SetEncryptionKey((u8*)&key[0]);
	const u32 newSize = buff.size();
	core::array<c8> tmpbuff;
	tmpbuff.set_used(newSize);
	
	for(u32 i = 0;i < newSize;i+=16)
		CEncryption::Decrypt((u8*)buff.pointer() + i, (u8*)tmpbuff.pointer() + i);

	buff = tmpbuff;
}

u32 SInPacket::getSize()
{
	return buff.size();
}

bool SInPacket::isValid()
{
	return valid;
}

} // Close Net Namespace
} // Close Irr namespace