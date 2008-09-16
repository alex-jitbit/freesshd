// FreeSSHDService.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f FreeSSHDServiceps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "FreeSSHDService.h"
#include "FreeSSHDService_i.c"

#include <tlhelp32.h>

#ifdef _ISFREEEXE
#include "../../component/wodSSHDCom.h"
#include "../../component/SSHKeyPair.h"
#include "../../component/SSHUsers.h"
#endif

#include "ComBSTR2.h"

#include <stdio.h>
#include <fstream>
#include <fcntl.h>
#include <sys\stat.h>
#include "wildmat.h"
#include "freeSSHdHandler.h"
#include "maindlg.h"
#include "pages.h"

#include "log.h"

#ifdef _ISFREEOPEN
#include "events.h"
#endif

using namespace std;

CServiceModule _Module;

CwodSSHDComPtr m_Telnet;
CwodSSHDComPtr m_SSH;

	
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

#define	WM_ICON_NOTIFY WM_APP+10
UINT WM_TASKBARCREATEDMESSAGE;

CServiceModule::CServiceModule()
{
	m_hWnd = NULL;
#ifdef _ISFREEOPEN
	m_SSHEvents = NULL;
	m_TelnetEvents = NULL;
#endif
}
CServiceModule::~CServiceModule()
{
	if (m_hWnd)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_NotifyIcon);	
	}
	
#if _ISFREEOPEN
	if (m_SSHEvents)
		m_SSHEvents->Release();
	if (m_TelnetEvents)
		m_TelnetEvents->Release();

#endif

}

LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
    while (p1 != NULL && *p1 != NULL)
    {
        LPCTSTR p = p2;
        while (p != NULL && *p != NULL)
        {
            if (*p1 == *p)
                return CharNext(p1);
            p = CharNext(p);
        }
        p1 = CharNext(p1);
    }
    return NULL;
}

// Although some of these functions are big they are declared inline since they are only used once

inline HRESULT CServiceModule::RegisterServer(BOOL bRegTypeLib, BOOL bService)
{
    HRESULT hr, hr2 = CoInitialize(NULL);
    /*if (FAILED(hr))
        return hr;*/

    // Remove any previous service since it may point to
    // the incorrect file
    Uninstall();

    // Add service entries
    UpdateRegistryFromResource(IDR_FreeSSHDService, TRUE);

    // Adjust the AppID for Local Server or Service
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{E90D9404-F6D6-4AA1-8392-EBA95FEC3CB9}"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    key.DeleteValue(_T("LocalService"));
   
    if (bService)
    {
        key.SetValue(_T("FreeSSHDService"), _T("LocalService"));
        key.SetValue(_T("-Service"), _T("ServiceParameters"));
        // Create service
        Install();
    }

    // Add object entries
    hr = CComModule::RegisterServer(bRegTypeLib);

   if (!FAILED(hr2))
	   CoUninitialize();

    return hr;
}

inline HRESULT CServiceModule::UnregisterServer()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove service entries
    UpdateRegistryFromResource(IDR_FreeSSHDService, FALSE);
    // Remove service
    Uninstall();
    // Remove object entries
    CComModule::UnregisterServer(TRUE);
    CoUninitialize();
    return S_OK;
}

inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    CComModule::Init(p, h, plibid);

    m_bService = TRUE;

    LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
}

LONG CServiceModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0 && !m_bService)
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
    return l;
}

BOOL CServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

inline BOOL CServiceModule::Install()
{
    if (IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, m_szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), m_szServiceName, MB_OK);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

inline BOOL CServiceModule::Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), m_szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    MessageBox(NULL, _T("Service could not be deleted"), m_szServiceName, MB_OK);
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions
void CServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;

    if (/*m_bService*/1)
    {
        /* Get a handle to use with ReportEvent(). */
        hEventSource = RegisterEventSource(NULL, m_szServiceName);
        if (hEventSource != NULL)
        {
            /* Write to event log. */
            ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
            DeregisterEventSource(hEventSource);
        }
    }
    else
    {
        // As we are not running as a service, just write the error to the console.
        _putts(chMsg);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration
inline void CServiceModule::Start()
{
    SERVICE_TABLE_ENTRY st[] =
    {
        { m_szServiceName, _ServiceMain },
        { NULL, NULL }
    };
    if (m_bService && !::StartServiceCtrlDispatcher(st))
    {
        m_bService = FALSE;
    }
    if (m_bService == FALSE)
        Run();
}

inline void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPTSTR* /* lpszArgv */)
{
    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING);

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
	
	// wodAppUpdate reconfig

	if (freeSSHdHandler.m_AppUpdateOK)
	{
		CComBSTR cmdbefore = "net stop freesshdservice", cmdafter = "net start freesshdservice", cmdline = " ";
#ifdef _ISFREEOPEN
		freeSSHdHandler.m_Update->put_CmdBefore(cmdbefore);
		freeSSHdHandler.m_Update->put_CmdAfter(cmdafter);
		freeSSHdHandler.m_Update->put_CmdLine(cmdline);
#else
		WODAPPUPDCOMLib::AppUpd_SetCmdBefore(freeSSHdHandler.m_Update, "net stop freesshdservice");
		WODAPPUPDCOMLib::AppUpd_SetCmdAfter(freeSSHdHandler.m_Update, "net start freesshdservice");	
#endif
	}

    // When the Run function returns, the service has stopped.
    Run();

    SetServiceStatus(SERVICE_STOPPED);
    LogEvent(_T("Service stopped"));
}

inline void CServiceModule::Handler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        SetServiceStatus(SERVICE_STOP_PENDING);
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    _Module.ServiceMain(dwArgc, lpszArgv);
}
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
    _Module.Handler(dwOpcode); 
}

