#pragma once
#include "EngineCommon.hpp"
#include "Engine/Core/General/Endianness.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
#define BIT_FLAG(f)   (1U << (f))

#define SIZE_TO_GROW (2)

#define LEAST_SIGNIFICANT_BITS (0b0111'1111)

//====================================================================================
// ENUMS
//====================================================================================
enum eBytePackerOptionBit : uint
{
	// a byte packer allocated its own memory and 
	// should free it when destroyed; 
	BYTEPACKER_OWNS_MEMORY    = BIT_FLAG(0),

	// If the byte packer runs out of space, will
	// allocate more memory (maintaining the data that was stored)
	// Must have flag BYTEPACKER_OWNS_MEMORY
	BYTEPACKER_CAN_GROW       = BIT_FLAG(1)
};
typedef uint eBytePackerOptions; 

// Variable types
// default to 0U;
// = BYTEPACKER_OWNS_MEMORY is Owns memory, can't grow
// = BYTEPACKER_CAN_GROW is doesn't own, can grow
// = BYTE_PACKER_CAN_GROW | BYTEPACKER_OWNS_MEMORY is owns and can grow

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class BytePacker 
{
public:     
	BytePacker( eEndianness byte_order = LITTLE_ENDIAN ); // Can grow, I own it
	BytePacker( size_t buffer_size, eEndianness byte_order = LITTLE_ENDIAN ); // can't grow, and I own it
	BytePacker( size_t buffer_size, void *buffer, eEndianness byte_order = LITTLE_ENDIAN ); // can't grow, and don't own
	~BytePacker(); 

	// sets how much of the buffer is readable; 
	void SetEndianness( eEndianness type );
	bool SetTotalWrittenByteCount( size_t byte_count );

	// tries to write data to the end of the buffer;  Returns success
	bool WriteBytes( size_t byte_count, void const *data ); 
	
	// Tries to read into out_data.  Returns how much
	// ended up being read; 
	size_t ReadBytes( void *out_data, size_t max_byte_count ); 

	size_t WriteSize( size_t size ); // returns how many bytes used
	size_t ReadSize( size_t *out_size ); // returns how many bytes read, fills out_size

	// See notes on encoding!
	bool WriteString( char const *str ); 
	size_t ReadString( char *out_str, size_t max_byte_size ); // max_str_size should be enough to contain the null terminator as well; 

	// HELPERS
	// suggested method names for commonly needed information; 
	void ResetWrite();  // resets writing to the beginning of the buffer.  Make sure read head stays valid (<= write_head)
	void ResetRead() { m_readableHead = 0U; }   // resets reading to the beginning of the buffer

	eEndianness GetEndianness() const { return m_endianness; } 
	size_t GetWrittenByteCount() const { return m_writableHead; }   // how much have I written to this buffer
	size_t GetWritableByteCount() const { return (m_bufferSize - m_writableHead); }   // how much more can I write to this buffer (if growble, this returns UINFINITY)
	size_t GetReadableByteCount() const { return (m_writableHead - m_readableHead); }   // how much more data can I read;


private:

	eEndianness					m_endianness;
	eBytePackerOptions			m_mode = 0U;
	
	size_t						m_writableHead;
	size_t						m_readableHead;
	
	size_t						m_bufferSize;
	void*						m_buffer;

}; 

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [8/30/2018]
//====================================================================================