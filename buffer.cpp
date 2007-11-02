/************************************
  REVISION LOG ENTRY
  Revision By: Ozzie
  Revised on 20.1.2002 20:56:03
  Comments: Implemenation of Buffer class
 ************************************/

#include "stdafx.h"
#include "buffer.h"
#ifdef _ISFREEEXE
#include "../../component/openssl/bn.h"
#endif
#include <malloc.h>

/* Initializes the buffer structure. */


// Function name	: Buffer::Buffer
// Description	    : 
// Return type		: 
Buffer::Buffer()
{
	m_alloc = 4096;
	m_buf = (char *)malloc(m_alloc);
	m_offset = 0;
	m_end = 0;
}

/* Frees any memory used for the buffer. */


// Function name	: Buffer::~Buffer
// Description	    : 
// Return type		: 
Buffer::~Buffer()
{
	memset(m_buf, 0, m_alloc);
	free(m_buf);
}

/*
 * Clears any data from the buffer, making it empty.  This does not actually
 * zero the memory.
 */


// Function name	: Buffer::Clear
// Description	    : 
// Return type		: void 
void Buffer::Clear()
{
	m_offset = 0;
	m_end = 0;
}

/* Appends data to the buffer, expanding it if necessary. */


// Function name	: Buffer::Append
// Description	    : 
// Return type		: void 
// Argument         : const char *data
// Argument         : unsigned int len
void Buffer::Append(const char *data)
{
	Append(data, strlen(data));
}
void Buffer::Append(const char *data, unsigned int len)
{
	char *cp;
	AppendSpace(&cp, len+1);
	if (cp)
	{
		memcpy(cp, data, len);
		cp[len]=0;
		m_end--;
	}
}

/*
 * Appends space to the buffer, expanding the buffer if necessary. This does
 * not actually copy the data into the buffer, but instead returns a pointer
 * to the allocated region.
 */


// Function name	: Buffer::AppendSpace
// Description	    : 
// Return type		: void 
// Argument         : char **datap
// Argument         : unsigned int len
void Buffer::AppendSpace(char **datap, unsigned int len)
{
	/* If the buffer is empty, start using it from the beginning. */
	if (m_offset == m_end) {
		m_offset = 0;
		m_end = 0;
	}
restart:
	/* If there is enough space to store all data, store it now. */
	if (m_end + len < m_alloc) {
		*datap = m_buf + m_end;
		m_end += len;
		return;
	}
	/*
	 * If the buffer is quite empty, but all data is at the m_end, move the
	 * data to the beginning and retry.
	 */
	if (m_offset > m_alloc / 2) {
		memmove(m_buf, m_buf + m_offset,
			m_end - m_offset);
		m_end -= m_offset;
		m_offset = 0;
		goto restart;
	}
	/* Increase the size of the buffer and retry. */
	m_alloc += len + 32768;
	m_buf = (char *)realloc(m_buf, m_alloc);
	goto restart;
}

/* Returns the number of bytes of data in the buffer. */


// Function name	: Buffer::Len
// Description	    : 
// Return type		: unsigned int 
unsigned int Buffer::Len()
{
	return m_end - m_offset;
}

/* Gets data from the beginning of the buffer. */


// Function name	: Buffer::Get
// Description	    : 
// Return type		: void 
// Argument         : char *buf
// Argument         : unsigned int len
void Buffer::Get(char *buf, unsigned int len)
{
	if (len > m_end - m_offset)
		return;
//		fatal("buffer_get: trying to get more bytes %d than in buffer %d",len, m_end - m_offset);
	memcpy(buf, m_buf + m_offset, len);
	m_offset += len;
}

/* Consumes the given number of bytes from the beginning of the buffer. */


// Function name	: Buffer::Consume
// Description	    : 
// Return type		: void 
// Argument         : unsigned int bytes
void Buffer::Consume(unsigned int bytes)
{
	if (bytes > m_end - m_offset)
		return;
//		fatal("buffer_consume: trying to get more bytes than in buffer");
	m_offset += bytes;
}

/* Consumes the given number of bytes from the m_end of the buffer. */


// Function name	: Buffer::ConsumeEnd
// Description	    : 
// Return type		: void 
// Argument         : unsigned int bytes
void Buffer::ConsumeEnd(unsigned int bytes)
{
	if (bytes > m_end - m_offset)
		return;
//		fatal("buffer_consume_end: trying to get more bytes than in buffer");
	m_end -= bytes;
}

/* Returns a pointer to the first used byte in the buffer. */


// Function name	: *Buffer::Ptr
// Description	    : 
// Return type		: char 
char *Buffer::Ptr()
{
	return m_buf + m_offset;
}

void Buffer::PutInt(unsigned int value)
{
	char buf[4];
	PUT_32BIT(buf, value);
	Append(buf, 4);
}


#ifndef _CMDTERM
/*
 * Stores an BIGNUM in the buffer with a 2-byte msb first bit count, followed
 * by (bits+7)/8 bytes of binary data, msb first.
 */

// Function name	: Buffer::PutBignum
// Description	    : 
// Return type		: void 
// Argument         : BIGNUM *value
#ifndef _ISFREEOPEN
void Buffer::PutBignum(BIGNUM *value)
{
	int bits = BN_num_bits(value);
	int bin_size = (bits + 7) / 8;
	unsigned char *buf = (unsigned char *)malloc(bin_size);
	int oi;
	char msg[2];

	/* Get the value of in binary */
	oi = BN_bn2bin(value, buf);
	if (oi != bin_size)
		return;
//		fatal("buffer_put_bignum: BN_bn2bin() failed: oi %d != bin_size %d",
//		      oi, bin_size);

	/* Store the number of bits in the buffer in two bytes, msb first. */
	PUT_16BIT(msg, bits);
	Append(msg, 2);
	/* Store the binary data. */
	Append((char *)buf, oi);

	memset(buf, 0, bin_size);
	free(buf);
}