void CServiceModule::SetServiceStatus(DWORD dwState)
{
    m_status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

void CServiceModule::Run()
{
    _Module.dwThreadID = GetCurrentThreadId();

    HRESULT hr = CoInitialize(NULL);
//  If you are running on NT 4.0 or higher you can use the following call
//  instead to make the EXE free threaded.
//  This means that calls come in on a random RPC thread
//  HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    _ASSERTE(SUCCEEDED(hr));

    // This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    _ASSERTE(SUCCEEDED(hr));

    hr = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
    _ASSERTE(SUCCEEDED(hr));

    LogEvent(_T("Service started"));

    if (m_bService)
		SetServiceStatus(SERVICE_RUNNING);

	if (StartServers())
	{
		if (freeSSHdHandler.Config.UpdateCheckOnStartup)
			freeSSHdHandler.CheckForUpdate(TRUE);
		
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
		{
			BOOL bProcess = TRUE;
				
			TESTDLGMESSAGE(m_Sheet);
			TESTDLGMESSAGE(m_pgPageAuth);
			TESTDLGMESSAGE(m_pgPageCrypto);
			TESTDLGMESSAGE(m_pgPageHostRestrictions);
			TESTDLGMESSAGE(m_pgPageLogging);
			TESTDLGMESSAGE(m_pgPageOnlineUsers);
			TESTDLGMESSAGE(m_pgPageSFTP);
			TESTDLGMESSAGE(m_pgPageSSH);
			TESTDLGMESSAGE(m_pgPageStatus);
			TESTDLGMESSAGE(m_pgPageTelnet);
			TESTDLGMESSAGE(m_pgPageTunneling);
			TESTDLGMESSAGE(m_pgPageUpdate);
			TESTDLGMESSAGE(m_pgPageUsers);

			if (bProcess)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	StopServers();

    _Module.RevokeClassObjects();

    CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
//
/*
int ParseCommandLine(char *cmdline, int *args, int maxargs)
{
	BOOL inquote = FALSE;
	char *a = cmdline;
	char *b = NULL;
	int ar = 0;
	BOOL dstart = FALSE;
	while (a && *a && ar<maxargs)
	{
		if (*a=='\'' || *a=='"')
		{
			if (!inquote)
			{
				b = a+1;
				*a = 0;
			}
			else
				*a = ' ';
			inquote = !inquote;
		}


		if (*a==' ' && b && !inquote)
		{
			args[ar++] = b-cmdline;
			*a = NULL;
		}
		else
			if (!b)
				b = a;


		a++;
	}
	return ar;
}
*/
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, 
    HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
    lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
    _Module.Init(ObjectMap, hInstance, IDS_SERVICENAME, &LIBID_FREESSHDSERVICELib);
    _Module.m_bService = TRUE;

    TCHAR szTokens[] = _T("-/");

    LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
    while (lpszToken != NULL)
    {
        if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
            return _Module.UnregisterServer();

        // Register as Local Server
        if (lstrcmpi(lpszToken, _T("RegServer"))==0)
            return _Module.RegisterServer(TRUE, FALSE);
        
        // Register as Service
        if (lstrcmpi(lpszToken, _T("Service"))==0)
            return _Module.RegisterServer(TRUE, TRUE);

		if (lstrcmpi(lpszToken, _T("KeyGen"))==0)
				_Module.freeSSHdHandler.AutoGenerateKeys = TRUE;
        
        lpszToken = FindOneOf(lpszToken, szTokens);
    }

	if (_Module.freeSSHdHandler.AutoGenerateKeys)
	{
		VARIANT var;
		var.vt = VT_I2;
		var.iVal = 1024;
		CwodSSHDComPtr ssh = NULL;
#ifdef _ISFREEEXE
		ssh = new CwodSSHDCom();
#endif
#ifdef _ISFREEOPEN		
		ssh.CreateInstance(wodSSHDComLIB::CLSID_wodSSHDCom, NULL);
#endif
		ssh->AddRef();

		CSSHKeyPairPtr keys;
		ssh->get_Keys(&keys);
		keys->Generate((SSHKeyTypes)/*DSAKey*/1, var);

		var.vt = VT_ERROR;
		CComBSTR2 b1 = _Module.freeSSHdHandler.GetPath();
		b1+= "DSAKey.cfg";

		int err = keys->Save((SSHKeyTypes)/*DSAKey*/1, (BSTR)b1, var);
		if (!err)
		{
			_Module.freeSSHdHandler.LoadConfig();
			strncpy(_Module.freeSSHdHandler.Config.DSAKeyPath, ((CComBSTR2)b1).ToString(), MAX_PATH);
			_Module.freeSSHdHandler.SaveConfig();
		}

		var.vt = VT_I2;
		var.iVal = 1024;
		keys->Generate((SSHKeyTypes)/*RSAKey*/0, var);

		var.vt = VT_ERROR;
		b1.Empty();
		b1 = _Module.freeSSHdHandler.GetPath();
		b1+= "RSAKey.cfg";
		err = keys->Save((SSHKeyTypes)/*RSAKey*/0, (BSTR)b1, var);
		if (!err)
		{
			_Module.freeSSHdHandler.LoadConfig();
			strncpy(_Module.freeSSHdHandler.Config.RSAKeyPath, ((CComBSTR2)b1).ToString(), MAX_PATH);
			_Module.freeSSHdHandler.SaveConfig();
		}
		ssh->Release();

		return 0;	
	}
    // Are we Service or Local Server
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{E90D9404-F6D6-4AA1-8392-EBA95FEC3CB9}"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    TCHAR szValue[_MAX_PATH];
    DWORD dwLen = _MAX_PATH;
    lRes = key.QueryValue(szValue, _T("LocalService"), &dwLen);

    _Module.m_bService = FALSE;
    if (lRes == ERROR_SUCCESS)
        _Module.m_bService = TRUE;

    _Module.Start();

    // When we get here, the service has been stopped
    return _Module.m_status.dwWin32ExitCode;
}

#define BUFF_SIZE   1024

BOOL IsAdmin(HANDLE hProcess) 
{
    HANDLE          hToken = NULL;
    PSID            pAdminSid = NULL;
    BYTE            buffer[BUFF_SIZE];
    PTOKEN_GROUPS   pGroups = (PTOKEN_GROUPS)buffer; 
    DWORD           dwSize;             // buffer size
    DWORD           i;
    BOOL            bSuccess;
	
    SID_IDENTIFIER_AUTHORITY siaNtAuth = SECURITY_NT_AUTHORITY;
	
	// get token handle
	if ( !OpenProcessToken ( hProcess, TOKEN_QUERY, &hToken ) )
		return false;
	
	bSuccess = GetTokenInformation ( hToken, TokenGroups,  (LPVOID)pGroups, BUFF_SIZE,  &dwSize );
	CloseHandle ( hToken );
	
	if ( !bSuccess )
		return false;
	if ( !AllocateAndInitializeSid ( &siaNtAuth, 2, 
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0,0,0,0,0,0, &pAdminSid ) )
		return false;
	
	bSuccess = FALSE;
	for ( i = 0; (i < pGroups->GroupCount) && !bSuccess; i++ )
	{
		if ( EqualSid ( pAdminSid, pGroups->Groups[i].Sid ) )
			bSuccess = TRUE;
	}
	FreeSid ( pAdminSid );
	
	return bSuccess;
}

BOOL IsLoggedOnUserAdmin()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );
		return FALSE;
	}
	do
	{
		if (!stricmp(pe32.szExeFile, "explorer.exe"))
		{
			BOOL fIsAdmin = FALSE;
			hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
			if( hProcess != NULL )
			{
				fIsAdmin = IsAdmin(hProcess);
				CloseHandle(hProcess);
			}
			CloseHandle( hProcessSnap );
			return fIsAdmin;
		}
		
		
	} while( Process32Next( hProcessSnap, &pe32 ) );
	
	CloseHandle( hProcessSnap );
	return( FALSE );
}

