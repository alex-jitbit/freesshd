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

#ifndef __CCOMBSTR2
#define __CCOMBSTR2

class CComBSTR2 : public CComBSTR
{
public:
		char *ToString(void);
		char *ToPersistentString(void);
		char *m_Buffer;

		CComBSTR2();
		CComBSTR2(int nSize); 
		CComBSTR2(int nSize, LPCOLESTR sz ); 
		CComBSTR2(LPCOLESTR pSrc ); 
		CComBSTR2(const CComBSTR& src );
		CComBSTR2(REFGUID src); 
		CComBSTR2(LPCSTR pSrc);


		~CComBSTR2();
};

#endif