/*
 * Retrieves an BIGNUM from the buffer.
 */

// Function name	: Buffer::GetBignum
// Description	    : 
// Return type		: int 
// Argument         : BIGNUM *value
int Buffer::GetBignum(BIGNUM *value)
{
	int bits, bytes;
	unsigned char buf[2], *bin;

	/* Get the number for bits. */
	Get((char *) buf, 2);
	bits = GET_16BIT(buf);
	/* Compute the number of binary bytes that follow. */
	bytes = (bits + 7) / 8;
	if (Len() < (unsigned)bytes)
		return 0;
//		fatal("buffer_get_bignum: input buffer too small");
	bin = (unsigned char *) Ptr();
	BN_bin2bn(bin, bytes, value);
	Consume(bytes);

	return 2 + bytes;
}

/*
 * Stores an BIGNUM in the buffer in SSH2 format.
 */

// Function name	: Buffer::PutBignum2
// Description	    : 
// Return type		: void 
// Argument         : BIGNUM *value
void Buffer::PutBignum2(BIGNUM *value)
{
	int bytes = BN_num_bytes(value) + 1;
	unsigned char *buf = (unsigned char *)malloc(bytes);
	int oi;
	int hasnohigh = 0;
	buf[0] = '\0';
	/* Get the value of in binary */
	oi = BN_bn2bin(value, buf+1);
	if (oi != bytes-1)
		return;
//		fatal("buffer_put_bignum: BN_bn2bin() failed: oi %d != bin_size %d",
//		      oi, bytes);
	if (oi<1)
		return;
	hasnohigh = (buf[1] & 0x80) ? 0 : 1;
	if (value->neg) {
		/**XXX should be two's-complement */
		int i, carry;
		unsigned char *uc = buf;
//		log("negativ!");
		for(i = bytes-1, carry = 1; i>=0; i--) {
			uc[i] ^= 0xff;
			if(carry)
				carry = !++uc[i];
		}
	}
	PutString(buf+hasnohigh, bytes-hasnohigh);
	memset(buf, 0, bytes);
	free(buf);
}


// Function name	: Buffer::GetBignum2
// Description	    : 
// Return type		: int 
// Argument         : BIGNUM *value
int Buffer::GetBignum2(BIGNUM *value)
{
	/**XXX should be two's-complement */
	int len;
	unsigned char *bin = (unsigned char *)GetString((unsigned int *)&len);
	BN_bin2bn(bin, len, value);
	free(bin);
	return len;
}

#endif
/*
 * Returns an integer from the buffer (4 bytes, msb first).
 */

// Function name	: Buffer::GetInt
// Description	    : 
// Return type		: unsigned int 
unsigned int Buffer::GetInt()
{
	unsigned char buf[4];
	Get((char *) buf, 4);
	return GET_32BIT(buf);
}
#endif
/*
 * Stores an integer in the buffer in 4 bytes, msb first.
 */

// Function name	: Buffer::PutInt
// Description	    : 
// Return type		: void 
// Argument         : unsigned int value

/*
 * Returns an arbitrary binary string from the buffer.  The string cannot
 * be longer than 256k.  The returned value points to memory allocated
 * with xmalloc; it is the responsibility of the calling function to free
 * the data.  If length_ptr is non-NULL, the length of the returned data
 * will be stored there.  A null character will be automatically appended
 * to the returned string, and is not counted in length.
 */

// Function name	: *Buffer::GetString
// Description	    : 
// Return type		: char 
// Argument         : unsigned int *length_ptr
char *Buffer::GetString(unsigned int *length_ptr)
{
	unsigned int len;
	char *value;
	/* Get the length. */
	len = GetInt();
	if (len > 256 * 1024)
		return NULL;
//		fatal("Received packet with bad string length %d", len);
	/* Allocate space for the string.  Add one byte for a null character. */
	value = (char *)malloc(len + 1);
	/* Get the string. */
	Get(value, len);
	/* Append a null character to make processing easier. */
	value[len] = 0;
	/* Optionally return the length of the string. */
	if (length_ptr)
		*length_ptr = len;
	return value;
}

/*
 * Stores and arbitrary binary string in the buffer.
 */

// Function name	: Buffer::PutString
// Description	    : 
// Return type		: void 
// Argument         : const void *buf
// Argument         : unsigned int len
void Buffer::PutString(const void *buf, unsigned int len)
{
	PutInt(len);
	Append((char *)buf, len);
}

// Function name	: Buffer::PutCString
// Description	    : 
// Return type		: void 
// Argument         : const char *s
void Buffer::PutCString(const char *s)
{
	PutString(s, strlen(s));
}

/*
 * Returns a character from the buffer (0 - 255).
 */

// Function name	: Buffer::GetChar
// Description	    : 
// Return type		: int 
int Buffer::GetChar()
{
	char ch;
	Get(&ch, 1);
	return (unsigned char) ch;
}

/*
 * Stores a character in the buffer.
 */

// Function name	: Buffer::PutChar
// Description	    : 
// Return type		: void 
// Argument         : int value
void Buffer::PutChar(int value)
{
	char ch = value;
	Append(&ch, 1);
}



char *Buffer::GetNextLine()
{
	int i,len;

	len = Len();
	char *mark = Ptr();

	if (len)
	{
		i=0;
		char *b = Ptr();
		while (i<len && b[i]!='\n') i++;
		if (i<len && b[i]=='\n')
		{
			// consume i+1
			Consume(i+1);

			if (i>0 && b[i-1]=='\r')
				i--;				
			
			b[i]=0;
			return mark;
		}
		else
			return NULL;
	}
	else
		return NULL;
}