LRESULT CALLBACK MainWindowProc(HWND hwnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CServiceModule *sm = NULL;

	if (uMsg == WM_TASKBARCREATEDMESSAGE)
	{
		sm = (CServiceModule *)GetWindowLong(hwnd, GWL_USERDATA);
		if (sm)
		{
			HDESK   hdeskCurrent;
			HDESK   hdesk;
			HWINSTA hwinstaCurrent;
			HWINSTA hwinsta;

			// 
			// Save the current Window station
			// 
			hwinstaCurrent = GetProcessWindowStation();
			if (hwinstaCurrent == NULL)
			 return FALSE;

			// 
			// Save the current desktop
			// 
			hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
			if (hdeskCurrent == NULL)
			 return FALSE;

			// 
			// Obtain a handle to WinSta0 - service must be running
			// in the LocalSystem account
			// 
			hwinsta = OpenWindowStation("winsta0", FALSE,
									  WINSTA_ACCESSCLIPBOARD   |
									  WINSTA_ACCESSGLOBALATOMS |
									  WINSTA_CREATEDESKTOP     |
									  WINSTA_ENUMDESKTOPS      |
									  WINSTA_ENUMERATE         |
									  WINSTA_EXITWINDOWS       |
									  WINSTA_READATTRIBUTES    |
									  WINSTA_READSCREEN        |
									  WINSTA_WRITEATTRIBUTES);
			if (hwinsta == NULL)
			 return FALSE;

			// 
			// Set the windowstation to be winsta0
			// 
			if (!SetProcessWindowStation(hwinsta))
			 return FALSE;

			// 
			// Get the default desktop on winsta0
			// 
			hdesk = OpenDesktop("default", 0, FALSE,
								DESKTOP_CREATEMENU |
					  DESKTOP_CREATEWINDOW |
								DESKTOP_ENUMERATE    |
								DESKTOP_HOOKCONTROL  |
								DESKTOP_JOURNALPLAYBACK |
								DESKTOP_JOURNALRECORD |
								DESKTOP_READOBJECTS |
								DESKTOP_SWITCHDESKTOP |
								DESKTOP_WRITEOBJECTS);
			if (hdesk == NULL)
			   return FALSE;

			// 
			// Set the desktop to be "default"
			// 
			BOOL succ = FALSE;
			succ = SetThreadDesktop(hdesk);

			sm->m_NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
			sm->m_NotifyIcon.hWnd   = hwnd;
			sm->m_NotifyIcon.uID    = 0;
			sm->m_NotifyIcon.hIcon  = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
			sm->m_NotifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
			sm->m_NotifyIcon.uCallbackMessage = WM_ICON_NOTIFY;
			strcpy(sm->m_NotifyIcon.szTip, "FreeSSHDService");
		


			if (IsLoggedOnUserAdmin())
			{
				Shell_NotifyIcon(NIM_ADD, &sm->m_NotifyIcon);
				WriteLog("Tray icon shown (admin logged in)");
			}
			else
			{
				WriteLog("Tray icon not show (no admin rights)");
				if (!_Module.m_bService)
				{
					MessageBox(NULL, "You don't have administrator rights! freeSSHd will close!", "Security", MB_ICONERROR);
					PostQuitMessage(1);
					return 0;
				}
			}


			// Reset the Window station and desktop
			// 
			if (!SetProcessWindowStation(hwinstaCurrent))
				   return FALSE;

			SetThreadDesktop(hdeskCurrent);
//			if (!)
//			  return FALSE;

			// 
			// Close the windowstation and desktop handles
			// 
			if (!CloseWindowStation(hwinsta))
			  return FALSE;

			if (!CloseDesktop(hdesk))
			  return FALSE;
		}
		
	}
	else
	switch (uMsg) 
	{ 
		case WM_COMMAND:
			
			if (LOWORD(wParam) == ID_POPUP_SETTINGS)
				_Module.freeSSHdHandler.ShowSettingsDialog();

			if (LOWORD(wParam) == ID_POPUP_UNLOAD)
				PostQuitMessage(0);
			
			break;
		case WM_ICON_NOTIFY:
			switch (lParam)
			{
				case WM_LBUTTONDOWN:			
					_Module.freeSSHdHandler.ShowSettingsDialog();
					break;
					
				case WM_RBUTTONDOWN:
					if (1)
					{
						POINT pt;
						HMENU hmnu;
						
						GetCursorPos(&pt);

						hmnu = LoadMenu(_Module.m_hInst,MAKEINTRESOURCE(IDR_MENU));
						
						TrackPopupMenu(GetSubMenu(hmnu,0),NULL,pt.x,pt.y,NULL,hwnd,NULL);
						
						DestroyMenu(hmnu);
					}
					break;
			}
			break;

		case WM_DESTROY: 
			return 0; 


		default: 
			return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	} 
	return 0; 
}

