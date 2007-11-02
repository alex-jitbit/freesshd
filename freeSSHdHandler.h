// freeSSHdHandler.h: interface for the CfreeSSHdHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREESSHDHANDLER_H__D1B371FB_572A_4B37_8606_049F8FC569F8__INCLUDED_)
#define AFX_FREESSHDHANDLER_H__D1B371FB_572A_4B37_8606_049F8FC569F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "freeSSHdUser.h"
//#include "objs/wodAppUp.tlh"

#include <vector>

using namespace std;

#import "C:\Windows\System32\wodappup.dll"  no_namespace raw_interfaces_only named_guids


bool IsWinNT();

// configuration structure (kept for compatibility with older versions)

typedef struct freeSSHdConfig
{
	char TelnetListenAddress[256];
	int TelnetListenPort;
	int TelnetMaxConnections;
	int TelnetTimeout;
	char TelnetBanner[MAX_PATH+1];
	char TelnetCMD[MAX_PATH+1];
	int TelnetRun;

	char SSHListenAddress[256];
	int SSHListenPort;
	int SSHMaxConnections;
	int SSHTimeout;
	char SSHBanner[MAX_PATH+1];
	char SSHCMD[MAX_PATH+1];
	int SSHRun;
	long SSHCiphers;
	long SSHMACs;
	int SSHPasswordAuth;
	int SSHPublickeyAuth;
	char SSHPublickeyPath[MAX_PATH+1];
	int SSHCompression;

	char RSAKeyPath[MAX_PATH+1];
	char DSAKeyPath[MAX_PATH+1];

	int SSHLocalTunnel;
	int SSHLocalTunnelOnly;
	int SSHRemoteTunnel;
	int SSHRemoteTunnelOnly;

	char SFTPHomePath[MAX_PATH+1];
	int HostRestrictionsAllow;
	char HostRestrictions[2048];

	// version 1.0.3
	int LogEvents;
	char LogFilePath[MAX_PATH+1];
	int LogResolveIP;

	// version 1.2.0
	int TelnetNewConsole;
	int SSHNewConsole;

	int UpdateCheckOnStartup;
	int UpdateDontPrompt;
	int UpdateShowMessages;
	int UpdateLastMessageID;

} freeSSHdConfig;

class CSettingsDlg;
class CPageStatus;
class CPageTelnet;
class CPageSSH;
class CPageUsers;
class CPageHostRestrictions;
class CPageCrypto;
class CPageSFTP;
class CPageTunneling;
class CPageAuth;
class CPageLogging;
class CPageOnlineUsers;
class CPageUpdate;

class wodAppUpdateEvents;

class CfreeSSHdHandler  
{
	private:
		void LoadUsers(int handle);
		freeSSHdConfig	m_Config;
		freeSSHdConfig m_TempConfig;
		BOOL m_IsInstalled;
		BOOL m_OldFormat;
		char m_Path[MAX_PATH+1];
#ifdef _ISFREEOPEN
		wodAppUpdateEvents *m_UpdateEvents;
#endif
		void ClearUsers();

	public:
		
		BOOL SaveConfig();
		BOOL LoadConfig();

		void EditConfig_Start();
		void EditConfig_Failed();
		void EditConfig_Complete();
		
		char * GetPath();
		BOOL IsInstalled();
		
		int  SSH_Start();
		BOOL SSH_Stop();
		BOOL SSH_IsActive();
		UINT SSH_UserCount();

		int  Telnet_Start();
		BOOL Telnet_Stop();
		BOOL Telnet_IsActive();
		UINT Telnet_UserCount();

		void CheckForUpdate(BOOL StartingUp);
		void SaveMessageID();

		BOOL ShowSettingsDialog();

		BOOL GenerateRSAKey(UINT Bits, string SaveToFile, string &Status);
		BOOL GenerateDSAKey(UINT Bits, string SaveToFile, string &Status);

		BOOL CheckHostRestriction(char *Host);
		BOOL CheckHostKey(char *KeyFile, SSHKeyTypes KeyType, string &Status, UINT &Bits);

		BOOL User_get_DidPubkey(CSSHUser *User);
		BOOL User_get_DidPassword(CSSHUser *User);
		BOOL User_put_DidPubkey(CSSHUser *User);
		BOOL User_put_DidPassword(CSSHUser *User);
		CfreeSSHdUser * GetUser(char * Login);
		BOOL RemoveUser(char * Login);

		freeSSHdConfig Config;
		vector<CfreeSSHdUser *> Users;
		BOOL AutoGenerateKeys;
		
		CfreeSSHdHandler();
		virtual ~CfreeSSHdHandler();

		// Dialog pages
		CSettingsDlg *m_Sheet;
		CPageStatus				*m_pgPageStatus;
		CPageTelnet				*m_pgPageTelnet;
		CPageSSH				*m_pgPageSSH;
		CPageUsers				*m_pgPageUsers;
		CPageHostRestrictions	*m_pgPageHostRestrictions;
		CPageCrypto				*m_pgPageCrypto;
		CPageSFTP				*m_pgPageSFTP;
		CPageTunneling			*m_pgPageTunneling;
		CPageAuth				*m_pgPageAuth;
		CPageLogging			*m_pgPageLogging;
		CPageOnlineUsers		*m_pgPageOnlineUsers;
		CPageUpdate				*m_pgPageUpdate;

#ifdef _ISFREEOPEN
		CComPtr<WODAPPUPDATECOMLib::IwodAppUpdateCom> m_Update;
#else
		WODAPPUPDCOMLib::AppUpdEventsStruct m_AppUpdEvents;
		void *m_Update;
#endif
		BOOL m_AppUpdateOK;
		BOOL m_StartupUpdate;
};

#endif // !defined(AFX_FREESSHDHANDLER_H__D1B371FB_572A_4B37_8606_049F8FC569F8__INCLUDED_)





















