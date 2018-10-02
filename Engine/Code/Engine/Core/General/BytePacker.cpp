#include "BytePacker.hpp"
#include "EngineCommon.hpp"
#include "..\..\Math\MathUtils.hpp"


//===============================================================================================
BytePacker::BytePacker(eEndianness byte_order)
{
	m_endianness = byte_order;
	m_mode = BYTEPACKER_OWNS_MEMORY | BYTEPACKER_CAN_GROW;
	
	// default size to 8kb and grow by size * 8kb
	m_buffer = malloc(8 * KILOBYTE);
	m_bufferSize = 8 * KILOBYTE;

	m_readableHead = 0U;
	m_writableHead = 0U;
}

//-----------------------------------------------------------------------------------------------
BytePacker::BytePacker(size_t buffer_size, eEndianness byte_order)
{
	m_endianness = byte_order;
	m_mode = BYTEPACKER_OWNS_MEMORY;

	m_buffer = malloc(buffer_size);
	m_bufferSize = buffer_size;

	m_readableHead = 0U;
	m_writableHead = 0U;
}

//-----------------------------------------------------------------------------------------------
BytePacker::BytePacker(size_t buffer_size, void * buffer, eEndianness byte_order)
{
	m_endianness = byte_order;
	m_bufferSize = buffer_size;
	m_buffer = buffer;

	m_readableHead = 0U;
	m_writableHead = 0U;

	// mode is defaulted to 0 in the hpp
	//m_mode  |= ((m_mode & ~BYTEPACKER_OWNS_MEMORY) | (m_mode & ~BYTEPACKER_CAN_GROW));
}

//-----------------------------------------------------------------------------------------------
BytePacker::~BytePacker()
{
	if(m_mode == BYTEPACKER_OWNS_MEMORY)
	{
		free(m_buffer);
	}

	// sanity
	m_buffer = nullptr;
	m_bufferSize = 0U;
}

//-----------------------------------------------------------------------------------------------
void BytePacker::SetEndianness(eEndianness type)
{
	m_endianness = type;
	ToEndianness(sizeof(m_buffer), m_buffer, m_endianness);
}

//-----------------------------------------------------------------------------------------------
bool BytePacker::SetTotalWrittenByteCount(size_t byte_count)
{
	if(byte_count < m_bufferSize)
	{
		m_writableHead = byte_count;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool BytePacker::WriteBytes(size_t byte_count, void const * data, bool changeEndianness)
{

	// make a copy of the data so that we can flip it
	void* newData = malloc(byte_count);
	memcpy(newData, data, byte_count);

	if(changeEndianness)
		ToEndianness(byte_count, newData, m_endianness);

	if((byte_count + m_writableHead) > m_bufferSize)
	{
		// if we grow, grow and add
		if(AreBitsSet(m_mode, BYTEPACKER_CAN_GROW))
		{
			size_t originalBufferSize = m_bufferSize;
			m_bufferSize *= SIZE_TO_GROW;
			
			void* newBuffer = malloc(m_bufferSize);
			memcpy(newBuffer, m_buffer, originalBufferSize);
			free(m_buffer);

			m_buffer = newBuffer;

		}
		else
		{
			return false;
		}
	}

	// just add to the buffer now that it is the proper size
	memcpy(((Byte*) m_buffer) + m_writableHead, newData, byte_count);
	m_writableHead += byte_count;
	
	free(newData);
	return true;
}

//-----------------------------------------------------------------------------------------------
bool BytePacker::WriteRawBytes(size_t byte_count, void const *data)
{
	return WriteBytes(byte_count, data, false);
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::ReadBytes(void * out_data, size_t max_byte_count, bool advanceReadHead, bool checkEndianess)
{
	// make sure we aren't gonna go out of bounds into garbage data
	if((m_readableHead + max_byte_count) < m_writableHead)
	{
		memcpy(out_data, ((Byte*)m_buffer) + m_readableHead, max_byte_count);

		if(checkEndianess)
			FromEndianness(max_byte_count, out_data, m_endianness);
		
		if(advanceReadHead)
			m_readableHead += max_byte_count;
		
		return max_byte_count;
	}
	else
	{
		// this is we can't freely read so we can only increase by v
		memcpy(out_data, (Byte*)m_buffer + m_readableHead, (m_writableHead - m_readableHead));
		
		if(checkEndianess)
			FromEndianness((m_writableHead - m_readableHead), out_data, m_endianness);
		
		size_t amountActuallyRead = m_writableHead - m_readableHead;

		if(advanceReadHead)
			m_readableHead = m_writableHead;
		
		return amountActuallyRead;
	}
	
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::Peek( void * out_data, size_t length)
{
	return ReadBytes( out_data, length, false );
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::WriteSize(size_t size)
{
	size_t index = size;
	size_t amountCreated = 0U;

	// loop through by cutting 7 bytes off until we have no mo 
	while((index >> 7 ) != 0)
	{
		// get the bytes to add
		Byte whatToWrite = (index & LEAST_SIGNIFICANT_BITS);
		whatToWrite |= 0b1000'0000; // put a 1 at the front

		// add to buffer
		WriteBytes(1, &whatToWrite);

		// move down the bit
		index >>= 7;

		// Add to our count so we know how many times we did the thing
		amountCreated++;
	}

	// we didn't add the last bits to it yet, so do that pls
	Byte whatToWrite = (index & LEAST_SIGNIFICANT_BITS);
	whatToWrite |= 0b0000'0000; // put a 1 at the front

	// add to buffer
	WriteBytes(1, &whatToWrite);

	// return 
	return amountCreated + 1;
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::ReadSize(size_t * out_size)
{
	size_t numberOfBytes = 0U;
	bool isReading = true;
	size_t theSize = 0;

	while( isReading )
	{
		// get bytes from buffer
		Byte currentByte;
		ReadBytes(&currentByte, 1);

		// Get the seven significant
		size_t significant = (currentByte & LEAST_SIGNIFICANT_BITS);

		// format it so we can now add it
		significant <<= (numberOfBytes * 7);

		// add to out size
		theSize += significant;

		// see if we are done
		isReading = ((currentByte & 0b1000'0000) != 0);
		
		// increase number
		numberOfBytes++;
	}
	
	*out_size = theSize;
	return numberOfBytes;
}

//-----------------------------------------------------------------------------------------------
bool BytePacker::WriteString(char const * str)
{
	
	String theString = str;
	bool check =  WriteSize(theString.size());
	WriteRawBytes(theString.size(), theString.c_str());
	
	return check;
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::ReadString(char * out_str, size_t max_byte_size)
{
	
	size_t size;
	ReadSize(&size);

	if(size > max_byte_size)
	{
		ReadBytes(out_str, max_byte_size, true, false);
		out_str[max_byte_size] = '\0';
	}
	else
	{
		ReadBytes(out_str, size, true, false);
		out_str[size] = '\0';
	}
	
	return size;
}

//-----------------------------------------------------------------------------------------------
void BytePacker::ResetWrite()
{
	m_writableHead = 0U;
	m_readableHead = 0U;
}

//-----------------------------------------------------------------------------------------------
size_t BytePacker::GetWritableByteCount() const
{
	if(AreBitsSet(m_mode, BYTEPACKER_CAN_GROW))
		return (size_t) UINFINITY;
	
	return (m_bufferSize - m_writableHead);
}