int StartSSH(void)
{
	int success = 0;
	
	m_SSH->put_Port(_Module.freeSSHdHandler.Config.SSHListenPort);
	CComBSTR b = _Module.freeSSHdHandler.Config.SSHListenAddress;
	m_SSH->put_BindIP(b);
	m_SSH->put_Protocol((ProtocolsEnum)3);
	m_SSH->put_Timeout(_Module.freeSSHdHandler.Config.SSHTimeout);
	
	m_SSH->put_TerminalEmulation((TerminalEmulationsEnum)_Module.freeSSHdHandler.Config.SSHNewConsole);
	
	switch (_Module.freeSSHdHandler.Config.SSHCiphers)
	{
		case encAES:
			m_SSH->put_Encryption(encAES);
			break;

		case encAES128:
			m_SSH->put_Encryption(encAES128);
			break;

		case encAES192:
			m_SSH->put_Encryption(encAES192);
			break;

		case encAES256:
			m_SSH->put_Encryption(encAES256);
			break;

		case enc3DES:
			m_SSH->put_Encryption(enc3DES);
			break;

		case encBLOWFISH:
			m_SSH->put_Encryption(encBLOWFISH);
			break;

		case encCAST128:
			m_SSH->put_Encryption(encCAST128);
			break;

		default:
			m_SSH->put_Encryption(encAny);
			break;
	}

	if (HANDLER.Config.SSHPasswordAuth!=1 && _Module.freeSSHdHandler.Config.SSHPublickeyAuth!=1)
		m_SSH->put_Authentication((AuthenticationsEnum) 0);
	else
	if (HANDLER.Config.SSHPasswordAuth!=1)
		m_SSH->put_Authentication((AuthenticationsEnum) 1);
	else
		m_SSH->put_Authentication((AuthenticationsEnum) 2);

	
	if (HANDLER.Config.SSHBanner && *HANDLER.Config.SSHBanner)
	{
		ifstream ifBanner;
		CComBSTR a;
		ifBanner.open(HANDLER.Config.SSHBanner, ios_base::in);
		
		
		if (ifBanner.is_open())
		{
			char c[16384];
			
			while (!ifBanner.eof())
			{
				ifBanner.read(c, 16383);
				c[16383] = 0;
				a+= c;
			}
		}
		else
			a = HANDLER.Config.SSHBanner;

		m_SSH->put_BannerText(a);
	}	

	
	if (HANDLER.Config.SSHCMD && *_Module.freeSSHdHandler.Config.SSHCMD)
	{
		CComBSTR a = HANDLER.Config.SSHCMD;
		m_SSH->put_Shell(a);
	}


	CComBSTR brsa = HANDLER.Config.RSAKeyPath;
	CComBSTR bdsa = HANDLER.Config.DSAKeyPath;
	
	VARIANT var;
	var.vt = VT_ERROR;

	CSSHKeyPairPtr keys;
	m_SSH->get_Keys(&keys);
	

	try
	{
		keys->Load((BSTR)brsa, var);
	}
	catch (...)
	{

	}
	
	try
	{
		keys->Load((BSTR)bdsa, var);
	}
	catch (...)
	{
	}
	
	var.vt = VT_ERROR;

#if _ISFREEOPEN
	try
	{
		m_SSH->Start(var);
		WriteLog("SSH server started.");
	}
	catch (_com_error e)
	{
		HRESULT h = e.Error();

		CComBSTR2 t = (BSTR) e.Description();
		char strTmp[10240] = "Error in starting SSH server: ";
		strncat(strTmp, t.ToString(), 10000);
		WriteLog(strTmp);
			
		success = HRESULT_CODE(h);
	}
	
#else

	success = m_SSH->Start(var);
	
	VARIANT_BOOL v = VARIANT_FALSE;
	m_SSH->get_Status(&v);

	if (v)
		WriteLog("SSH server started.");
	else
	{
		char strTmp[10240] = "Error in starting SSH server: ";

		strncat(strTmp, GetErrorString(success), 10000);
		WriteLog(strTmp);
	}
#endif 
	
	UpdateNotifyIcon();

	return success;
}

int ReloadSSH(void) // reloads settings, but only ones that can be changed while server is running
{
	int success = 0;
	
//	m_SSH->put_Port(_Module.freeSSHdHandler.Config.SSHListenPort);
//	CComBSTR b = _Module.freeSSHdHandler.Config.SSHListenAddress;
//	m_SSH->put_BindIP(b);
	m_SSH->put_Protocol((ProtocolsEnum)3);
	m_SSH->put_Timeout(_Module.freeSSHdHandler.Config.SSHTimeout);
	
	switch (_Module.freeSSHdHandler.Config.SSHCiphers)
	{
		case encAES:
			m_SSH->put_Encryption(encAES);
			break;

		case encAES128:
			m_SSH->put_Encryption(encAES128);
			break;

		case encAES192:
			m_SSH->put_Encryption(encAES192);
			break;

		case encAES256:
			m_SSH->put_Encryption(encAES256);
			break;

		case enc3DES:
			m_SSH->put_Encryption(enc3DES);
			break;

		case encBLOWFISH:
			m_SSH->put_Encryption(encBLOWFISH);
			break;

		case encCAST128:
			m_SSH->put_Encryption(encCAST128);
			break;

		default:
			m_SSH->put_Encryption(encAny);
			break;
	}

	if (HANDLER.Config.SSHPasswordAuth!=1 && _Module.freeSSHdHandler.Config.SSHPublickeyAuth!=1)
		m_SSH->put_Authentication((AuthenticationsEnum) 0);
	else
	if (HANDLER.Config.SSHPasswordAuth!=1)
		m_SSH->put_Authentication((AuthenticationsEnum) 1);
	else
		m_SSH->put_Authentication((AuthenticationsEnum) 2);

	
	if (HANDLER.Config.SSHBanner && *HANDLER.Config.SSHBanner)
	{
		ifstream ifBanner;
		CComBSTR a;
		ifBanner.open(HANDLER.Config.SSHBanner, ios_base::in);
		
		
		if (ifBanner.is_open())
		{
			char c[16384];
			
			while (!ifBanner.eof())
			{
				ifBanner.read(c, 16383);
				c[16383] = 0;
				a+= c;
			}
		}
		else
			a = HANDLER.Config.SSHBanner;

		m_SSH->put_BannerText(a);
	}	

	if (HANDLER.Config.SSHCMD && *_Module.freeSSHdHandler.Config.SSHCMD)
	{
		CComBSTR a = HANDLER.Config.SSHCMD;
		m_SSH->put_Shell(a);
	}


//	CComBSTR brsa = HANDLER.Config.RSAKeyPath;
//	CComBSTR bdsa = HANDLER.Config.DSAKeyPath;
	
//	VARIANT var;
//	var.vt = VT_ERROR;

//	m_SSH->m_Keys->Load(brsa, var);
//	m_SSH->m_Keys->Load(bdsa, var);
		
	UpdateNotifyIcon();

	return success;
}

