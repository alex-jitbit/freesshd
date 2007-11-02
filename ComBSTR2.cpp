/*********************************************************
  * Copyright (C) 2001-2002 Kresimir Petric (kreso@weonlydo.com)
  * All rights reserved.
  *
  * This code is property of WeOnlyDo! Inc.
  * You have no right to change or use this code without written
  * permission of WeOnlyDo! Software.
  *
  * Any changes to the code must be requested from general@weonlydo.com 
  *
  * Last revised on 24.10.2002 17:50:32
 *********************************************************/

#include "stdafx.h"
#include "ComBSTR2.h"


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
CComBSTR2::CComBSTR2() : CComBSTR()
{
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
// Argument         : int nSize
CComBSTR2::CComBSTR2(int nSize) : CComBSTR(nSize) 
{
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
// Argument         : int nSize
// Argument         : LPCOLESTR sz ) : CComBSTR(nSize
// Argument         : sz
CComBSTR2::CComBSTR2(int nSize, LPCOLESTR sz ) : CComBSTR(nSize, sz)
{
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
// Argument         : LPCOLESTR pSrc
CComBSTR2::CComBSTR2(LPCOLESTR pSrc ) : CComBSTR (pSrc)
{
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
// Argument         : const CComBSTR& src
CComBSTR2::CComBSTR2(const CComBSTR& src ) : CComBSTR (src)
{
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::CComBSTR2
// Description	    : 
// Return type		: 
// Argument         : REFGUID src
CComBSTR2::CComBSTR2(REFGUID src) : CComBSTR (src)
{
	m_Buffer = NULL;
}

// Function name	: CComBSTR2::~CComBSTR2
// Description	    : 
// Return type		: 
CComBSTR2::CComBSTR2(LPCSTR src) 
{
	USES_CONVERSION;
	BSTR s = T2BSTR(src);
	this->m_str = s;
//	::SysFreeString(s);
	m_Buffer = NULL;
}


// Function name	: CComBSTR2::~CComBSTR2
// Description	    : 
// Return type		: 
CComBSTR2::~CComBSTR2() 
{
	if (m_Buffer)
		free(m_Buffer);
}


// Function name	: *CComBSTR2::ToString
// Description	    : 
// Return type		: char 
char *CComBSTR2::ToString()
{
	if (m_Buffer)
		free(m_Buffer);
	m_Buffer = NULL;

	int result = ::WideCharToMultiByte(CP_ACP, 0, m_str, Length() ,0,0 , NULL, NULL);
	if (result<1)
		return "";

	m_Buffer = (char *)malloc(result + 1);
	::WideCharToMultiByte(CP_ACP, 0, m_str, Length() ,m_Buffer,result , NULL, NULL);
	m_Buffer[result]=0;

	return m_Buffer;	
}

// Function name	: *CComBSTR2::ToPersistentString
// Description	    : 
// Return type		: char 
char *CComBSTR2::ToPersistentString()
{

	if (m_Buffer)
		return m_Buffer;
	else
		return ToString();
}


