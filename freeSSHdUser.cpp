// freeSSHdUser.cpp: implementation of the CfreeSSHdUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "freeSSHdUser.h"

#ifndef _ISFREEOPEN
#include "..\..\Component\misc.h"
#endif

#ifdef _ISFREEEXE
#define _USE_OPENSSL
#endif

#ifdef _USE_OPENSSL
	#include "..\..\Component\OpenSSL\sha.h"
#else
	#include "sha1.h"
#endif

#include <string>
#include <cstdio>

#define HANDLER _Module.freeSSHdHandler

#ifdef _ISFREEOPEN
/* begin */

DWORD GetTextualSid(PSID pSid, LPTSTR *szSid)
{
    PSID_IDENTIFIER_AUTHORITY psia;
    DWORD dwSidSize, dwSubAuthorities;
    bool bRtnVal = true;
    DWORD RetVal = 0;
	DWORD dwCounter = 0;
    _ASSERTE(pSid);

    // Validate the binary SID
    if(!IsValidSid(pSid)){
        RetVal = ERROR_INVALID_PARAMETER;
        return RetVal;
    }
    
	// Get the identifier authority value from the SID
    psia = GetSidIdentifierAuthority(pSid);

    // Get the number of subauthorities in the SID.
    dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

    // Compute the buffer length
    // S-SID_REVISION- + IdentifierAuthority- + subauthorities- + NULL
    dwSidSize = (15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(TCHAR);
    
    *szSid = (LPTSTR)malloc(dwSidSize);
 
	if (szSid)
	{
        memset(*szSid,0,dwSidSize);

    }
	else
	{
        RetVal =  ERROR_NOT_ENOUGH_MEMORY;
        goto exitfunc;
    }
    
    // Add 'S' prefix and revision number to the string
    dwSidSize = _stprintf(*szSid, _T("S-%lu-"), SID_REVISION);

    // Add SID identifier authority to the string.
    if((psia->Value[0] != 0) || (psia->Value[1] != 0))
    {
        dwSidSize += _stprintf(*szSid + lstrlen(*szSid),
            _T("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
            (USHORT)psia->Value[0],
            (USHORT)psia->Value[1],
            (USHORT)psia->Value[2],
            (USHORT)psia->Value[3],
            (USHORT)psia->Value[4],
            (USHORT)psia->Value[5]);
    }
    else
    {
        dwSidSize += _stprintf(*szSid + lstrlen(*szSid),
            _T("%lu"),
            (ULONG)(psia->Value[5]) +
            (ULONG)(psia->Value[4] << 8) +
            (ULONG)(psia->Value[3] << 16) +
            (ULONG)(psia->Value[2] << 24) );
    }

    // Add SID subauthorities to the string
    for( dwCounter = 0; dwCounter < dwSubAuthorities; dwCounter++)
    {
        dwSidSize += _stprintf(*szSid + dwSidSize, _T("-%lu"),
            *GetSidSubAuthority(pSid, dwCounter));
    }
	

exitfunc:     
    
     return RetVal;
}

UINT FetchSID(LPTSTR strText/*,LPTSTR systemName*/,LPTSTR *SID/*,LPTSTR *domainName*/)
{
    BOOL            RetBln = FALSE;
    DWORD           sidSize = 0;
    DWORD           refDomainSize = 0;
    DWORD           returnValue = 0;
    SID_NAME_USE    snu;
    PSID            mySid    = NULL;
    LPTSTR          tempdomainName = NULL;
    
    sidSize = 0;
    refDomainSize = 255;

    //This is done just to know the buffer size for SID as well as Domain name to malloc
	
    returnValue = LookupAccountName (/*systemName*/ NULL, strText, mySid, &sidSize, tempdomainName, &refDomainSize, &snu);

    returnValue = GetLastError();

	if (returnValue != ERROR_INSUFFICIENT_BUFFER)
        return returnValue;

    if(sidSize){
        mySid = (PSID) malloc (sidSize);
        memset(mySid,0,sidSize);
    }
	else{
        returnValue = ERROR_INVALID_PARAMETER;
        goto exitfunc;
    }
    
    if(refDomainSize){
        tempdomainName = (LPTSTR) malloc (refDomainSize * sizeof(TCHAR));
        memset(tempdomainName,0,refDomainSize * sizeof(TCHAR));
    }
    
	//Now get the SID and the domain name
	
    if (!LookupAccountName (/*systemName*/ NULL,
                            strText,
                            mySid,
                            &sidSize,
                            tempdomainName,
                            &refDomainSize,
                            &snu))
    {
		returnValue = GetLastError();
        goto exitfunc;
    }
/*    *domainName = tempdomainName;*/
    //convert the PSID structure as a plain string SID

	free(tempdomainName);
	returnValue = GetTextualSid(mySid,SID);
    if(returnValue){
        goto exitfunc;;
    }

exitfunc:
    
    if(mySid){
        free(mySid);
        mySid = NULL;
    }

    return returnValue;
}
/* end */

DWORD PathFromLogin(LPTSTR Login, LPTSTR szPath)
{
	CRegKey rkReg;
	DWORD dwRet = 0;
	DWORD dwCount = _MAX_PATH;
	char tmp[_MAX_PATH];
	LPTSTR szSID;
	

	char *sysdrive = NULL;
	
	szSID = NULL;
	dwRet = FetchSID(Login,&szSID);
	
	if (dwRet == 0 )
	{
		sprintf(tmp,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s",szSID);
		if (szSID)
			free(szSID);
		szSID = NULL;
		if ( rkReg.Open(HKEY_LOCAL_MACHINE,tmp,KEY_READ) == ERROR_SUCCESS )
		{

			if (rkReg.QueryValue(tmp, "ProfileImagePath", &dwCount) == ERROR_SUCCESS)
			{
				
				sysdrive = getenv("SYSTEMDRIVE");
						
				strcpy(&(tmp[3]),&(tmp[14]));
						
				tmp[0]=sysdrive[0];
				tmp[1]=':';
				tmp[2]='\\';
				
				strcpy(szPath,tmp);
				
				
				rkReg.Close();
				return 1;
			}
			else 
			{
				rkReg.Close();
				return NULL;
			}
			
		}
		else return NULL;
	}
	else
	{
		if (szSID)
			free(szSID);
		return NULL;
	}

}

#endif 

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CfreeSSHdUser::CfreeSSHdUser()
{
	memset(&m_Data, 0, sizeof(m_Data));
}

CfreeSSHdUser::~CfreeSSHdUser()
{

}

BOOL CfreeSSHdUser::ConsumeData(freeSSHdUserInfo Data)
{
	m_Data = Data;

	return TRUE;
}

freeSSHdUserInfo CfreeSSHdUser::RetrieveData()
{
	return m_Data;
}

const char * CfreeSSHdUser::GetName() const
{
	return m_Data.Name;
}

const char * CfreeSSHdUser::GetDomain() const
{
	return m_Data.Domain;
}

char * CfreeSSHdUser::GetHomeDir(char *Dir)
{
	char SFTPHomeDir[MAX_PATH+1];
	char WinHomeDir[MAX_PATH+1];
	
	strncpy(SFTPHomeDir, HANDLER.Config.SFTPHomePath, MAX_PATH);	

	if (!Dir)
		return NULL;
				
	if ( strstr(SFTPHomeDir,"$HOME") ) 
	{
		PathFromLogin((LPTSTR) GetName(),WinHomeDir);
					
		strncat(WinHomeDir,SFTPHomeDir+5, MAX_PATH-6);
		strncpy(Dir, WinHomeDir, MAX_PATH);
		return Dir;
	}
	else
	{
		strncpy(Dir, SFTPHomeDir, MAX_PATH);
		return Dir;

	}
			
	return NULL;

}

const BOOL CfreeSSHdUser::IsPasswordSet() const
{
	if (GetAuthType() == 1)
	{
		BOOL fSet = FALSE;

		for (int i = 0; i < 20; i++)
			if (m_Data.Password[i] != 0)
				fSet = TRUE;
		
		return fSet;

	}	
	return TRUE;
}

const int    CfreeSSHdUser::GetAuthType() const
{
	return m_Data.Auth;
}

const BOOL   CfreeSSHdUser::HasShellAccess() const
{
	return m_Data.Shell ? TRUE:FALSE;
}

const BOOL   CfreeSSHdUser::HasSFTPAccess() const
{
	return m_Data.SFTP ? TRUE:FALSE;
}

const BOOL   CfreeSSHdUser::HasTunnelAccess() const
{
	return m_Data.Tunnel ? TRUE:FALSE;
}

BOOL CfreeSSHdUser::AuthPassword(char *Password, UINT Length)
{
	if (m_Data.Auth == 1)
	{
		unsigned char shabuff[20];

#ifdef _USE_OPENSSL
		SHA1((const unsigned char *)Password, strlen(Password), shabuff);
#else
		SHA1Context sha;
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) Password, strlen(Password));
		SHA1Result(&sha, shabuff);
#endif
		if (memcmp(shabuff,m_Data.Password, sizeof(char)*20))	
			return FALSE;
		else 
			return TRUE;
		
	}

	return FALSE;
}

BOOL CfreeSSHdUser::AuthPublicKey(char *PublicKey, UINT Length)
{
	FILE *fKeyFile = NULL;
	char cBuff[16384];
	int fOK = FALSE;
	char PKeyPath[MAX_PATH+1];
	char UserHomeDir[MAX_PATH+1];
	string sKeyFile;

	strncpy(PKeyPath, HANDLER.Config.SSHPublickeyPath, MAX_PATH);	
				
	if ( strstr(PKeyPath,"$HOME") ) 
	{
		PathFromLogin((LPTSTR) GetName(),UserHomeDir);
					
		strncat(UserHomeDir, PKeyPath+5, MAX_PATH-6);
		strncpy(PKeyPath, UserHomeDir, MAX_PATH);
	}
	
	sKeyFile = PKeyPath;

	if (sKeyFile[sKeyFile.length()-1] != '\\')
		sKeyFile += "\\";

	sKeyFile += GetName();

	fKeyFile = fopen(sKeyFile.c_str(), "rb");

	if (fKeyFile)
	{	
		int i = 0;

		while (!feof(fKeyFile) && !fOK)
		{
			memset(cBuff, 0, 16384);
			fgets(cBuff, 16380, fKeyFile);
			i = 0;
			i += 8;
				
			while (cBuff[i] && cBuff[i]!=' ')
				i++;
								
			if (!strncmp(cBuff, PublicKey, i))
				fOK = TRUE;
		}
		
		fclose(fKeyFile);
	}

	return fOK;
}

BOOL CfreeSSHdUser::SetName(char *Name)
{
	strncpy(m_Data.Name, Name, 256);
	
	m_Data.Name[255] = 0;

	return TRUE;
}

BOOL CfreeSSHdUser::SetPassword(char *Password)
{
	if (GetAuthType() == 1)
	{

		unsigned char shahash[20];

#ifdef _USE_OPENSSL
		SHA1((const unsigned char *)Password, strlen(Password), shahash);
#else
		SHA1Context sha;
		SHA1Reset(&sha);	
		SHA1Input(&sha, (const unsigned char *) Password, strlen(Password));
		SHA1Result(&sha, shahash);
#endif
		memcpy(m_Data.Password, shahash, sizeof(char)*20);
	}
	
	return FALSE;
}



BOOL CfreeSSHdUser::SetAuthType(int AuthType)
{
	if ( (AuthType < 0) || (AuthType > 2))
		return FALSE;

	m_Data.Auth = AuthType;

	return TRUE;
}

BOOL CfreeSSHdUser::SetDomain(char *Domain)
{
	strncpy(m_Data.Domain, Domain, 256);

	m_Data.Domain[255] = 0;

	return TRUE;
}

BOOL CfreeSSHdUser::SetShellAccess(BOOL Access)
{
	m_Data.Shell = Access;

	return TRUE;
}

BOOL CfreeSSHdUser::SetSFTPAccess(BOOL Access)
{
	m_Data.SFTP = Access;

	return TRUE;
}

BOOL CfreeSSHdUser::SetTunnelAccess(BOOL Access)
{
	m_Data.Tunnel = Access;

	return TRUE;
}