int StartTelnet(void) 
{
	int success = 0;
	m_Telnet->put_Port(_Module.freeSSHdHandler.Config.TelnetListenPort);
	CComBSTR b = _Module.freeSSHdHandler.Config.TelnetListenAddress;
	m_Telnet->put_BindIP(b);
	m_Telnet->put_Protocol((ProtocolsEnum)1);
	m_Telnet->put_Timeout(_Module.freeSSHdHandler.Config.TelnetTimeout);
	m_Telnet->put_TerminalEmulation((TerminalEmulationsEnum)HANDLER.Config.TelnetNewConsole);

	if (HANDLER.Config.TelnetBanner && *HANDLER.Config.TelnetBanner)
	{
		ifstream ifBanner;
		CComBSTR a;
		ifBanner.open(HANDLER.Config.TelnetBanner, ios_base::in);
		
		
		if (ifBanner.is_open())
		{
			char c[16384];
			
			while (!ifBanner.eof())
			{
				ifBanner.read(c, 16383);
				c[16383] = 0;
				a+= c;
			}
		}
		else
			a = HANDLER.Config.TelnetBanner;

		m_Telnet->put_BannerText(a);
	}	

	if (_Module.freeSSHdHandler.Config.TelnetCMD && *_Module.freeSSHdHandler.Config.TelnetCMD)
	{
		CComBSTR a = _Module.freeSSHdHandler.Config.TelnetCMD;
		m_Telnet->put_Shell(a);
	}

	VARIANT var;
	var.vt = VT_ERROR;

#if _ISFREEOPEN
	try
	{
		m_Telnet->Start(var);
		WriteLog("Telnet server started.");
	}
	catch (_com_error e)
	{
		HRESULT h = e.Error();

		CComBSTR2 t = (BSTR) e.Description();
		char strTmp[10240] = "Error in starting telnet server: ";
		strncat(strTmp, GetErrorString(success), 10000);
		WriteLog(strTmp);
		
		success = HRESULT_CODE(h);
	}

#else
	
	success = m_Telnet->Start(var);
	
	VARIANT_BOOL v = VARIANT_FALSE;
	m_Telnet->get_Status(&v);
	
	if (v)
		WriteLog("Telnet server started.");
	else
	{
		char strTmp[10240] = "Error in starting telnet server: ";
		strncat(strTmp, GetErrorString(success), 10000);
		WriteLog(strTmp);
	}
	
#endif

	UpdateNotifyIcon();

	return success;
}

int ReloadTelnet(void) // reloads settings, but only ones that can be changed while server is running
{
	int success = 0;
// 	m_Telnet->put_Port(HANDLER.Config.TelnetListenPort);
// 	CComBSTR b =HANDLER.Config.TelnetListenAddress;
// 	m_Telnet->put_BindIP(b);
	m_Telnet->put_Protocol((ProtocolsEnum)1);
	m_Telnet->put_Timeout(HANDLER.Config.TelnetTimeout);

	if (HANDLER.Config.TelnetBanner && *HANDLER.Config.TelnetBanner)
	{
		ifstream ifBanner;
		CComBSTR a;
		ifBanner.open(HANDLER.Config.TelnetBanner, ios_base::in);
		
		
		if (ifBanner.is_open())
		{
			char c[16384];
			
			while (!ifBanner.eof())
			{
				ifBanner.read(c, 16383);
				c[16383] = 0;
				a+= c;
			}
		}
		else
			a = HANDLER.Config.TelnetBanner;

		m_Telnet->put_BannerText(a);
	}	

	if (HANDLER.Config.TelnetCMD && *HANDLER.Config.TelnetCMD)
	{
		CComBSTR a = _Module.freeSSHdHandler.Config.TelnetCMD;
		m_Telnet->put_Shell(a);
	}

	UpdateNotifyIcon();

	return success;
}

BOOL CServiceModule::StartServers(void)
{

	_Module.freeSSHdHandler.LoadConfig();

#ifdef _ISFREEEXE
	m_Telnet = new CwodSSHDCom();
	m_SSH = new CwodSSHDCom();

	memset(&m_TelnetEvents, 0, sizeof(m_TelnetEvents));
	memset(&m_SSHEvents, 0, sizeof(m_SSHEvents));

	m_TelnetEvents.Connecting = Telnet_Connecting;
	m_TelnetEvents.Disconnected = Telnet_Disconnected;
	m_TelnetEvents.LoginPassword = Telnet_LoginPassword;
	m_TelnetEvents.Received = Telnet_Received;
	m_TelnetEvents.ServiceRequest = Telnet_ServiceRequest;
	m_TelnetEvents.ServiceStart = Telnet_ServiceStart;
	m_TelnetEvents.StateChanged = Telnet_StateChanged;

	m_SSHEvents.Connecting = SSH_Connecting;
	m_SSHEvents.CryptoInformation = SSH_CryptoInformation;
	m_SSHEvents.Disconnected = SSH_Disconnected;
	m_SSHEvents.LoginPassword = SSH_LoginPassword;
	m_SSHEvents.LoginPubkey = SSH_LoginPubkey;
	m_SSHEvents.PortBindRequest = SSH_PortBindRequest;
	m_SSHEvents.PortForwardConnect = SSH_PortForwardConnect;
	m_SSHEvents.PortForwardDisconnect = SSH_PortForwardDisconnect;
	m_SSHEvents.PortForwardRequest = SSH_PortForwardRequest;
	m_SSHEvents.Received = SSH_Received;
	m_SSHEvents.ServiceRequest = SSH_ServiceRequest;
	m_SSHEvents.ServiceStart = SSH_ServiceStart;
	m_SSHEvents.SftpDeleteFile = SSH_SftpDeleteFile;
	m_SSHEvents.SftpDownloadFile = SSH_SftpDownloadFile;
	m_SSHEvents.SftpListDir = SSH_SftpListDir;
	m_SSHEvents.SftpMakeDir = SSH_SftpMakeDir;
	m_SSHEvents.SftpRemoveDir = SSH_SftpRemoveDir;
	m_SSHEvents.SftpRename = SSH_SftpRename;
	m_SSHEvents.SftpUploadFile = SSH_SftpUploadFile;
	m_SSHEvents.StateChanged = SSH_StateChanged;

	m_Telnet->m_Events = &m_TelnetEvents;
	m_SSH->m_Events = &m_SSHEvents;
	
	m_Telnet->AddRef();
	m_SSH->AddRef();

#endif

#ifdef _ISFREEOPEN

	HRESULT hr;
	
	hr = m_SSH.CreateInstance(wodSSHDComLIB::CLSID_wodSSHDCom, NULL);
	
	if (FAILED(hr))
	{
		MessageBox(NULL,"Failed to initialize wodSSHServer component!\r\nfreeSSHd will now close.","wodSSHServer component not found!",MB_ICONERROR);
		PostQuitMessage(1);
		return FALSE;
	}
	else
		m_SSH.AddRef();

	hr = m_Telnet.CreateInstance(wodSSHDComLIB::CLSID_wodSSHDCom, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL,"Failed to initialize wodSSHServer component!","wodSSHServer component not found!",MB_OK);
		PostQuitMessage(1);
		return FALSE;
	}
	else
		m_Telnet.AddRef();

	m_SSHEvents = new SSHEvents(m_SSH, this);
	m_TelnetEvents = new TelnetEvents(m_Telnet, this);

