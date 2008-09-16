// freeSSHdHandler.cpp: implementation of the CfreeSSHdHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "maindlg.h"
#include "pages.h"

#ifdef _ISFREEEXE
#include "../../component/wodSSHDCom.h"
#include "../../component/SSHKeyPair.h"
#include "../../component/SSHUsers.h"
#endif 

#include "wildmat.h"
#include "ComBSTR2.h"

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <vector>

#include "buffer.h"

void ToHex(Buffer *b, char *data, int len)
{
	const char hex[17] = "0123456789ABCDEF";
	
	for (int i=0;i<len;i++)
	{
		unsigned char d = (unsigned char)data[i];
		b->Append(&hex[d/16],1);
		b->Append(&hex[d%16],1);
	}
	
	b->Append("\0",1);
	b->ConsumeEnd(1);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _ISFREEOPEN
bool IsWinNT()  //check if we're running NT
{
  OSVERSIONINFO osv;
  osv.dwOSVersionInfoSize = sizeof(osv);
  GetVersionEx(&osv);
  return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
#endif

CfreeSSHdHandler::CfreeSSHdHandler()
{

	DWORD dwBuffer = MAX_PATH;
	m_IsInstalled = FALSE;
	AutoGenerateKeys = FALSE;
	
	m_Sheet = NULL;
	m_pgPageStatus	= NULL;
	m_pgPageTelnet = NULL;
	m_pgPageSSH = NULL;
	m_pgPageUsers = NULL;
	m_pgPageHostRestrictions = NULL;
	m_pgPageCrypto = NULL;
	m_pgPageLogging = NULL;
	
	m_StartupUpdate = FALSE;
	
#ifdef _ISFREEOPEN
	m_UpdateEvents = NULL;

	CoInitialize(NULL);
	HRESULT hr = m_Update.CoCreateInstance(WODAPPUPDATECOMLib::CLSID_wodAppUpdateCom);

	if (FAILED(hr))
	{
		m_AppUpdateOK = FALSE;
		CoUninitialize();
	}
	else
	{
		CComBSTR dialognoteurl = "http://www.weonlydo.com/index.asp?showform=AppUpdate&from=SSHServer";
		CComBSTR updater("freeSSHdUpdate");

		m_AppUpdateOK = TRUE;
		m_UpdateEvents = new wodAppUpdateEvents(m_Update);
		m_Update->put_UpdaterName(updater);
		m_Update->put_DialogNoteURL(dialognoteurl);
 	}
#else
	
	WODAPPUPDCOMLib::_AppUpd_LibInit(_Module.m_hInst);	

	memset (&m_AppUpdEvents, 0, sizeof(m_AppUpdEvents));
	m_AppUpdEvents.CheckDone = wodAppUpd_CheckDone;
	m_AppUpdEvents.DownloadDone = wodAppUpd_DownloadDone;
	m_AppUpdEvents.PrevDetected = wodAppUpd_PrevDetected;

	m_Update = WODAPPUPDCOMLib::_AppUpd_Create(&m_AppUpdEvents);

	if (m_Update)
		m_AppUpdateOK = TRUE;
	//VARIANT var;
	//var.vt = VT_I4;
	//var.lVal = (LONG)this;
	//WODAPPUPDCOMLib::AppUpd_SetTag(m_Update, var);
#endif

	memset(m_Path, 0, MAX_PATH + 1);

	CRegKey key;
	
	LONG lRes = key.Open(HKEY_CLASSES_ROOT, _T("TypeLib"), KEY_READ);
    
	if (lRes != ERROR_SUCCESS)
        return;

    lRes = key.Open(key, _T("{F641DBB3-CBB1-47A3-A258-957DF1E10260}"), KEY_READ);
    
	if (lRes != ERROR_SUCCESS)
        return;

	lRes = key.Open(key, _T("1.0"), KEY_READ);
    
	if (lRes != ERROR_SUCCESS)
        return;

	lRes = key.Open(key, _T("HELPDIR"), KEY_READ);
    
	if (lRes != ERROR_SUCCESS)
        return;

	if (key.QueryValue(m_Path,"", &dwBuffer ) == ERROR_SUCCESS)
		m_IsInstalled = TRUE;
	
	
}

CfreeSSHdHandler::~CfreeSSHdHandler()
{
	ClearUsers();

	if (m_Sheet)
	{
		if (m_Sheet->IsWindow())
			m_Sheet->DestroyWindow();
			
		delete m_Sheet;
		delete m_pgPageStatus;
		delete m_pgPageTelnet;
		delete m_pgPageSSH;
		delete m_pgPageCrypto;
		delete m_pgPageSFTP;
		delete m_pgPageTunneling;
		delete m_pgPageAuth;
		delete m_pgPageUsers;
		delete m_pgPageHostRestrictions;
		delete m_pgPageLogging;
		delete m_pgPageOnlineUsers;
		delete m_pgPageUpdate;
	}

	if (m_AppUpdateOK)
	{
#ifdef _ISFREEOPEN
		delete m_UpdateEvents;
		m_Update.Release();
		CoUninitialize();
#else
		WODAPPUPDCOMLib::_AppUpd_Destroy(m_Update);
		WODAPPUPDCOMLib::_AppUpd_LibDeInit();
#endif	
	}
}

BOOL CfreeSSHdHandler::IsInstalled()
{
	return m_IsInstalled;
}

char * CfreeSSHdHandler::GetPath()
{
	return m_Path;
}

BOOL CfreeSSHdHandler::LoadConfig()
{
	memset(&m_Config, 0, sizeof(m_Config));

	char buff[1024];
	

	// set defaults

	strcpy(m_Config.TelnetListenAddress, "0.0.0.0");
	m_Config.TelnetListenPort = 23;
	char *a = getenv("COMSPEC");
	if (!a || !*a)
	{
		if (IsWinNT())
			a = "CMD.EXE";
		else
			a = "COMMAND.EXE";
	}

	strncpy(m_Config.TelnetCMD, a, MAX_PATH);
	m_Config.TelnetRun = FALSE;

	strcpy(m_Config.SSHListenAddress, "0.0.0.0");
	m_Config.SSHListenPort = 22;
	strncpy(m_Config.SSHCMD, a, MAX_PATH);
	m_Config.SSHRun = TRUE;
	m_Config.SSHCiphers = 0;
	m_Config.SSHMACs = 65535;
	m_Config.SSHCompression = 9;
	m_Config.SSHPasswordAuth = 0;
	m_Config.SSHPublickeyAuth = 0;
	strncpy(m_Config.SSHPublickeyPath, GetPath(), MAX_PATH);
	strncpy(m_Config.SFTPHomePath , "$HOME\\", MAX_PATH);
	m_Config.LogEvents = FALSE;
	m_Config.LogResolveIP = FALSE;
	strncpy(m_Config.LogFilePath, GetPath(), MAX_PATH-12);
	strncat(m_Config.LogFilePath, "freesshd.log", 12);
	
	// version 1.2.0
	m_Config.TelnetNewConsole = 1;
	m_Config.SSHNewConsole = 1;
	m_Config.UpdateCheckOnStartup = 1;
	m_Config.UpdateShowMessages = 1;

	// first try to load from INI file
	
	strcpy(buff, m_Path);
	strcat(buff, "\\FreeSSHDService.ini");

	OFSTRUCT of = { 0 };
	
	HANDLE hfile = CreateFile(buff, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (hfile == INVALID_HANDLE_VALUE)
	{
		m_OldFormat = TRUE;
		strcpy(buff, m_Path);
		strcat(buff, "FreeSSHDService.cfg");
		int handle = open(buff, O_BINARY | O_RDONLY, S_IREAD);
		if (handle>=0)
		{
			int gcsize;
			read(handle, &gcsize, sizeof(int));
			read(handle, &m_Config, gcsize<sizeof(m_Config)?gcsize:sizeof(m_Config));
			if (gcsize>sizeof(m_Config))
				lseek(handle, gcsize - sizeof(m_Config), SEEK_CUR);

			LoadUsers(handle);
		
			close(handle);
		}
		else
		{
			// probably first run
			m_OldFormat = FALSE; 
			memcpy(&Config, &m_Config, sizeof(Config));
			LoadUsers(-1);
			return FALSE;
		}
	}
	else
	{	
		m_OldFormat = FALSE;
		
		CloseHandle(hfile);		
		
		GetPrivateProfileString("Telnet server", "TelnetListenAddress", m_Config.TelnetListenAddress, m_Config.TelnetListenAddress, 255, buff);
		m_Config.TelnetListenPort = GetPrivateProfileInt("Telnet server", "TelnetListenPort", m_Config.TelnetListenPort, buff);
		m_Config.TelnetMaxConnections = GetPrivateProfileInt("Telnet server", "TelnetMaxConnections", m_Config.TelnetMaxConnections, buff);
		m_Config.TelnetTimeout = GetPrivateProfileInt("Telnet server", "TelnetTimeout", m_Config.TelnetTimeout, buff);
		GetPrivateProfileString("Telnet server", "TelnetBanner", m_Config.TelnetBanner, m_Config.TelnetBanner, MAX_PATH + 1, buff);
		GetPrivateProfileString("Telnet server", "TelnetCMD", m_Config.TelnetCMD, m_Config.TelnetCMD, MAX_PATH + 1, buff);
		m_Config.TelnetRun = GetPrivateProfileInt("Telnet server", "TelnetRun", m_Config.TelnetRun, buff);
		m_Config.TelnetNewConsole = GetPrivateProfileInt("Telnet server", "TelnetNewConsole", m_Config.TelnetNewConsole, buff); //version 1.2.0

		GetPrivateProfileString("SSH server", "SSHListenAddress", m_Config.SSHListenAddress, m_Config.SSHListenAddress, 255, buff);
		m_Config.SSHListenPort = GetPrivateProfileInt("SSH server", "SSHListenPort", m_Config.SSHListenPort, buff);
		m_Config.SSHMaxConnections = GetPrivateProfileInt("SSH server", "SSHMaxConnections", m_Config.SSHMaxConnections, buff);
		m_Config.SSHTimeout = GetPrivateProfileInt("SSH server", "SSHTimeout", m_Config.SSHTimeout, buff);
		GetPrivateProfileString("SSH server", "SSHBanner", m_Config.SSHBanner, m_Config.SSHBanner, MAX_PATH + 1, buff);
		GetPrivateProfileString("SSH server", "SSHCMD", m_Config.SSHCMD, m_Config.SSHCMD, MAX_PATH + 1, buff);
		m_Config.SSHRun = GetPrivateProfileInt("SSH server", "SSHRun", m_Config.SSHRun, buff);
		m_Config.SSHNewConsole = GetPrivateProfileInt("SSH server", "SSHNewConsole", m_Config.SSHNewConsole, buff); //version 1.2.0
		m_Config.SSHCiphers = GetPrivateProfileInt("SSH server", "SSHCiphers", m_Config.SSHCiphers, buff);
		m_Config.SSHMACs = GetPrivateProfileInt("SSH server", "SSHMACs", m_Config.SSHMACs, buff);
		m_Config.SSHPasswordAuth = GetPrivateProfileInt("SSH server", "SSHPasswordAuth", m_Config.SSHPasswordAuth, buff);
		m_Config.SSHPublickeyAuth = GetPrivateProfileInt("SSH server", "SSHPublickeyAuth", m_Config.SSHPublickeyAuth, buff);
		GetPrivateProfileString("SSH server", "SSHPublickeyPath", m_Config.SSHPublickeyPath, m_Config.SSHPublickeyPath, MAX_PATH+1, buff);
		GetPrivateProfileString("SSH server", "RSAKeyPath", m_Config.RSAKeyPath, m_Config.RSAKeyPath, MAX_PATH+1, buff);
		GetPrivateProfileString("SSH server", "DSAKeyPath", m_Config.DSAKeyPath, m_Config.DSAKeyPath, MAX_PATH+1, buff);
		m_Config.SSHLocalTunnel = GetPrivateProfileInt("SSH tunneling", "SSHLocalTunnel", m_Config.SSHLocalTunnel, buff);
		m_Config.SSHLocalTunnelOnly = GetPrivateProfileInt("SSH tunneling", "SSHLocalTunnelOnly", m_Config.SSHLocalTunnelOnly, buff);
		m_Config.SSHRemoteTunnel = GetPrivateProfileInt("SSH tunneling", "SSHRemoteTunnel", m_Config.SSHRemoteTunnel, buff);
		m_Config.SSHRemoteTunnelOnly = GetPrivateProfileInt("SSH tunneling", "SSHRemoteTunnelOnly", m_Config.SSHRemoteTunnelOnly, buff);
		
		GetPrivateProfileString("SFTP", "SFTPHomePath", m_Config.SFTPHomePath, m_Config.SFTPHomePath, MAX_PATH+1, buff);
		
		char tmp[2048] = {0};

		GetPrivateProfileString("Access filtering", "HostRestrictions", tmp, tmp, 2048, buff);
		
		for(int i=0;i<2048;i++) 
		{
			if (tmp[i] == '\\') tmp[i]='\r';
			if (tmp[i] == '/') tmp[i]='\n';
		}

		memcpy(m_Config.HostRestrictions, tmp, 2048);
	
	
		m_Config.HostRestrictionsAllow = GetPrivateProfileInt("Access filtering", "HostRestrictionsAllow", m_Config.HostRestrictionsAllow, buff);
		
		m_Config.LogEvents = GetPrivateProfileInt("Logging", "LogEvents", m_Config.LogEvents, buff);
		GetPrivateProfileString("Logging", "LogFilePath", m_Config.LogFilePath, m_Config.LogFilePath, MAX_PATH+1, buff);
		m_Config.LogResolveIP = GetPrivateProfileInt("Logging", "LogResolveIP", m_Config.LogResolveIP, buff);

		m_Config.UpdateCheckOnStartup = GetPrivateProfileInt("Automatic updates", "UpdateCheckOnStartup", m_Config.UpdateCheckOnStartup, buff);	//version 1.2.0
		m_Config.UpdateDontPrompt = GetPrivateProfileInt("Automatic updates", "UpdateDontPrompt", m_Config.UpdateDontPrompt, buff);	//version 1.2.0
		m_Config.UpdateShowMessages = GetPrivateProfileInt("Automatic updates", "UpdateShowMessages", m_Config.UpdateShowMessages, buff);	//version 1.2.0
		m_Config.UpdateLastMessageID = GetPrivateProfileInt("Automatic updates", "UpdateLastMessageID", m_Config.UpdateLastMessageID, buff);	//version 1.2.0

		// users

		ClearUsers();

		i = GetPrivateProfileInt("Users", "UserCount", 0, buff);

		for (int l=0; l<i; l++)
		{
			char buff2[256];
			CfreeSSHdUser *ti = new CfreeSSHdUser;
			freeSSHdUserInfo UserInfo;

			memset(&UserInfo, 0, sizeof(freeSSHdUserInfo));
			sprintf(buff2, "User%d", l);
			
			GetPrivateProfileString(buff2, "Name", "???", UserInfo.Name, 256, buff);
			UserInfo.Auth = GetPrivateProfileInt(buff2, "Auth", UserInfo.Auth, buff);
			GetPrivateProfileStruct(buff2, "Password", (LPVOID) UserInfo.Password, 20, buff);
			GetPrivateProfileString(buff2, "Domain", UserInfo.Domain, UserInfo.Domain, 256, buff);
			UserInfo.Shell = GetPrivateProfileInt(buff2, "Shell", UserInfo.Shell, buff);
			UserInfo.SFTP = GetPrivateProfileInt(buff2, "SFTP", UserInfo.SFTP, buff);
			UserInfo.Tunnel = GetPrivateProfileInt(buff2, "Tunnel", UserInfo.Tunnel, buff);
			
			ti->ConsumeData(UserInfo);

			Users.push_back(ti);
		}
	}
	
	memcpy(&Config, &m_Config, sizeof(Config));

	return TRUE;
}

void CfreeSSHdHandler::LoadUsers(int handle)
{
	DWORD l = MAX_PATH;

	Users.clear();

	if (handle >=0)
	{
		int count;
		read(handle, &count, sizeof(count));
		if (count>0)
		{
			while (count--)
			{
				freeSSHdUserInfo UserInfo;

				if (read(handle, &UserInfo, sizeof(freeSSHdUserInfo)) == sizeof(freeSSHdUserInfo)) 
				{
					CfreeSSHdUser *ti = new CfreeSSHdUser;
					ti->ConsumeData(UserInfo);
					Users.push_back(ti);
				}
			}
		}
	}

}

BOOL CfreeSSHdHandler::SaveConfig()
{
	int i = 0;
	BOOL success = TRUE;
	char buff[1024];
	char buff2[256];
	strcpy(buff, m_Path);
	
	memcpy(&m_Config, &Config, sizeof(Config));

	if (m_OldFormat)
	{
		strcat(buff, "\\FreeSSHDService.cfg");
		int handle = open(buff, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
		if (handle>=0)
		{
			int gcsize = sizeof(freeSSHdConfig);
			write(handle, &gcsize, sizeof(int));
			if (write(handle, &m_Config, sizeof(freeSSHdConfig)) != sizeof(freeSSHdConfig))
				success = FALSE;

			// now write users
			int i = Users.size();
			write(handle, &i, sizeof(i));
		
			for (int l=0;l<i;l++)
			{
				write(handle, Users[l], sizeof(freeSSHdUserInfo));
			}

			close(handle);
		}
		else
			success = FALSE;

		return success;
	}
	else
	{
		strcat(buff, "\\FreeSSHDService.ini");
		
		DeleteFile(buff);
		
		WritePrivateProfileString("Telnet server","TelnetListenAddress",m_Config.TelnetListenAddress, buff);
		
		itoa(m_Config.TelnetListenPort, buff2, 10);
		WritePrivateProfileString("Telnet server", "TelnetListenPort", buff2, buff);
		
		itoa(m_Config.TelnetMaxConnections, buff2, 10);
		WritePrivateProfileString("Telnet server", "TelnetMaxConnections", buff2, buff);
		
		itoa(m_Config.TelnetTimeout, buff2, 10);
		WritePrivateProfileString("Telnet server", "TelnetTimeout", buff2, buff);
		
		WritePrivateProfileString("Telnet server", "TelnetBanner", m_Config.TelnetBanner, buff);

		WritePrivateProfileString("Telnet server", "TelnetCMD",	m_Config.TelnetCMD, buff);

		itoa(m_Config.TelnetRun, buff2, 10);
		WritePrivateProfileString("Telnet server", "TelnetRun", buff2, buff);

		itoa(m_Config.TelnetNewConsole, buff2, 10);
		WritePrivateProfileString("Telnet server", "TelnetNewConsole", buff2, buff);	// version 1.2.0

		WritePrivateProfileString("SSH server", "SSHListenAddress", m_Config.SSHListenAddress, buff);

		itoa(m_Config.SSHListenPort, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHListenPort", buff2, buff);

		itoa(m_Config.SSHMaxConnections, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHMaxConnections", buff2, buff);
		
		itoa(m_Config.SSHTimeout, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHTimeout", buff2, buff);
		
		WritePrivateProfileString("SSH server", "SSHBanner", m_Config.SSHBanner, buff);

		WritePrivateProfileString("SSH server", "SSHCMD",	m_Config.SSHCMD, buff);

		itoa(m_Config.SSHRun, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHRun", buff2, buff);		// version 1.2.0

		itoa(m_Config.SSHNewConsole, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHNewConsole", buff2, buff);

		ltoa(m_Config.SSHCiphers, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHCiphers", buff2, buff);

		ltoa(m_Config.SSHMACs, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHMACs", buff2, buff);

		itoa(m_Config.SSHPasswordAuth, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHPasswordAuth", buff2, buff);
		
		itoa(m_Config.SSHPublickeyAuth, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHPublickeyAuth", buff2, buff);
		
		WritePrivateProfileString("SSH server", "SSHPublickeyPath", m_Config.SSHPublickeyPath, buff);

	/*	itoa(m_Config.SSHCompression, buff2, 10);
		WritePrivateProfileString("SSH server", "SSHCompression", buff2, buff);
	*/
		WritePrivateProfileString("SSH server", "RSAKeyPath", m_Config.RSAKeyPath, buff);

		WritePrivateProfileString("SSH server", "DSAKeyPath", m_Config.DSAKeyPath, buff);

		itoa(m_Config.SSHLocalTunnel, buff2, 10);
		WritePrivateProfileString("SSH tunneling", "SSHLocalTunnel", buff2, buff);

		itoa(m_Config.SSHLocalTunnelOnly, buff2, 10);
		WritePrivateProfileString("SSH tunneling", "SSHLocalTunnelOnly", buff2, buff);

		itoa(m_Config.SSHRemoteTunnel, buff2, 10);
		WritePrivateProfileString("SSH tunneling", "SSHRemoteTunnel", buff2, buff);
		
		itoa(m_Config.SSHRemoteTunnelOnly, buff2, 10);
		WritePrivateProfileString("SSH tunneling", "SSHRemoteTunnelOnly", buff2, buff);

		WritePrivateProfileString("SFTP", "SFTPHomePath", m_Config.SFTPHomePath, buff);

		char tmp[2048];
		
		memcpy(tmp, m_Config.HostRestrictions, 2048);
		
		for(i=0;i<2048;i++) 
		{
			if (tmp[i] == '\r') tmp[i]='\\';
			if (tmp[i] == '\n') tmp[i]='/';
		}

		WritePrivateProfileString("Access filtering", "HostRestrictions", tmp, buff);

		itoa(m_Config.HostRestrictionsAllow, buff2, 10);
		WritePrivateProfileString("Access filtering", "HostRestrictionsAllow", buff2, buff);

		itoa(m_Config.LogEvents, buff2, 10);
		WritePrivateProfileString("Logging", "LogEvents", buff2, buff);

		WritePrivateProfileString("Logging", "LogFilePath", m_Config.LogFilePath, buff);

		itoa(m_Config.LogResolveIP, buff2, 10);
		WritePrivateProfileString("Logging", "LogResolveIP", buff2, buff);

		itoa(m_Config.UpdateCheckOnStartup, buff2, 10);
		WritePrivateProfileString("Automatic updates", "UpdateCheckOnStartup", buff2, buff);		// version 1.2.0

		itoa(m_Config.UpdateDontPrompt, buff2, 10);
		WritePrivateProfileString("Automatic updates", "UpdateDontPrompt", buff2, buff);		// version 1.2.0

		itoa(m_Config.UpdateShowMessages, buff2, 10);
		WritePrivateProfileString("Automatic updates", "UpdateShowMessages", buff2, buff);		// version 1.2.0

		itoa(m_Config.UpdateLastMessageID, buff2, 10);
		WritePrivateProfileString("Automatic updates", "UpdateLastMessageID", buff2, buff);		// version 1.2.0

		// Users

		 i = Users.size();
		
		itoa(i, buff2, 10);
		WritePrivateProfileString("Users", "UserCount", buff2, buff);
		
		for (int l=0;l<i;l++)
		{
			freeSSHdUserInfo UserInfo = Users[l]->RetrieveData();
			
			char buff3[256];
			
			sprintf(buff3,"User%d",l);
			
			WritePrivateProfileString(buff3, "Name", UserInfo.Name, buff);

			itoa(UserInfo.Auth, buff2, 10);
			WritePrivateProfileString(buff3, "Auth", buff2, buff);
			
			WritePrivateProfileStruct(buff3, "Password", (LPVOID)UserInfo.Password, 20, buff);

			WritePrivateProfileString(buff3, "Domain", UserInfo.Domain, buff);

			WritePrivateProfileString(buff3, "Shell", (UserInfo.Shell)?"1":"0", buff);

			WritePrivateProfileString(buff3, "SFTP", (UserInfo.SFTP)?"1":"0", buff);

			WritePrivateProfileString(buff3, "Tunnel", (UserInfo.Tunnel)?"1":"0", buff);
		}
	
		return success;
	}

}

CfreeSSHdUser * CfreeSSHdHandler::GetUser(char * Login)
{
	for (int i =0; i < Users.size(); i++)
	{
		if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, Users[i]->GetName(), -1, Login, -1) == CSTR_EQUAL)
			return Users[i];
	}

	return NULL;
}

BOOL CfreeSSHdHandler::RemoveUser(char *Login)
{
	for (int i =0; i < Users.size(); i++)
	{
		if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, Users[i]->GetName(), -1, Login, -1) == CSTR_EQUAL)
		{
			Users.erase(Users.begin() + i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CfreeSSHdHandler::ShowSettingsDialog()
{
	if (m_Sheet)
	{
		if (m_Sheet->IsWindow())
		{
			m_Sheet->ShowWindow(SW_SHOW);
			return TRUE;
		}
		
		delete m_Sheet;
		delete m_pgPageStatus;
		delete m_pgPageTelnet;
		delete m_pgPageSSH;
		delete m_pgPageCrypto;
		delete m_pgPageSFTP;
		delete m_pgPageTunneling;
		delete m_pgPageAuth;
		delete m_pgPageUsers;
		delete m_pgPageHostRestrictions;
		delete m_pgPageLogging;
		delete m_pgPageOnlineUsers;
		delete m_pgPageUpdate;
	}
	
	m_Sheet = new CSettingsDlg("freeSSHd settings");
	m_pgPageStatus	= new CPageStatus;
	m_pgPageTelnet = new CPageTelnet;
	m_pgPageSSH = new CPageSSH;
	m_pgPageUsers = new CPageUsers;
	m_pgPageHostRestrictions = new CPageHostRestrictions;
	m_pgPageCrypto = new CPageCrypto;
	m_pgPageSFTP = new CPageSFTP;
	m_pgPageTunneling = new CPageTunneling;
	m_pgPageAuth = new CPageAuth;
	m_pgPageLogging = new CPageLogging;
	m_pgPageOnlineUsers = new CPageOnlineUsers;
	m_pgPageUpdate	= new CPageUpdate;
	
	m_Sheet->AddPage (*m_pgPageStatus);
	m_Sheet->AddPage (*m_pgPageTelnet);
	m_Sheet->AddPage (*m_pgPageSSH);
	m_Sheet->AddPage (*m_pgPageAuth);
	m_Sheet->AddPage (*m_pgPageCrypto);
	m_Sheet->AddPage (*m_pgPageTunneling);
	m_Sheet->AddPage (*m_pgPageSFTP);
	m_Sheet->AddPage (*m_pgPageUsers);
	m_Sheet->AddPage (*m_pgPageHostRestrictions);
	m_Sheet->AddPage (*m_pgPageLogging);
	m_Sheet->AddPage (*m_pgPageOnlineUsers);
	m_Sheet->AddPage (*m_pgPageUpdate);

	if (m_Sheet->Create(GetDesktopWindow()) == NULL) {
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}
		
	m_Sheet->SetIcon(LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME)));

	m_Sheet->ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL CfreeSSHdHandler::SSH_IsActive()
{

	VARIANT_BOOL b = VARIANT_FALSE;

	m_SSH->get_Status(&b);

	if (b)
		return TRUE;
	else
		return FALSE;


}

BOOL CfreeSSHdHandler::Telnet_IsActive()
{
	VARIANT_BOOL b = VARIANT_FALSE;

	m_Telnet->get_Status(&b);

	if (b)
		return TRUE;
	else
		return FALSE;
}

UINT CfreeSSHdHandler::SSH_UserCount()
{
	short count;
	CSSHUsers *users;
	m_SSH->get_Users(&users);
	users->get_Count(&count);
	return count;
}

UINT CfreeSSHdHandler::Telnet_UserCount()
{
	short count;
	CSSHUsers *users;
	m_Telnet->get_Users(&users);
	users->get_Count(&count);
	return count;
}

int CfreeSSHdHandler::SSH_Start()
{
	return StartSSH();
}

BOOL CfreeSSHdHandler::SSH_Stop()
{
	return (m_SSH->Stop() == S_OK);
}

int CfreeSSHdHandler::Telnet_Start()
{
	return StartTelnet();
}

BOOL CfreeSSHdHandler::Telnet_Stop()
{
	return (m_Telnet->Stop() == S_OK);
}

BOOL CfreeSSHdHandler::CheckHostRestriction(char *Host)
{
	Buffer b;
	BOOL match = FALSE;

	b.Append(Config.HostRestrictions, strlen(Config.HostRestrictions));
	b.Append("\r\n\0",3);
	
	if (Host && strlen(Host))
	{
		char *a = NULL;
		
		do
		{
			a = b.GetNextLine();

			if (a)
			{
				if (*a)
				{
					if (wildmat(Host, a)) match = TRUE;
				}
			}

		} while (a && !match);
	}

		
	if (match)
	{
		if (Config.HostRestrictionsAllow)
			return TRUE;
		else
			return FALSE;
	}	
	else
	{
		if (Config.HostRestrictionsAllow)
			return FALSE;
		else
			return TRUE;
	}
}


BOOL CfreeSSHdHandler::CheckHostKey(char *KeyFile, SSHKeyTypes KeyType, string &Status, UINT &Bits)
{
	Bits = 0;


	if (strlen(KeyFile))
	{
		CSSHKeyPairPtr m_Key = NULL;

#ifdef _ISFREEOPEN
		HRESULT hr;

		hr = m_Key.CreateInstance(wodSSHDComLIB::CLSID_SSHKeyPair, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL,"Failed to initialize KeyPair object!","Error",MB_OK);
		}
		else
			m_Key.AddRef();

#else
		m_Key = new CSSHKeyPair();
		m_Key->AddRef();
#endif

		

		VARIANT var;
		CComBSTR bstrKeyFile = KeyFile;
		var.vt = VT_ERROR;
							
		int err = m_Key->Load((BSTR)bstrKeyFile, var);
		if (!err)
		{
				var.vt = VT_I4;
				var.iVal = 0;
				VARIANT sa;
				sa.vt = VT_ERROR;
				err = m_Key->get_FingerPrintData(KeyType, var, &sa);
				if (!err)
				{
						SAFEARRAY *psa = sa.parray;
						char HUGEP *data;
						unsigned long ubound, lbound;
						
						SafeArrayGetLBound(psa,1,(long *)&lbound);
						SafeArrayGetUBound(psa,1,(long *)&ubound);
						
						unsigned long count = ubound-lbound+1;
						SafeArrayAccessData(psa, (void HUGEP**) &data);

						Buffer c;
						ToHex(&c, data, count);
						SafeArrayUnaccessData(psa);
						SafeArrayDestroy(psa);
						
						short sBits = 0;

						if (KeyType == 0)	
							m_Key->get_BitCount((SSHKeyTypes)0, &sBits);
						else
							Bits = m_Key->get_BitCount((SSHKeyTypes)1, &sBits);
						
						Bits = sBits;

						Status = c.Ptr();

#ifdef _ISFREEOPEN
						m_Key.Release();
#else
						m_Key->Release();
#endif

						return TRUE;
				}

		}
		if (err)
		{
			Status = GetErrorString(err);

			m_Key->Release();

			return FALSE;
		}		
	}
	else
	{
		Status = "No key file specified.";
		return FALSE;
	}


	return FALSE;
}

BOOL CfreeSSHdHandler::GenerateRSAKey(UINT Bits, string SaveToFile, string &Status)
{

		VARIANT var;
		var.vt = VT_I2;
		var.iVal = Bits;
		
		CSSHKeyPairPtr m_Key = NULL;;

#ifdef _ISFREEOPEN
		HRESULT hr;

		hr = m_Key.CreateInstance(wodSSHDComLIB::CLSID_SSHKeyPair, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL,"Failed to initialize KeyPair object!","Error",MB_OK);
		}
		else
			m_Key.AddRef();

#else
		m_Key = new CSSHKeyPair();
		m_Key->AddRef();	
#endif
		
		m_Key->Generate((SSHKeyTypes)0, var);
		
		var.vt = VT_ERROR;
		
		CComBSTR b1 = SaveToFile.c_str();
		
		int err = m_Key->Save((SSHKeyTypes)0, (BSTR)b1, var);
	
#ifdef _ISFREEOPEN	
		m_Key.Release();
#else
		m_Key->Release();
#endif
		if (err  == 0)
		{
			Status = "";
			return TRUE;
		}
		else
		{
			Status = GetErrorString(err);
			return FALSE;
							
		}

					
	return FALSE;
}

BOOL CfreeSSHdHandler::GenerateDSAKey(UINT Bits, string SaveToFile, string &Status)
{

	VARIANT var;
	var.vt = VT_I2;
	var.iVal = Bits;
	
	CSSHKeyPairPtr m_Key = NULL;;

#ifdef _ISFREEOPEN
	HRESULT hr;

	hr = m_Key.CreateInstance(wodSSHDComLIB::CLSID_SSHKeyPair, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL,"Failed to initialize KeyPair object!","Error",MB_OK);
	}
	else
		m_Key.AddRef();

#else
	m_Key = new CSSHKeyPair();
	m_Key->AddRef();	
#endif
	
	m_Key->Generate((SSHKeyTypes)1, var);
	
	var.vt = VT_ERROR;
	
	CComBSTR b1 = SaveToFile.c_str();
	
	int err = m_Key->Save((SSHKeyTypes)1, (BSTR)b1, var);

#ifdef _ISFREEOPEN
	m_Key.Release();
#else
	m_Key->Release();
#endif

	if (err  == 0)
	{
		Status = "";
		return TRUE;
	}
	else
	{
		Status = GetErrorString(err);
		return FALSE;
						
	}
			
		
	return FALSE;
}

void CfreeSSHdHandler::EditConfig_Start()
{
	memcpy(&m_TempConfig, &Config, sizeof(Config));
}

void CfreeSSHdHandler::EditConfig_Failed()
{
	memcpy(&Config, &m_TempConfig, sizeof(m_TempConfig));
}

void CfreeSSHdHandler::EditConfig_Complete()
{

}

void CfreeSSHdHandler::CheckForUpdate(BOOL StartingUp)
{
	m_StartupUpdate = StartingUp;

	CComBSTR url = "http://www.freesshd.com/update.php";
	CComVariant a = url;


	if (_Module.freeSSHdHandler.m_AppUpdateOK)
#ifdef _ISFREEOPEN
		_Module.freeSSHdHandler.m_Update->Check(a);
#else
		WODAPPUPDCOMLib::AppUpd_SetURL(m_Update, "http://www.freesshd.com/update.php");
		WODAPPUPDCOMLib::AppUpd_Check(m_Update);
#endif 
}

void CfreeSSHdHandler::SaveMessageID()
{
	int i = 0;

	char buff[1024];
	char buff2[256];

	strcpy(buff, m_Path);

	strcat(buff, "\\FreeSSHDService.ini");

	itoa(Config.UpdateLastMessageID, buff2, 10);
	WritePrivateProfileString("Automatic updates", "UpdateLastMessageID", buff2, buff);		// version 1.2.0

}

void CfreeSSHdHandler::ClearUsers()
{
	int i = Users.size();

	for (int j = 0; j < i; j++)
	{
		CfreeSSHdUser *fi = Users[j];

		delete fi;
	}

	Users.clear();
}

BOOL CfreeSSHdHandler::User_get_DidPubkey(CSSHUser *User)
{
	VARIANT vTag;
					
	User->get_Tag(&vTag);
	
	if (vTag.vt == VT_I2)
	{
		if ((vTag.iVal == 1) || (vTag.iVal == 3))
			return TRUE;
	}

	return FALSE;
}

BOOL CfreeSSHdHandler::User_get_DidPassword(CSSHUser *User)
{
	VARIANT vTag;

	User->get_Tag(&vTag);

	if (vTag.vt == VT_I2)
	{
		if ((vTag.iVal == 2) || (vTag.iVal == 3))
			return TRUE;
	}
	
	return FALSE;
}

BOOL CfreeSSHdHandler::User_put_DidPubkey(CSSHUser *User)
{
	VARIANT vTag;
					
	User->get_Tag(&vTag);
	
	if (vTag.vt == VT_I2)
		vTag.iVal++;
	else
	{
		vTag.vt = VT_I2;
		vTag.iVal = 1;
	}

	User->put_Tag(vTag);
	
	return TRUE;
}

BOOL CfreeSSHdHandler::User_put_DidPassword(CSSHUser *User)
{
	VARIANT vTag;
					
	User->get_Tag(&vTag);
	
	if (vTag.vt == VT_I2)
		vTag.iVal = vTag.iVal +2;
	else
	{
		vTag.vt = VT_I2;
		vTag.iVal = 2;
	}

	User->put_Tag(vTag);

	return TRUE;
}
