// freeSSHdUser.h: interface for the CfreeSSHdUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREESSHDUSER_H__0826E1A7_A6F6_4C99_85BA_9A85E87129CE__INCLUDED_)
#define AFX_FREESSHDUSER_H__0826E1A7_A6F6_4C99_85BA_9A85E87129CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CfreeSSHdHandler;

// user info structure (kept for compatibility with older versions)

typedef struct freeSSHdUserInfo
{
	char Name[256];
	char Password[20];
	int Auth;
	char Domain[256];
	BOOL Shell;
	BOOL SFTP;
	BOOL Tunnel;
} freeSSHdUserInfo;


class CfreeSSHdUser  
{
	private:
		freeSSHdUserInfo m_Data;
		CfreeSSHdHandler *m_Handler;

	public:
		CfreeSSHdUser();
		virtual ~CfreeSSHdUser();
		
		BOOL ConsumeData(freeSSHdUserInfo Data);
		freeSSHdUserInfo RetrieveData();

		BOOL AuthPassword(char *Password, UINT Length = MAX_PATH);
		BOOL AuthPublicKey(char *PublicKey, UINT Length = MAX_PATH);
		const BOOL   IsPasswordSet() const;

		const char * GetName() const;
		const char * GetDomain() const;
		      char * GetHomeDir(char *Dir);
		const int    GetAuthType() const;
		const BOOL   HasShellAccess() const;
		const BOOL   HasSFTPAccess() const;
		const BOOL   HasTunnelAccess() const;

		BOOL		 SetName(char *Name);
		BOOL		 SetPassword(char *Password);
		BOOL         SetAuthType(int AuthType);
		BOOL         SetDomain(char *Domain);
		BOOL         SetShellAccess(BOOL Access);
		BOOL		 SetSFTPAccess(BOOL Access);
		BOOL		 SetTunnelAccess(BOOL Access);
		
};

#endif // !defined(AFX_FREESSHDUSER_H__0826E1A7_A6F6_4C99_85BA_9A85E87129CE__INCLUDED_)