#endif


	
	if (freeSSHdHandler.Config.TelnetRun)
		StartTelnet();
	
	if (freeSSHdHandler.Config.SSHRun)
		StartSSH();


    WNDCLASS wndclass;

	memset(&wndclass, 0, sizeof(wndclass));
	wndclass.lpfnWndProc = MainWindowProc;
	wndclass.hInstance   = _Module.m_hInst;
	wndclass.lpszClassName = "FreeSSHDService";
	wndclass.cbWndExtra  = 4;
	RegisterClass(&wndclass);

	m_hWnd = CreateWindow(wndclass.lpszClassName,wndclass.lpszClassName,WS_POPUP,0, 0, 0, 0,NULL,NULL,_Module.m_hInst,NULL);
	if(m_hWnd) 
	{
		SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

		m_NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
		m_NotifyIcon.hWnd   = m_hWnd;
		m_NotifyIcon.uID    = 0;
		m_NotifyIcon.hIcon  = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
		m_NotifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_NotifyIcon.uCallbackMessage = WM_ICON_NOTIFY;
		strcpy(m_NotifyIcon.szTip, "FreeSSHDService");
		
		WM_TASKBARCREATEDMESSAGE = ::RegisterWindowMessage(_T("TaskbarCreated"));
		::PostMessage(m_hWnd, WM_TASKBARCREATEDMESSAGE, 0, 0);
	}


	return TRUE;
}

BOOL CServiceModule::StopServers(void)
{
	if (m_Telnet)
	{
		WriteLog("Telnet server stopped.");
#ifdef _ISFREEEXE
		m_Telnet->m_Events = NULL;
#endif	
		m_Telnet->Stop();

#ifdef _ISFREEEXE
	m_Telnet->Release();
#else
	m_Telnet.Release();
#endif	
	
	}
	if (m_SSH)
	{
		WriteLog("SSH server stopped.");
#ifdef _ISFREEEXE	
		m_SSH->m_Events = NULL;
#endif	
		m_SSH->Stop();
#ifdef _ISFREEEXE
	m_SSH->Release();
#else
	m_SSH.Release();
#endif	
	}



	return TRUE;	
}

/*
void Connecting(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action);
void LoginPassword(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
void Disconnected(CwodSSHDComPtr Owner, CSSHUser *User);
void StateChanged(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState);
void Received(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount);
void ServiceRequest(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
void ServiceStart(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
void SftpListDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpDownloadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpUploadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpRemoveDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpDeleteFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpMakeDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
void SftpRename(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
void PortBindRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
void PortForwardRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
void PortForwardConnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
void LoginPubkey(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
void CryptoInformation(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);
*/

void CServiceModule::Telnet_Connecting(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action)
{
	BOOL ValidIp = FALSE;
	CComBSTR2 RemoteIp;

	CSSHUsers *users;
	Owner->get_Users(&users);
	short iUserCount;

	users->get_Count(&iUserCount);
	if ((iUserCount > _Module.freeSSHdHandler.Config.TelnetMaxConnections) && (_Module.freeSSHdHandler.Config.TelnetMaxConnections != 0))
	{
		WriteUserLog(User, "too many Telnet connections.");
		*Action = (SSHActions) 0; //deny
		return;
	}

	User->get_RemoteIP(&RemoteIp);
	
	ValidIp = _Module.freeSSHdHandler.CheckHostRestriction(RemoteIp.ToString());

	if (ValidIp)
	{
		WriteConnectionLog(User,1,connecting);
		*Action = (SSHActions) 1;	
	}
	else
	{
		WriteConnectionLog(User,0,host_restriction);
		*Action = (SSHActions) 0;

	}
}

void CServiceModule::Telnet_LoginPassword(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action)
{
	*Action = (SSHActions) 0; // Default value
	
	CComBSTR2 bstr2Login = Login;
	CfreeSSHdUser * freeUser;

	freeUser = HANDLER.GetUser(bstr2Login.ToString());
		
	if (freeUser)
	{

		if (freeUser->HasShellAccess() == TRUE)
		{
			if (freeUser->GetAuthType() == 0)	// SHA1 password hash
			{
				CComBSTR2 bstr2Domain = freeUser->GetDomain();
				User->put_Domain(bstr2Domain);
			
				User->put_UseNTAuthentication(VARIANT_TRUE);
				*Action = (SSHActions) 1;
			}
			if (freeUser->GetAuthType() == 1)	// NT authentication
			{
				CComBSTR2 bstr2Password = Password;

				if (freeUser->AuthPassword(bstr2Password.ToString()))
				{	
					*Action = (SSHActions) 1;
					WriteConnectionLog(User, 1, success);
				}
				else
				{
					*Action = (SSHActions) 0;	
					WriteConnectionLog(User, 1, bad_pass);
				}
			}
			if (freeUser->GetAuthType() == 2)	// Public key
			{
				WriteUserLog(User,"public key authentication only (not supported by Telnet).");
				*Action = (SSHActions) 0;	
			}
		}
		else
		{
			WriteUserLog(User," doesn't have shell access.");
			*Action = (SSHActions) 0;
		}
	}
	else
		WriteConnectionLog(User, 1, user_unknown);
}

