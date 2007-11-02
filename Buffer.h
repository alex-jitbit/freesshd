#ifndef __BUFFER_CPP
#define __BUFFER_CPP

#ifdef _ISFREEEXE
#include "../../component/getputdefines.h"
#include "../../component/openssl/bn.h"
#endif 

#ifdef _ISFREEOPEN

#define GET_32BIT(cp) (((u_long)(u_char)(cp)[0] << 24) | \
		       ((u_long)(u_char)(cp)[1] << 16) | \
		       ((u_long)(u_char)(cp)[2] << 8) | \
		       ((u_long)(u_char)(cp)[3]))

#define PUT_32BIT(cp, value) do { \
  (cp)[0] = (value) >> 24; \
  (cp)[1] = (value) >> 16; \
  (cp)[2] = (value) >> 8; \
  (cp)[3] = (value); } while (0)

#endif 

class Buffer
{


public:
		char	*m_buf;		/* Buffer for data. */
		unsigned int	 m_alloc;		/* Number of bytes allocated for data. */
public:
		unsigned int	 m_offset;	/* Offset of first byte containing data. */
		unsigned int	 m_end;		/* Offset of last byte containing data. */

public:
	Buffer();
	~Buffer();
	void	 Clear();

	unsigned int	 Len();
	char	*Ptr();

	void	 Append(const char *, unsigned int);
	void	 Append(const char *);
	void	 AppendSpace(char **, unsigned int);

	void	 Get(char *, unsigned int);

	void	 Consume(unsigned int);
	void	 ConsumeEnd(unsigned int);

	void PutInt(unsigned int);

	
#ifndef _CMDTERM
#ifndef _ISFREEOPEN
	void PutBignum(BIGNUM *);
	void PutBignum2(BIGNUM *);
	int GetBignum(BIGNUM *);
	int GetBignum2(BIGNUM *);
#endif

	unsigned int GetInt();

	int GetChar();
	void PutChar(int);

	void PutString(const void *, unsigned int);

	char *GetString(unsigned int *);
	void PutCString(const char *);
#endif

	char *GetNextLine(void);

};


#endif
