#include "Endianness.hpp"
#include "EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
void ToEndianness(size_t const size, void * buffer, eEndianness endianness)
{
	if(GetPlatformEndianness() == endianness)
	{
		return;
	}

	Byte* byte_buffer = (Byte*) buffer;

	uint i = 0;
	uint j = size - 1U;

	while(i<j)
	{
		std::swap(byte_buffer[i], byte_buffer[j]);
		++i;
		--j;
	}

	byte_buffer[i] == *(byte_buffer + i);
}

//-----------------------------------------------------------------------------------------------
void FromEndianness(size_t const size, void * buffer, eEndianness endianness)
{
	if(GetPlatformEndianness() == endianness)
	{
		return;
	}

	Byte* byte_buffer = (Byte*) buffer;

	uint i = 0;
	uint j = size - 1U;

	while(i<j)
	{
		std::swap(byte_buffer[i], byte_buffer[j]);
		++i;
		--j;
	}

	byte_buffer[i] == *(byte_buffer + i);
}

//-----------------------------------------------------------------------------------------------
eEndianness GetPlatformEndianness()
{
	uint const num = 1;
	Byte const* array = (Byte const*) &num;
	
	if(array[0] == 0x01)
	{
		return LITTLE_ENDIAN;
	}
	else
	{
		return BIG_ENDIAN;
	}
}