void CServiceModule::Telnet_Disconnected(CwodSSHDComPtr Owner, CSSHUser *User)
{
	if (HANDLER.m_pgPageStatus && HANDLER.m_pgPageStatus->IsWindow())
		HANDLER.m_pgPageStatus->UpdateTelnetInfo(TRUE);
	
	if (HANDLER.m_pgPageOnlineUsers && HANDLER.m_pgPageOnlineUsers->IsWindow())
		HANDLER.m_pgPageOnlineUsers->RemoveConnection(User);
	
	WriteConnectionLog(User,1,disconnect);

}
void CServiceModule::Telnet_StateChanged(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState)
{
	// WriteLog("STATE CHANGED");
}
void CServiceModule::Telnet_Received(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount){}
void CServiceModule::Telnet_ServiceRequest(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action)
{
	// dirty, dirty hack :) 
	// i'm using this event just to know that user has successfully connected.
	WriteConnectionLog(User,1,success);

	if (HANDLER.m_pgPageStatus && HANDLER.m_pgPageStatus->IsWindow())
		HANDLER.m_pgPageStatus->UpdateTelnetInfo();

	if (HANDLER.m_pgPageOnlineUsers && HANDLER.m_pgPageOnlineUsers->IsWindow())
		HANDLER.m_pgPageOnlineUsers->AddTelnetConnection(User);
	
}
void CServiceModule::Telnet_ServiceStart(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName){}
void CServiceModule::Telnet_SftpListDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpDownloadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpUploadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpRemoveDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpDeleteFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpMakeDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action){}
void CServiceModule::Telnet_SftpRename(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action){}
void CServiceModule::Telnet_PortBindRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action){}
void CServiceModule::Telnet_PortForwardRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action){}
void CServiceModule::Telnet_PortForwardConnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action){}
void CServiceModule::Telnet_LoginPubkey(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action){}
void CServiceModule::Telnet_CryptoInformation(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept){}

void CServiceModule::SSH_Connecting(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action)
{
	BOOL ValidIp = FALSE;
	CComBSTR2 RemoteIp;
	
	CSSHUsers *users;
	Owner->get_Users(&users);
	short iUserCount;

	users->get_Count(&iUserCount);

	if ((iUserCount + 1 > _Module.freeSSHdHandler.Config.SSHMaxConnections) && (_Module.freeSSHdHandler.Config.SSHMaxConnections != 0)) 
	{
		*Action = (SSHActions) 0; // To many connections
		WriteUserLog(User, "too many SSH connections.");
		return;
	}
	
	User->get_RemoteIP(&RemoteIp);
	
	ValidIp = _Module.freeSSHdHandler.CheckHostRestriction(RemoteIp.ToString());

	if (ValidIp)
	{
		VARIANT vTag;
				
		vTag.vt = VT_I2;
		vTag.iVal = 0;

		User->put_Tag(vTag); // no authentication done

		WriteConnectionLog(User,0,connecting);
		*Action = (SSHActions) 1;
	}
	else
	{
		WriteConnectionLog(User,0,host_restriction);
		*Action = (SSHActions) 0;

	}
}

void CServiceModule::SSH_LoginPassword(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action)
{
	*Action = (SSHActions) 0;
	BOOL fOK = FALSE;

	CfreeSSHdUser * freeUser;
	CComBSTR2 bstr2Login = Login, bstr2Password = Password;

	if (_Module.freeSSHdHandler.Config.SSHPasswordAuth != 1)	// Checking if password authentication is allowed
	{
		freeUser = _Module.freeSSHdHandler.GetUser(bstr2Login.ToString());
		
		if (freeUser)
		{
			if (freeUser->GetAuthType() == 2) // Public key
			{
				WriteUserLog(User,"public key authentication only.");
				*Action = (SSHActions) 0;
				return;
			}
			if (freeUser->GetAuthType() == 0) // NT auth
			{
				
				CComBSTR2 bstr2Domain = freeUser->GetDomain();

#ifdef _ISFREEEXE
				if (User->m_ImpersonateToken != (INVALID_HANDLE_VALUE))
					CloseHandle(User->m_ImpersonateToken);

				User->m_ImpersonateToken = INVALID_HANDLE_VALUE;
	
				fOK = LogonUser(bstr2Login.ToString(),bstr2Domain.ToString(),bstr2Password.ToString(), LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT,&User->m_ImpersonateToken);
#endif
#ifdef _ISFREEOPEN
				HANDLE hImpersonateToken = INVALID_HANDLE_VALUE;

				fOK = LogonUser(bstr2Login.ToString(),bstr2Domain.ToString(),bstr2Password.ToString(), LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT,&hImpersonateToken);
#endif	

				if (_Module.m_bService)
				{
					User->put_Domain(bstr2Domain);
					User->put_UseNTAuthentication(VARIANT_TRUE);
				}

			}
			else
				fOK = freeUser->AuthPassword(bstr2Password.ToString()); // SHA1 auth

			if (fOK) // password OK
			{
				char dir[MAX_PATH+2] = "";
				CComBSTR bstrHomeDir = freeUser->GetHomeDir(dir);

				User->put_HomeDir(bstrHomeDir);
				HANDLER.User_put_DidPassword(User);

				if ((HANDLER.Config.SSHPublickeyAuth == 2) && (HANDLER.User_get_DidPubkey(User)) == FALSE) // pass & key both required
				{
					WriteUserLog(User,"public key authentication required.");
					*Action = (SSHActions) 0;
					return;
				}
				else
				{
					int auth_method = 0;
			
						if (HANDLER.User_get_DidPubkey(User))
							auth_method = 2;

					WriteConnectionLog(User,0,success, auth_method);				

					*Action = (SSHActions) 1;
					
					if (HANDLER.m_pgPageStatus && HANDLER.m_pgPageStatus->IsWindow())
						HANDLER.m_pgPageStatus->UpdateSSHInfo();

					return;
				}
			}
			else
				WriteConnectionLog(User, 0, bad_pass);
		}
		else
			WriteConnectionLog(User, 0, user_unknown);
		
	}
	else
		WriteUserLog(User, "password authentication is disabled.");
}

void CServiceModule::SSH_Disconnected(CwodSSHDComPtr Owner, CSSHUser *User)
{
	WriteConnectionLog(User,0,disconnect);
	
	if (HANDLER.m_pgPageStatus && HANDLER.m_pgPageStatus->IsWindow())
		HANDLER.m_pgPageStatus->UpdateSSHInfo(TRUE);

	if (HANDLER.m_pgPageOnlineUsers && HANDLER.m_pgPageOnlineUsers->IsWindow())
		HANDLER.m_pgPageOnlineUsers->RemoveConnection(User);
}

