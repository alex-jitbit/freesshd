// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__C8DC1AA6_1052_496E_9680_A32B5A097A4B__INCLUDED_)
#define AFX_STDAFX_H__C8DC1AA6_1052_496E_9680_A32B5A097A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define STRICT

#define _WIN32_IE	0x0500
#define _WIN32_WINNT 0x0501
#define _RICHEDIT_VER	0x0100

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>


#ifdef _ISFREEEXE
#include <comdef.h>

	namespace WODAPPUPDCOMLib
	{
		#include "..\..\..\..\wodAppUpdate\Win32LIB\Win32LIB.h"
	}	
#else
	#import "c:\Windows\System32\wodappup.dll"  raw_interfaces_only named_guids
#endif

#include <shellapi.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module


#ifdef _ISFREEOPEN
		#import "C:\windows\system32\wodsshd.dll"  named_guids
#endif

#ifdef _ISFREEEXE
#define CwodSSHDComPtr CwodSSHDCom *
#define CSSHKeyPairPtr CSSHKeyPair *
#endif 


#ifdef _ISFREEOPEN
	#define CwodSSHDComPtr			wodSSHDComLIB::IwodSSHDComPtr 
	#define CSSHKeyPairPtr			wodSSHDComLIB::ISSHKeyPairPtr
	#define CSSHUser				wodSSHDComLIB::ISSHUser
	#define CSSHUsers				wodSSHDComLIB::ISSHUsers
	#define SSHKeyTypes				wodSSHDComLIB::SSHKeyTypes
	#define SSHActions				wodSSHDComLIB::SSHActions
	#define SSHStates				wodSSHDComLIB::SSHStates
	#define SSHServiceTypes			wodSSHDComLIB::SSHServiceTypes
	#define EncryptionsEnum			wodSSHDComLIB::EncryptionsEnum
	#define ProtocolsEnum			wodSSHDComLIB::ProtocolsEnum
	#define AuthenticationsEnum		wodSSHDComLIB::AuthenticationsEnum
	#define TerminalEmulationsEnum	wodSSHDComLIB::TerminalEmulationsEnum
	#define encAny					wodSSHDComLIB::encAny
	#define encAES					wodSSHDComLIB::encAES
	#define encAES128				wodSSHDComLIB::encAES128
	#define encAES192				wodSSHDComLIB::encAES192
	#define encAES256				wodSSHDComLIB::encAES256
	#define enc3DES					wodSSHDComLIB::enc3DES
	#define encBLOWFISH				wodSSHDComLIB::encBLOWFISH
	#define encCAST128				wodSSHDComLIB::encCAST128
	#define RSAKey					wodSSHDComLIB::RSAKey
	#define DSAKey					wodSSHDComLIB::DSAKey

	#define DIID__IwodSSHDComEvents		wodSSHDComLIB::DIID__IwodSSHDComEvents
#endif


#ifndef _ISFREEOPEN
#include "..\MyClassFactory.h"
#endif

#include "freeSSHdHandler.h"

#ifdef _ISFREEOPEN
	class SSHEvents;
	class TelnetEvents;
#endif

class CServiceModule : public CComModule
{
public:
	CServiceModule();
	~CServiceModule();
	HRESULT RegisterServer(BOOL bRegTypeLib, BOOL bService);
	HRESULT UnregisterServer();
	void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid = NULL);
    void Start();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
	LONG Unlock();
	void LogEvent(LPCTSTR pszFormat, ...);
    void SetServiceStatus(DWORD dwState);
    void SetupAsLocalServer();

	BOOL StartServers(void);
	BOOL StopServers(void);

//Implementation
private:
	static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI _Handler(DWORD dwOpcode);

// data members
public:
    TCHAR m_szServiceName[256];
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_status;
	DWORD dwThreadID;
	BOOL m_bService;

#ifdef _ISFREEOPEN
	SSHEvents *m_SSHEvents;
	TelnetEvents *m_TelnetEvents;
#endif 

	HWND		m_hWnd;
	NOTIFYICONDATA		m_NotifyIcon;
	CfreeSSHdHandler freeSSHdHandler;
	
	//events
	void static Telnet_Connecting(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action);
	void static Telnet_LoginPassword(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
	void static Telnet_Disconnected(CwodSSHDComPtr Owner, CSSHUser *User);
	void static Telnet_StateChanged(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState);
	void static Telnet_Received(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount);
	void static Telnet_ServiceRequest(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
	void static Telnet_ServiceStart(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
	void static Telnet_SftpListDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpDownloadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpUploadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpRemoveDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpDeleteFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpMakeDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static Telnet_SftpRename(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
	void static Telnet_PortBindRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
	void static Telnet_PortForwardRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
	void static Telnet_PortForwardConnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
	void static Telnet_LoginPubkey(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
	void static Telnet_CryptoInformation(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);

	void static SSH_Connecting(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action);
	void static SSH_LoginPassword(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
	void static SSH_Disconnected(CwodSSHDComPtr Owner, CSSHUser *User);
	void static SSH_StateChanged(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState);
	void static SSH_Received(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount);
	void static SSH_ServiceRequest(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
	void static SSH_ServiceStart(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
	void static SSH_SftpListDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpDownloadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpUploadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpRemoveDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpDeleteFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpMakeDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
	void static SSH_SftpRename(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
	void static SSH_PortBindRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
	void static SSH_PortForwardRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
	void static SSH_PortForwardConnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
	void static SSH_PortForwardDisconnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort);
	void static SSH_LoginPubkey(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
	void static SSH_CryptoInformation(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);


};

extern CwodSSHDComPtr m_Telnet;
extern CwodSSHDComPtr m_SSH;

extern CServiceModule _Module;

#define _WTL_NEW_PAGE_NOTIFY_HANDLERS

#include <atlcom.h>
#include <atlapp.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlcrack.h>
#include <atlctrlx.h>
#include <atlctrlw.h>

#undef _WTL_NO_WTYPES

#include <atlmisc.h>

int StartSSH();
int StartTelnet();

int ReloadSSH();
int ReloadTelnet();

#include "wodAppUpdateEvents.h"

void ShowBallonError(HWND Edit_hWnd, LPWSTR pszText);

BOOL IsLoggedOnUserAdmin();

#define TESTDLGMESSAGE( x ) if (bProcess && HANDLER.##x && HANDLER.##x->IsWindow() && ::IsDialogMessage(HANDLER.##x->m_hWnd, &msg)) \
						bProcess = FALSE;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C8DC1AA6_1052_496E_9680_A32B5A097A4B__INCLUDED)