void CServiceModule::SSH_StateChanged(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState){}
void CServiceModule::SSH_Received(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount){}
void CServiceModule::SSH_ServiceRequest(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action)
{
	
	*Action = (SSHActions) 0;


	CComBSTR2 bstr2Login;
	User->get_Login(&bstr2Login);

	CfreeSSHdUser *freeUser = _Module.freeSSHdHandler.GetUser(bstr2Login.ToString());
	
	if (freeUser)
	{
		if (*ServiceType == (SSHServiceTypes) 1 || *ServiceType == (SSHServiceTypes) 2) 
		{
			if (freeUser->HasShellAccess())
			{
				*Action = (SSHActions) 1;
				WriteServiceLog(User, shell, 0);

				if (HANDLER.m_pgPageOnlineUsers && HANDLER.m_pgPageOnlineUsers->IsWindow())
					HANDLER.m_pgPageOnlineUsers->AddSSHConnection(User);
				
				return;
			}
			else
			{
				*ServiceType = (SSHServiceTypes) 0;
				WriteServiceLog(User, shell, 1);
				*Action = (SSHActions) 1;
				return;
			}
		}
		if ((*ServiceType == (SSHServiceTypes) 3) || (	*ServiceType == (SSHServiceTypes) 5))
		{
			if (freeUser->HasSFTPAccess())
			{
				*Action = (SSHActions) 1;
				WriteServiceLog(User, sftp, 0);

				if (HANDLER.m_pgPageOnlineUsers && HANDLER.m_pgPageOnlineUsers->IsWindow())
					HANDLER.m_pgPageOnlineUsers->AddSSHConnection(User);
				
				return;
			}
			else
			{
				*Action = (SSHActions) 0;
				WriteServiceLog(User,sftp,1);
				return;
			}
		}
	}
	else
		WriteConnectionLog(User, 0, user_unknown);
		
}
void CServiceModule::SSH_ServiceStart(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName)
{
	if (ServiceType == (SSHServiceTypes) 0)
	{
		_bstr_t bstrText = "This service is prohibited.";
		VARIANT vt;
		vt.vt =  VT_ERROR;

		User->Send(bstrText, vt);
	}
}
void CServiceModule::SSH_SftpListDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is listing ");

	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";

	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpDownloadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is downloading ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpUploadFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is uploading ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpRemoveDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is removing ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpDeleteFile(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is deleting ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpMakeDir(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	_bstr_t t("is making ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ")";
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_SftpRename(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action)
{
	_bstr_t t("is renaming ");
	
	t += RelativePath;
	t += " (";
	t += *ResolvedPath;
	t += ") to ";
	t += *NewName;
	
	WriteUserLog(User, (char *) t);
}
void CServiceModule::SSH_PortBindRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action)
{
	*Action = (SSHActions) 0;

	if (!_Module.freeSSHdHandler.Config.SSHRemoteTunnel)
	{
		WriteUserLog(User, "remote port forwarding is disabled.");
		*Action = (SSHActions) 0;
		return;
	}

	CComBSTR2 bstr2IP = *BindIP;

	if (_Module.freeSSHdHandler.Config.SSHRemoteTunnelOnly)
		if (strncmp(bstr2IP.ToString(), "127.0.0.1", 10))
		{	
			WriteUserLog(User, "remote port forwarding is limited to localhost.");
			*Action = (SSHActions) 0;
			return;
		}

	CComBSTR2 bstr2Login;
	 User->get_Login(&bstr2Login);

	CfreeSSHdUser *freeUser = _Module.freeSSHdHandler.GetUser(bstr2Login.ToString());
	
	if (freeUser)
	{
		if (freeUser->HasTunnelAccess())
		{
			WriteServiceLog(User, tunnel, 0);
			*Action = (SSHActions) 1;
			return;
		}
		else
			WriteServiceLog(User,tunnel,1);
	}
	else
		WriteConnectionLog(User, 0, user_unknown);
	
}
void CServiceModule::SSH_PortForwardRequest(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action)
{
	*Action = (SSHActions) 0;

	if (!_Module.freeSSHdHandler.Config.SSHLocalTunnel)
	{
		WriteUserLog(User, "local port forwarding is disabled.");
		*Action = (SSHActions) 0;
		return;
	}

	CComBSTR2 bstr2Host = *RemoteHost;

	if (_Module.freeSSHdHandler.Config.SSHLocalTunnelOnly)
		if (strncmp(bstr2Host.ToString(), "127.0.0.1", 10))
		{	
			WriteUserLog(User, "local port forwarding is limited to localhost.");
			*Action = (SSHActions) 0;
			return;
		}

	CComBSTR2 bstr2Login;
	User->get_Login(&bstr2Login);

	CfreeSSHdUser *freeUser = _Module.freeSSHdHandler.GetUser(bstr2Login.ToString());
	
	if (freeUser)
	{
		if (freeUser->HasTunnelAccess())
		{
			WriteServiceLog(User, tunnel, 0);
			*Action = (SSHActions) 1;
			return;
		}
		else
			WriteServiceLog(User,tunnel,1);
	}
	else
		WriteConnectionLog(User, 0, user_unknown);
}

void CServiceModule::SSH_PortForwardConnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action){};
void CServiceModule::SSH_PortForwardDisconnect(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort){};
void CServiceModule::SSH_LoginPubkey(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action)
{
	*Action = (SSHActions) 0;

	if (HANDLER.Config.SSHPublickeyAuth != 1)
	{
			CComBSTR2 bstr2Login = Login;
			CfreeSSHdUser * freeUser = _Module.freeSSHdHandler.GetUser(bstr2Login.ToString());
			
			if (freeUser)
			{
				BOOL fOK = FALSE;
				CComBSTR2 bstr2PubKey = PublicKey;

				fOK = freeUser->AuthPublicKey(bstr2PubKey.ToString());
				
				if (fOK)
				{			
					char dir[MAX_PATH+2] = "";
					CComBSTR bstrHomeDir = freeUser->GetHomeDir(dir);

					HANDLER.User_put_DidPubkey(User);
					User->put_HomeDir(bstrHomeDir);
					
					if ((_Module.freeSSHdHandler.Config.SSHPasswordAuth == 2) && (HANDLER.User_get_DidPassword(User) == FALSE)) // pass & key both required
					{
						WriteUserLog(User,"password authentication required.");
						*Action = (SSHActions) 0;
						return;
					}
					else
					{
						int auth_method = 1;
			
						if (HANDLER.User_get_DidPassword(User))
							auth_method = 2;

						WriteConnectionLog(User,0,success, auth_method);			
						
						*Action = (SSHActions) 1;
						
						if (HANDLER.m_pgPageStatus && HANDLER.m_pgPageStatus->IsWindow())
							HANDLER.m_pgPageStatus->UpdateSSHInfo();
						
						return;
					}
				}
				else
					WriteConnectionLog(User, 0, bad_key);

			}
			else
				WriteConnectionLog(User, 0, user_unknown);			
	}
	else
		WriteUserLog(User, "public key authentication is denied.");
}

void CServiceModule::SSH_CryptoInformation(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept){}
