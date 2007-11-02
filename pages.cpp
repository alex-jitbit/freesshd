#include "stdafx.h"
#include "maindlg.h"
#include "user.h"
#include "userpages.h"
#include "pages.h"
#include "gdiu.h"
#include <shlobj.h>
#include <string>

#ifdef _ISFREEEXE
#include "../../Component/wodSSHDCom.h"
#include "../../Component/SSHUsers.h"
#include "../../Component/SSHUser.h"
#include "../../Component/cwodwinsocket.h"
#endif

#include "buffer.h"
#include "ComBSTR2.h"


using namespace std;

#include "log.h"

#ifdef _ISFREEOPEN

	#ifndef WSA_IO_PENDING
	#define WSA_IO_PENDING          (ERROR_IO_PENDING)
	#endif

	#ifndef WSA_IO_INCOMPLETE
	#define WSA_IO_INCOMPLETE       (ERROR_IO_INCOMPLETE)
	#endif

	#ifndef WSA_INVALID_HANDLE
	#define WSA_INVALID_HANDLE      (ERROR_INVALID_HANDLE)
	#endif

	#ifndef WSA_INVALID_PARAMETER
	#define WSA_INVALID_PARAMETER   (ERROR_INVALID_PARAMETER)
	#endif

	#ifndef WSA_NOT_ENOUGH_MEMORY
	#define WSA_NOT_ENOUGH_MEMORY   (ERROR_NOT_ENOUGH_MEMORY)
	#endif

	#ifndef WSA_OPERATION_ABORTED
	#define WSA_OPERATION_ABORTED   (ERROR_OPERATION_ABORTED)
	#endif

	typedef struct ErrTable
	{
		int code;
		LPSTR msg;
	}
	ErrTable, FAR * LPErrTable;

	ErrTable ErrLookupTable[] =
	{
		{ 0, "No error." },
		{ 13, "Type mismatch." },
		{ 380, "Invalid Property Value." },
		{ WSAEINTR, "System level interrupt interrupted socket operation." },
		{ WSAEBADF, "Generic error for invalid format, bad format." },
		{ WSAEACCES, "Generic error for access violation." },
		{ WSAEFAULT, "Generic error for fault." },
		{ WSAEINVAL, "Generic error for invalid format, entry, etc." },
		{ WSAEMFILE, "Generic error for file error." },
		{ 10025, "The IP address provided is not valid or the host specified by the IP does not exist." },
		{ WSAENOTSOCK, "Invalid socket or not connected to remote." },
		{ WSAEADDRINUSE, "The specified address is already in use." },
		{ WSAEADDRNOTAVAIL, "The specified address is not available." },
		{ WSAENETDOWN, "The connected network is not available." },
		{ WSAENETUNREACH, "The connected network is not reachable." },
		{ WSAENETRESET, "The connected network connection has been reset." },
		{ WSAECONNABORTED, "The current connection has been aborted by the network or intermediate services." },
		{ WSAECONNRESET, "The current socket connection has been reset." },
		{ WSAENOTCONN, "The current socket has not been connected." },
		{ WSAESHUTDOWN, "The connection has been shutdown." },
		{ WSAETIMEDOUT, "The current connection has timed-out." },
		{ WSAECONNREFUSED, "The requested connection has been refused by the remote host." },
		{ WSAENAMETOOLONG, "Specified host name is too long." },
		{ WSAEHOSTDOWN, "Remote host is currently unavailable." },
		{ WSAEHOSTUNREACH, "Remote host is currently unreachable. " },
		{ WSASYSNOTREADY, "Remote system is not ready." },
		{ WSAVERNOTSUPPORTED, "Current socket version not supported by application." },
		{ WSANOTINITIALISED, "Socket API is not initialized." },
		{ WSAEDISCON, "Socket has been disconnected." },
		{ WSAENOPROTOOPT, "Bad protocol option." },
		{ WSAHOST_NOT_FOUND , "Host not found." },
		{ WSAEAFNOSUPPORT , "Address family not supported by protocol family." },
		{ WSAEALREADY , "Operation already in progress." },
		{ WSAEDESTADDRREQ , "Destination address required."},
		{ WSAEINPROGRESS , "Operation now in progress."},
		{ WSAEISCONN , "Socket is already connected."},
		{ WSAEMSGSIZE , "Message too long."},
		{ WSAENOBUFS , "No buffer space available."},
		{ WSAEOPNOTSUPP , "Operation not supported."},
		{ WSAEPFNOSUPPORT , "Protocol family not supported."},
		{ WSAEPROCLIM , "Too many processes."},
		{ WSAEPROTONOSUPPORT , "Protocol not supported."},
		{ WSAEPROTOTYPE , "Protocol wrong type for socket."},
		{ WSAESOCKTNOSUPPORT , "Socket type not supported."},
		// { WSATYPE_NOT_FOUND, "Class type not found."},
		{ WSA_INVALID_HANDLE , "Specified event object handle is invalid."},
		{ WSA_INVALID_PARAMETER , "One or more parameters are invalid."},
		//	{ WSAINVALIDPROCTABLE , "Invalid procedure table from service provider."},
		//	{ WSAINVALIDPROVIDER , "Invalid service provider version number."},
		{ WSA_IO_INCOMPLETE , "Overlapped I/O event object not in signaled state."},
		{ WSA_IO_PENDING , "Overlapped operations will complete later."},
		{ WSA_NOT_ENOUGH_MEMORY , "Insufficient memory available."},
		{ WSANO_DATA , "Valid name, no data record of requested type."},
		{ WSANO_RECOVERY , "This is a non-recoverable error."},
		//	{ WSAPROVIDERFAILEDINIT ,"Unable to initialize a service provider."},
		// { WSASYSCALLFAILURE , "System call failure."},
		{ WSATRY_AGAIN , "Non-authoritative host not found."},
		{ WSA_OPERATION_ABORTED , "Overlapped operation aborted."},
		{ WSAEWOULDBLOCK, "Resource temporary unavailable."},
		{ 30000, "Password too short. It should be at least 4 bytes." },
		{ 30001, "Failed to create file on disk." },
		{ 30002, "Invalid key type."},
		{ 30003, "Failed to write key."},
		{ 30004, "Failed to generate new key."},
		{ 30005, "Failed to open key file."},
		{ 30006, "Failed to import key from file."},
		{ 30007, "Unknown key format."},
		{ 30008, "Invalid service index."},
		{ 30009, "Invalid data format."},
		{ 30010, "Keys not loaded or generated."},
		{ 30011, "Cannot change protocol at this time. Stop server first."},
		{ 39999, "License key missing. You can not use this component in design environment." },
		{ -1, "Unrecognized Error." }
	};

	LPSTR GetErrorString(int err)
	{
		LPErrTable p = ErrLookupTable;
		
	//	m_LastError = err;
		
		while (p->code != -1)
		{
			if (p->code == err)
				return p->msg;
			p++;
		}
		return p->msg;
	}

#endif 
#define HANDLER _Module.freeSSHdHandler

int CancelPrompt()
{
	if (::IsWindowEnabled(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW)))
		return MessageBox(_Module.freeSSHdHandler.m_Sheet->m_hWnd, "Do you want to close this window and lose all changes?", "freeSSHd settings", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2);
	else
		return IDYES;
}

bool GetFolder(std::string& folderpath, 
               const char* szCaption = NULL, 
               HWND hOwner = NULL)
{
   bool retVal = false;

   // The BROWSEINFO struct tells the shell 
   // how it should display the dialog.
   BROWSEINFO bi;
   memset(&bi, 0, sizeof(bi));

   bi.ulFlags   = BIF_USENEWUI;
   bi.hwndOwner = hOwner;
   bi.lpszTitle = szCaption;

   // must call this if using BIF_USENEWUI
   ::OleInitialize(NULL);

   // Show the dialog and get the itemIDList for the 
   // selected folder.
   LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

   if(pIDL != NULL)
   {
      // Create a buffer to store the path, then 
      // get the path.
      char buffer[_MAX_PATH] = {'\0'};
      if(::SHGetPathFromIDList(pIDL, buffer) != 0)
      {
         // Set the string value.
         folderpath = buffer;
         retVal = true;
      }

      // free the item id list
      CoTaskMemFree(pIDL);
   }

   ::OleUninitialize();

   return retVal;
}


Buffer *GetMyIPs(void)
{
	WSADATA wsadata;
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSAStartup(MAKEWORD(2, 0), &wsadata);
	
	char     hostname[100];
	HOSTENT *pHostEnt;
	int      nAdapter = 0;
	struct   sockaddr_in sAddr;

	gethostname(hostname, sizeof(hostname) );
	pHostEnt = gethostbyname(hostname);

	Buffer *b = new Buffer();

	if (pHostEnt != NULL) 
	{
		while ( pHostEnt->h_addr_list[nAdapter] )
		{
			memcpy ( &sAddr.sin_addr.s_addr, pHostEnt->h_addr_list[nAdapter],pHostEnt->h_length);
			b->PutCString(inet_ntoa(sAddr.sin_addr));
			nAdapter++;
		}	
	}
	b->PutCString("127.0.0.1 (Only localhost)");

	WSACleanup();
	return b;
}

// CPageStatus
LRESULT CPageStatus::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SetMsgHandled(FALSE);

	m_TelnetStartStop.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON);
	m_SSHStartStop.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON);

	m_hiconOK = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_OK));
	m_hiconBAD = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_BAD));
	m_hbmpLogo = LoadBitmap(_Module.m_hInst, MAKEINTRESOURCE(IDB_WEONLYDO));

	UpdateTelnetInfo();
	UpdateSSHInfo();
	
	return TRUE;
}

LRESULT CPageStatus::OnTelnetStatusLineClicked(WORD Code, WORD Control, HWND hWnd)
{
	if (Control == IDC_STATUS_TELNET_LINE2)
	{
		m_TelnetStartStop.UnsubclassWindow();
		::InvalidateRect(GetDlgItem(IDC_STATUS_TELNET_LINE2), NULL, TRUE);
		
		if (HANDLER.Telnet_Stop())
		{
			WriteLog("Telnet server stopped.");
			UpdateNotifyIcon();
		}
	}
	else
	{
		int iErr = 0;
		m_TelnetStartStop.UnsubclassWindow();
		::InvalidateRect(GetDlgItem(IDC_STATUS_TELNET_LINE1), NULL, TRUE);
		
		iErr = HANDLER.Telnet_Start();

		if (iErr)
		{
			if (VisualStyles())
			{		
				WCHAR wcsErr[1024];
				
   				MultiByteToWideChar( CP_ACP, 0, GetErrorString(iErr), strlen(GetErrorString(iErr))+1, wcsErr, sizeof(wcsErr)/sizeof(wcsErr[0]) );

				ShowBallonError(GetDlgItem(IDC_STATUS_TELNETBALLOON), wcsErr);
			}
			else
				MessageBox(GetErrorString(iErr), "Error starting SSH server", MB_ICONERROR);
		}	
		
	}

	UpdateTelnetInfo();
	
	return TRUE;
}
	
LRESULT CPageStatus::OnSSHStatusLineClicked(WORD Code, WORD Control, HWND hWnd)
{
	if (Control == IDC_STATUS_SSH_LINE2)
	{
		m_SSHStartStop.UnsubclassWindow();
		::InvalidateRect(GetDlgItem(IDC_STATUS_SSH_LINE2), NULL, TRUE);
		
		if (HANDLER.SSH_Stop())
		{
			WriteLog("SSH server stopped.");
			UpdateNotifyIcon();
		};
	}
	else
	{
		int iErr = 0;
		m_SSHStartStop.UnsubclassWindow();
		::InvalidateRect(GetDlgItem(IDC_STATUS_SSH_LINE1), NULL, TRUE);
		
		iErr = HANDLER.SSH_Start();
		
		if (iErr)
		{
			if (VisualStyles())
			{		
				WCHAR wcsErr[1024];
				
   				MultiByteToWideChar( CP_ACP, 0, GetErrorString(iErr), strlen(GetErrorString(iErr))+1, wcsErr, sizeof(wcsErr)/sizeof(wcsErr[0]) );

				ShowBallonError(GetDlgItem(IDC_STATUS_SSHBALLOON), wcsErr);
			}
			else
				MessageBox(GetErrorString(iErr), "Error starting SSH server", MB_ICONERROR);
		}	
		
	}
	
	UpdateSSHInfo();

	return TRUE;
}

BOOL CPageStatus::OnSetActive() 
{
	SendDlgItemMessage(IDC_WEONLYDOLOGO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_hbmpLogo);	
	return TRUE;
}

BOOL CPageStatus::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DeleteObject(m_hbmpLogo);
		DestroyIcon(m_hiconBAD);
		DestroyIcon(m_hiconOK);
		return TRUE;
	}
}

BOOL CPageStatus::OnApply()
{
	return FALSE;
}

BOOL CPageStatus::UpdateTelnetInfo(BOOL Disconnected)
{
	char strTmp[1024];

	if (HANDLER.Telnet_IsActive())
	{
		UINT count = HANDLER.Telnet_UserCount();
		
		if (Disconnected)
		{
			if (count - 1 >= 0)
				count = count - 1;
		}
		
		SetDlgItemText(IDC_STATUS_TELNET, "Telnet server is running.");
		
		if (count)
		{	
			if (count == 1)
				_snprintf(strTmp, 1024, "There is %d user currently online.", count);
			else
				_snprintf(strTmp, 1024, "There are %d users currently online.", count);

			strTmp[1023] = 0;
		}
		else
			strcpy(strTmp, "There are no users currently online.");

		SetDlgItemText(IDC_STATUS_TELNET_LINE1, strTmp);
		
		if (m_TelnetStartStop.IsWindow())
			m_TelnetStartStop.UnsubclassWindow();

		m_TelnetStartStop.SubclassWindow(GetDlgItem(IDC_STATUS_TELNET_LINE2));
		m_TelnetStartStop.SetLabel("Click here to stop it.");
		::InvalidateRect(GetDlgItem(IDC_STATUS_TELNET_LINE2), NULL, TRUE);
		SendDlgItemMessage(IDC_TELNET_PIC, STM_SETICON, (WPARAM) m_hiconOK, 0);
		
	}
	else
	{
		SetDlgItemText(IDC_STATUS_TELNET, "Telnet server is not running.");
		SetDlgItemText(IDC_STATUS_TELNET_LINE2, "");
	
		if (m_TelnetStartStop.IsWindow())
			m_TelnetStartStop.UnsubclassWindow();
		
		m_TelnetStartStop.SubclassWindow(GetDlgItem(IDC_STATUS_TELNET_LINE1));
		m_TelnetStartStop.SetLabel("Click here to start it.");

		::InvalidateRect(GetDlgItem(IDC_STATUS_TELNET_LINE1), NULL, TRUE);
		SendDlgItemMessage(IDC_TELNET_PIC, STM_SETICON, (WPARAM) m_hiconBAD, 0);
	}

	return TRUE;
}

BOOL CPageStatus::UpdateSSHInfo(BOOL Disconnected)
{
	char strTmp[1024];

	if (HANDLER.SSH_IsActive())
	{
		UINT count = HANDLER.SSH_UserCount();
		
		if (Disconnected)
		{
			if (count - 1 >= 0)
				count = count - 1;
		}

		SetDlgItemText(IDC_STATUS_SSH, "SSH server is running.");
		
		if (count)
		{	
			if (count == 1)
				_snprintf(strTmp, 1024, "There is %d user currently online.", count);
			else
				_snprintf(strTmp, 1024, "There are %d users currently online.", count);

			strTmp[1023] = 0;
		}
		else
			strcpy(strTmp, "There are no users currently online.");

		SetDlgItemText(IDC_STATUS_SSH_LINE1, strTmp);
		
		if (m_SSHStartStop.IsWindow())
				m_SSHStartStop.UnsubclassWindow();

		m_SSHStartStop.SubclassWindow(GetDlgItem(IDC_STATUS_SSH_LINE2));
		m_SSHStartStop.SetLabel("Click here to stop it.");
		::InvalidateRect(GetDlgItem(IDC_STATUS_SSH_LINE2), NULL, TRUE);
		SendDlgItemMessage(IDC_SSH_PIC, STM_SETICON, (WPARAM) m_hiconOK, 0);
	}
	else
	{
		SetDlgItemText(IDC_STATUS_SSH, "SSH server is not running.");
		SetDlgItemText(IDC_STATUS_SSH_LINE2, "");

		if (m_SSHStartStop.IsWindow())
				m_SSHStartStop.UnsubclassWindow();

		m_SSHStartStop.SubclassWindow(GetDlgItem(IDC_STATUS_SSH_LINE1));
		m_SSHStartStop.SetLabel("Click here to start it.");

		::InvalidateRect(GetDlgItem(IDC_STATUS_SSH_LINE1), NULL, TRUE);
		SendDlgItemMessage(IDC_SSH_PIC, STM_SETICON, (WPARAM) m_hiconBAD, 0);
	}

	return TRUE;
}
// CPageTelnet

LRESULT CPageTelnet::OnInitDialog(HWND hWnd, LPARAM lParam)
{

	Buffer *b;

	SetMsgHandled(FALSE);
	
	CComboBox m_IpBox = GetDlgItem(IDC_TELNET_LISTENADDRESS);
	
	m_IpBox.ResetContent();
	m_IpBox.AddString("0.0.0.0 (All interfaces)");
	m_IpBox.SetCurSel(0);

	// a little trick to save me trouble writing same thing over and over....
#define CONFIG HANDLER.Config

	int i = 0;
	
	b = GetMyIPs();

	if (b)
	{
		while (b->Len())
		{
			char *a = b->GetString(NULL);
			if (a && *a)
			{	
				m_IpBox.AddString(a);
				
				if (*CONFIG.TelnetListenAddress && !strncmp(a, CONFIG.TelnetListenAddress, strlen(CONFIG.TelnetListenAddress)))
							m_IpBox.SetCurSel(i+1);
				free(a);

						i++;
			}
		}
		delete b;
	}
	
	SetDlgItemInt(IDC_TELNET_PORT,CONFIG.TelnetListenPort, TRUE);
	SetDlgItemInt(IDC_TELNET_MAXCONNECTIONS, CONFIG.TelnetMaxConnections, TRUE);
	SetDlgItemInt(IDC_TELNET_IDLETIMEOUT, CONFIG.TelnetTimeout, TRUE);
	SetDlgItemText(IDC_TELNET_GREETINGMESSAGE, CONFIG.TelnetBanner);
	SetDlgItemText(IDC_TELNET_CMD, CONFIG.TelnetCMD);
	CheckDlgButton(IDC_TELNET_AUTOSTART, CONFIG.TelnetRun);
	CheckDlgButton(IDC_TELNET_NEWCONSOLE, CONFIG.TelnetNewConsole);
	
#undef CONFIG
	
	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_TELNET));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);

	SetMsgHandled(FALSE);
	return TRUE;
}
	
BOOL CPageTelnet::OnSetActive() 
{
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);
	
	return TRUE;
}

BOOL CPageTelnet::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		
		return TRUE;
	}
}

LRESULT CPageTelnet::OnBrowse(WORD Code, WORD Control, HWND hWnd)
{
	int i = 0;

	if (Control == IDC_TELNET_BANNERMESSAGE_BROWSE)
	{
		CFileDialog dlg(TRUE, "*.txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.txt (Text Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your Banner text file";
		
		i = dlg.DoModal();
		
		if (i == 1)
			SetDlgItemText(IDC_TELNET_BANNERMESSAGE, dlg.m_szFileName);
	}
	
	if (Control == IDC_TELNET_CMD_BROWSE)
	{
		CFileDialog dlg(TRUE, "*.exe", "*.exe",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.exe (Executable Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your command shell executable";
		
		i = dlg.DoModal();
		
		if (i == 1)
			SetDlgItemText(IDC_TELNET_CMD, dlg.m_szFileName);	
	}

	return TRUE;
}

LRESULT CPageTelnet::OnChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

// LRESULT CPageTelnet::OnEditSetFocus(WORD Code, WORD Control, HWND hWnd)
// {
// 	CEdit edit = GetDlgItem(Control);
// 	
// 	if (!edit.GetWindowTextLength())
// 	{
// 		if(GetDllVersion(TEXT("comctl32.dll")) >= PACKVERSION(6,0))if(GetDllVersion(TEXT("comctl32.dll")) >= PACKVERSION(6,0))
// 		{
// 			
// 			EDITBALLOONTIP balloon;
// 			LPWSTR pszText = NULL;
// 			LPWSTR pszTitle = L"Suggestion"; 
// 			
// 			switch (Control)
// 			{	
// 			case IDC_TELNET_PORT:
// 				pszText = L"'21' is the standard value.";
// 				break;
// 				
// 			case IDC_TELNET_MAXCONNECTIONS:
// 				pszText = L"Enter '0' for unlimited number of connections."; 
// 				break;
// 				
// 			case IDC_TELNET_IDLETIMEOUT:
// 				pszText = L"Enter '0' for no idle timeout.";
// 				break;
// 			}
// 			
// 			if (pszText == NULL)
// 				return TRUE;
// 			
// 			balloon.cbStruct = sizeof(EDITBALLOONTIP); 
// 			balloon.pszText = pszText; 
// 			balloon.pszTitle = pszTitle; 
// 			balloon.ttiIcon = TTI_NONE; 
// 			
// 			edit.ShowBalloonTip(&balloon);
// 		}
// 		else
// 		{
// 			
// 		}
// 	}
// 	return TRUE;
// }

// CPageSSH

LRESULT CPageSSH::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	Buffer *b;	
	CComboBox m_IpBox = GetDlgItem(IDC_SSH_LISTENADDRESS);
	
	m_IpBox.ResetContent();
	m_IpBox.AddString("0.0.0.0 (All interfaces)");
	m_IpBox.SetCurSel(0);

	// a little trick to save me trouble writing same thing over and over....
#define CONFIG HANDLER.Config

	int i = 0;
	
	b = GetMyIPs();

	if (b)
	{
		while (b->Len())
		{
			char *a = b->GetString(NULL);
			if (a && *a)
			{	
				m_IpBox.AddString(a);
				
				if (*CONFIG.SSHListenAddress && !strncmp(a, CONFIG.SSHListenAddress, strlen(CONFIG.SSHListenAddress)))
							m_IpBox.SetCurSel(i+1);
				free(a);

						i++;
			}
		}
		delete b;
	}

	SetDlgItemInt(IDC_SSH_PORT, CONFIG.SSHListenPort, TRUE);
	SetDlgItemInt(IDC_SSH_MAXCONNECTIONS, CONFIG.SSHMaxConnections, TRUE);
	SetDlgItemInt(IDC_SSH_IDLETIMEOUT, CONFIG.SSHTimeout, TRUE);
	SetDlgItemText(IDC_SSH_BANNERMESSAGE, CONFIG.SSHBanner);
	SetDlgItemText(IDC_SSH_CMD, CONFIG.SSHCMD);
	CheckDlgButton(IDC_SSH_AUTOSTART, CONFIG.SSHRun);
	CheckDlgButton(IDC_SSH_NEWCONSOLE, CONFIG.SSHNewConsole);

	SetDlgItemText(IDC_SSH_RSAKEY, CONFIG.RSAKeyPath);
	SetDlgItemText(IDC_SSH_DSAKEY, CONFIG.DSAKeyPath);

	CheckHostKeys();

#undef CONFIG
	
	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_SSH));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);
	
	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageSSH::OnNewRSAKey(WORD wCode, WORD wID, HWND hWnd)
{
	RECT rect;
	HMENU hmnu;
	int i;
	
	::GetWindowRect(GetDlgItem(wID), &rect);

	hmnu = LoadMenu(_Module.m_hInst,MAKEINTRESOURCE(IDR_MENU));
						
	UINT uiSelectedMenu = TrackPopupMenu(GetSubMenu(hmnu,1), TPM_RETURNCMD, rect.left, rect.bottom, NULL, m_hWnd, NULL);

	if (uiSelectedMenu)
	{
		string Status;
		BOOL fRet = FALSE;

		UINT Bits = 512;
		if (uiSelectedMenu == ID_POPUP2_1024BITS)
			Bits = 1024;
		if (uiSelectedMenu == ID_POPUP2_2048BITS)
			Bits = 2048;
		
		CFileDialog dlg(FALSE, "", "privatekey.rsa",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.* (All Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please select SECRET place to save your private key";
		
		i = dlg.DoModal();
		
		if (i == 1)
		{
				SetCursor(LoadCursor(NULL, IDC_WAIT));
				fRet = HANDLER.GenerateRSAKey(Bits, dlg.m_szFileName, Status);
				SetCursor(LoadCursor(NULL, IDC_ARROW));
					
				if (fRet)
				{
					SetDlgItemText(IDC_SSH_RSAKEY, dlg.m_szFileName);
					CheckHostKeys();
				}
				else
					MessageBox(Status.c_str(), "Error generating RSA key", MB_ICONERROR);
		}
		
	
	
	}
	DestroyMenu(hmnu);
	
	return TRUE;
}

LRESULT CPageSSH::OnNewDSAKey(WORD wCode, WORD wID, HWND hWnd)
{
	RECT rect;
	HMENU hmnu;
	int i;
	
	::GetWindowRect(GetDlgItem(wID), &rect);

	hmnu = LoadMenu(_Module.m_hInst,MAKEINTRESOURCE(IDR_MENU));
						
	UINT uiSelectedMenu = TrackPopupMenu(GetSubMenu(hmnu,1), TPM_RETURNCMD, rect.left, rect.bottom, NULL, m_hWnd, NULL);

	if (uiSelectedMenu)
	{
		string Status;
		BOOL fRet = FALSE;

		UINT Bits = 512;
		if (uiSelectedMenu == ID_POPUP2_1024BITS)
			Bits = 1024;
		if (uiSelectedMenu == ID_POPUP2_2048BITS)
			Bits = 2048;
		
		CFileDialog dlg(FALSE, "", "privatekey.dsa",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.* (All Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please select SECRET place to save your private key";
		
		i = dlg.DoModal();
		
		if (i==1)
		{
				SetCursor(LoadCursor(NULL, IDC_WAIT));
				fRet = HANDLER.GenerateDSAKey(Bits, dlg.m_szFileName, Status);
				SetCursor(LoadCursor(NULL, IDC_ARROW));
					
				if (fRet)
				{
					SetDlgItemText(IDC_SSH_DSAKEY, dlg.m_szFileName);
					CheckHostKeys();
				}
				else
					MessageBox(Status.c_str(), "Error generating DSA key", MB_ICONERROR);
		}
		
	
	
	}

	DestroyMenu(hmnu);

	return TRUE;
}

LRESULT CPageSSH::OnBrowse(WORD wCode, WORD wID, HWND hWnd)
{
	int i = 0;

	if (wID == IDC_SSH_BANNERMESSAGE_BROWSE)
	{
		CFileDialog dlg(TRUE, "*.txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.txt (Text Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your Banner text file";
		
		i = dlg.DoModal();
		
		if (i == 1)
			SetDlgItemText(IDC_SSH_BANNERMESSAGE, dlg.m_szFileName);
	}
	
	if (wID == IDC_SSH_CMD_BROWSE)
	{
		CFileDialog dlg(TRUE, "*.exe", "*.exe",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.exe (Executable Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your command shell executable";
		
		i = dlg.DoModal();
		
		if (i == 1)
			SetDlgItemText(IDC_SSH_CMD, dlg.m_szFileName);	
	}

	if (wID == IDC_SSH_RSAKEY_BROWSE)
	{
		CFileDialog dlg(TRUE, "*", "",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.* (All Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your RSA private key";
	
		i = dlg.DoModal();
	
		if (i == 1)
		{
			SetDlgItemText(IDC_SSH_RSAKEY, dlg.m_szFileName);
			CheckHostKeys();
		}
	}
		
	if (wID == IDC_SSH_DSAKEY_BROWSE)
	{
		CFileDialog dlg(TRUE, "*", "",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.* (All Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please locate your DSA private key";
	
		i = dlg.DoModal();
	
		if (i == 1)
		{
			SetDlgItemText(IDC_SSH_DSAKEY, dlg.m_szFileName);
			CheckHostKeys();
		}
	}

	return TRUE;
}
	
LRESULT CPageSSH::OnChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

BOOL CPageSSH::OnSetActive() 
{
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);	
	
	return TRUE;
}

BOOL CPageSSH::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		
		return TRUE;
	}
}

void CPageSSH::CheckHostKeys()
{
	UINT Bits = 0;
	string strStatus("");
	char bits[1024];
	char FileName[MAX_PATH+1];

	GetDlgItemText(IDC_SSH_RSAKEY, FileName, MAX_PATH);

	HANDLER.CheckHostKey(FileName, (SSHKeyTypes) 0, strStatus, Bits);
	SetDlgItemText(IDC_LABEL_SSH_RSAKEY_STATUS, strStatus.c_str());
	
	if (Bits == 0)
		strncpy(bits, "[none]",7);
	else
		_snprintf(bits, 1024, "%d bits", Bits);
	SetDlgItemText(IDC_LABEL_SSH_RSAKEY_BITS, bits);

	GetDlgItemText(IDC_SSH_DSAKEY, FileName, MAX_PATH);

	HANDLER.CheckHostKey(FileName, (SSHKeyTypes) 1, strStatus, Bits);
	SetDlgItemText(IDC_LABEL_SSH_DSAKEY_STATUS, strStatus.c_str());
	
	if (Bits == 0)
		strncpy(bits, "[none]",7);
	else
		_snprintf(bits, 1024, "%d bits", Bits);

	SetDlgItemText(IDC_LABEL_SSH_DSAKEY_BITS, bits);

}

// CPageUsers

LRESULT CPageUsers::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	int iIndex = 0;
	
	m_ListBoxWndProc = (WNDPROC)::SetWindowLong(GetDlgItem(IDC_USERS_LIST), GWL_WNDPROC, (LONG)ListBoxProc);

	m_UsersBox = GetDlgItem(IDC_USERS_LIST);
	
	for (int i = 0; i < HANDLER.Users.size(); i++)
	{
		int iIndex = m_UsersBox.AddString(HANDLER.Users[i]->GetName());
		m_UsersBox.SetItemDataPtr(iIndex, HANDLER.Users[i]);
	}

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_USERS));
	m_hYes = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_YES));
	m_hNo = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_NO));
	m_hUser = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_USER));
	
	SetMsgHandled(FALSE);
	return TRUE;
}

BOOL CPageUsers::OnSetActive()
{
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);	
	
	return TRUE;
}

BOOL CPageUsers::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		
		return TRUE;
	}
}

LRESULT CPageUsers::OnAddUser(UINT Code, int Control, HWND hWnd)
{
	CfreeSSHdUser *freeNewUser = new CfreeSSHdUser();

	CUserDlg UserDlg("User properties");
	CUserPage1 UserPage1;

	UserDlg.m_User = freeNewUser;

	UserDlg.m_pgPage1 = &UserPage1;
	UserPage1.m_Parent = &UserDlg;
	
	UserDlg.AddPage(UserPage1);
	
	UserDlg.m_fNewUser = true;

	if (UserDlg.DoModal() == IDOK)
	{
		HANDLER.Users.push_back(freeNewUser);
		int iIndex = m_UsersBox.AddString(freeNewUser->GetName());
		m_UsersBox.SetItemDataPtr(iIndex, freeNewUser);
	}
	else
		delete freeNewUser;
	
	return TRUE;	
}

LRESULT CPageUsers::OnChangeUser(UINT Code, int Control, HWND hWnd)
{
	CfreeSSHdUser freeTmpUser, *freeSelUser = (CfreeSSHdUser *) m_UsersBox.GetItemData(m_UsersBox.GetCurSel());
	
	memcpy(&freeTmpUser, freeSelUser, sizeof(*freeSelUser));

	string sTitle = freeSelUser->GetName();
	sTitle += " properties";
	
	CUserDlg UserDlg(sTitle.c_str());
	CUserPage1 UserPage1;

	UserDlg.m_User = &freeTmpUser;

	UserDlg.m_pgPage1 = &UserPage1;
	UserPage1.m_Parent = &UserDlg;

	UserDlg.AddPage(UserPage1);
	
	if (UserDlg.DoModal() == IDOK)
		memcpy(freeSelUser, &freeTmpUser, sizeof(*freeSelUser));
		
	return TRUE;
}
	
LRESULT CPageUsers::OnRemoveUser(UINT Code, int Control, HWND hWnd)
{
	CListBox m_UsersBox = GetDlgItem(IDC_USERS_LIST);
	
	int i = m_UsersBox.GetCurSel();
	
	if (i >= 0)
	{
		TCHAR strPrompt[2048];
		TCHAR strQuestion[1024] = _T("Do you really want to remove user '%s'?");
		TCHAR strLabel[256];
		
		m_UsersBox.GetText(i, strLabel);
		
		_stprintf(strPrompt, strQuestion, strLabel);
		
		if (MessageBox(strPrompt, _T("freeSSHd"), MB_YESNO) == IDYES)
		{
			CWindow w = GetDlgItem(IDC_USERS_REMOVE);
			w.EnableWindow(FALSE);
			
			w = GetDlgItem(IDC_USERS_CHANGE);
			w.EnableWindow(FALSE);
			
			HANDLER.RemoveUser(strLabel);
			m_UsersBox.DeleteString(i);
		}
		
		
	}
	return TRUE;
}

LRESULT CPageUsers::OnItemActivate(UINT Code, int Control, HWND hWnd)
{	

	int i = m_UsersBox.GetCurSel();
	
	if (i > -1)
	{
		
		CWindow w = GetDlgItem(IDC_USERS_REMOVE);
		
		w.EnableWindow();
		
		w = GetDlgItem(IDC_USERS_CHANGE);
		w.EnableWindow();
	}

	if (Code == LBN_SELCHANGE)
		::InvalidateRect(GetDlgItem(IDC_USERS_LIST), NULL, FALSE);
	
	return TRUE;
}

WNDPROC CPageUsers::m_ListBoxWndProc = NULL;

LRESULT CPageUsers::OnMeasureItem(UINT CtlId, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	
	if (CtlId == IDC_USERS_LIST)
	{
		lpMeasureItemStruct->itemHeight = 19;
		return TRUE;
	}

	return FALSE;
	
}

LRESULT CPageUsers::OnDrawItem(UINT CtlId, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDCHandle dc(lpDrawItemStruct->hDC);
	CPen pen;
	CPenHandle OldPen;
	
	pen.CreatePen(PS_DOT, 1, RGB(128,128,128));

	HFONT hOldFont;
	hOldFont = dc.SelectFont(m_UsersBox.GetFont());

	OldPen = dc.SelectPen(pen);
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(RGB(255,255,255));
	dc.SetTextColor(RGB(0,0,0));
				
	if (lpDrawItemStruct->itemID < HANDLER.Users.size())
	{		

		CfreeSSHdUser *freeUser = (CfreeSSHdUser *) m_UsersBox.GetItemDataPtr(lpDrawItemStruct->itemID);
 
		lpDrawItemStruct->rcItem.top += 2;

		RECT r;
		memcpy(&r, &lpDrawItemStruct->rcItem, sizeof(RECT));
		
		dc.FillRect(&r, (HBRUSH) GetStockObject(WHITE_BRUSH));
		
		if (m_UsersBox.GetCurSel() == lpDrawItemStruct->itemID)
			dc.Rectangle(&r);
	
		r.top += 2;
		dc.DrawIconEx(r.left+2, r.top, m_hUser, 16, 16, 0, NULL, DI_NORMAL);
		
		r.left += 20;
		r.right = r.left + 170;
		
		dc.DrawText(freeUser->GetName(), strlen(freeUser->GetName()), &r, DT_LEFT);
		
		r.left = lpDrawItemStruct->rcItem.right - 200;

		if (freeUser->HasShellAccess())
			dc.DrawIconEx(r.left, r.top, m_hYes, 16, 16, 0, NULL, DI_NORMAL);
		else
			dc.DrawIconEx(r.left, r.top, m_hNo, 16, 16, 0, NULL, DI_NORMAL);
		
		r.left += 60;
		if (freeUser->HasSFTPAccess())
			dc.DrawIconEx(r.left, r.top, m_hYes, 16, 16, 0, NULL, DI_NORMAL);
		else
			dc.DrawIconEx(r.left, r.top, m_hNo, 16, 16, 0, NULL, DI_NORMAL);
		
		r.left += 60;
		if (freeUser->HasTunnelAccess())
			dc.DrawIconEx(r.left, r.top, m_hYes, 16, 16, 0, NULL, DI_NORMAL);
		else
			dc.DrawIconEx(r.left, r.top, m_hNo, 16, 16, 0, NULL, DI_NORMAL);
		
		
	}
	
	dc.SelectFont(hOldFont);
	dc.SelectPen(OldPen);

	return FALSE;
}


int WINAPI CPageUsers::ListBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CListBox UsersBox = hWnd;

    switch (uMsg) 
	{
		case WM_LBUTTONDOWN:
			if (1)
			{

				RECT rc;

				SendMessage(hWnd, LB_GETITEMRECT, 0, (LPARAM)&rc);
				
				rc.left = rc.right - 200;

				int index = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				int l = index + yPos/19;

				if (l < HANDLER.Users.size())
				{
					CfreeSSHdUser *freeUser = (CfreeSSHdUser *) UsersBox.GetItemDataPtr(l);
				
					if (xPos > rc.left && xPos < rc.left+16)
					{
						freeUser->SetShellAccess(!freeUser->HasShellAccess());
						::InvalidateRect(hWnd, NULL, FALSE);
					}
					if (xPos > rc.left+60 && xPos < rc.left+76)
					{
						freeUser->SetSFTPAccess(!freeUser->HasSFTPAccess());
						::InvalidateRect(hWnd, NULL, FALSE);
					}
					if (xPos > rc.left+120 && xPos < rc.left+136)
					{
						freeUser->SetTunnelAccess(!freeUser->HasTunnelAccess());
						::InvalidateRect(hWnd, NULL, FALSE);
					}
				}
			}
			break;

		case WM_ERASEBKGND:
			RECT rc;
			::GetWindowRect(hWnd, &rc);
			::FillRect((HDC)wParam, &rc, ::GetSysColorBrush(COLOR_WINDOW));
			break;
	}	
	
	return CallWindowProc(m_ListBoxWndProc, hWnd, uMsg, wParam, lParam);
	
}
// CPageHostRestrictions

LRESULT CPageHostRestrictions::OnInitDialog(HWND hWnd, LPARAM lParam) 
{
// a little trick to save me trouble writing same thing over and over....

#define CONFIG HANDLER.Config
	
	  if (CONFIG.HostRestrictionsAllow)
		CheckDlgButton(IDC_HOSTRESTRICTIONS_ALLOW, TRUE);
	  else
		CheckDlgButton(IDC_HOSTRESTRICTIONS_REFUSE, TRUE);
	  
	SetDlgItemText(IDC_HOSTRESTRICTIONS_IPADDRESSES, CONFIG.HostRestrictions);
		
 #undef CONFIG

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_HOSTRESTRICTIONS));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);

	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageHostRestrictions::OnChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

BOOL CPageHostRestrictions::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}

// CPageCrypto

LRESULT CPageCrypto::OnInitDialog(HWND hWnd, LPARAM lParam) 
{

#define CONFIG HANDLER.Config

		switch (CONFIG.SSHCiphers)
			{
				case encAES:
					CheckDlgButton(IDC_CRYPTO_CIPHER_AES, TRUE);
					break;

				case encAES128:
					CheckDlgButton(IDC_CRYPTO_CIPHER_AES128, TRUE);
					break;

				case encAES192:
					CheckDlgButton(IDC_CRYPTO_CIPHER_AES192, TRUE);
					break;

				case encAES256:
					CheckDlgButton(IDC_CRYPTO_CIPHER_AES256, TRUE);
					break;

				case enc3DES:
					CheckDlgButton(IDC_CRYPTO_CIPHER_3DES, TRUE);
					break;

				case encBLOWFISH:
					CheckDlgButton(IDC_CRYPTO_CIPHER_BLOWFISH, TRUE);
					break;

				case encCAST128:
					CheckDlgButton(IDC_CRYPTO_CIPHER_CAST128, TRUE);
					break;

				default:
					CheckDlgButton(IDC_CRYPTO_CIPHER_ANY, TRUE);
					break;
			}

			switch (CONFIG.SSHMACs)
			{
				case 1:
					CheckDlgButton(IDC_CRYPTO_MAC_MD5, FALSE);
					CheckDlgButton(IDC_CRYPTO_MAC_HMAC, TRUE);
					break;
				case 2:
					CheckDlgButton(IDC_CRYPTO_MAC_MD5, TRUE);
					CheckDlgButton(IDC_CRYPTO_MAC_HMAC, TRUE);
					break;
				default:
					CheckDlgButton(IDC_CRYPTO_MAC_MD5, TRUE);
					CheckDlgButton(IDC_CRYPTO_MAC_HMAC, FALSE);
				
			}
	

	
#undef CONFIG

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_CRYPTO));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);

	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageCrypto::OnChange(WORD wCode, WORD wID, HWND hWnd)
{
	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;

}

BOOL CPageCrypto::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}

// CPageSFTP

LRESULT CPageSFTP::OnInitDialog(HWND hWnd, LPARAM lParam) 
{

#define CONFIG HANDLER.Config

	SetDlgItemText(IDC_SFTP_HOME, CONFIG.SFTPHomePath);
	
#undef CONFIG

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_SFTP));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);

	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageSFTP::OnEditChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

LRESULT CPageSFTP::OnBrowse(WORD Code, WORD Control, HWND hWnd)
{
	string strPath;

	if (GetFolder(strPath, "Select SFTP home folder", this->m_hWnd))
		SetDlgItemText(IDC_SFTP_HOME, strPath.c_str());

	return TRUE;
}

BOOL CPageSFTP::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}

// CPageTunneling

LRESULT CPageTunneling::OnInitDialog(HWND hWnd, LPARAM lParam) 
{
	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_TUNNELING));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);
	
#define CONFIG HANDLER.Config

	CheckDlgButton(IDC_TUNNELING_LOCAL, CONFIG.SSHLocalTunnel);
	CheckDlgButton(IDC_TUNNELING_REMOTE, CONFIG.SSHRemoteTunnel);
	CheckDlgButton(IDC_TUNNELING_LOCAL_ONLY, CONFIG.SSHLocalTunnelOnly);
	CheckDlgButton(IDC_TUNNELING_REMOTE_ONLY, CONFIG.SSHRemoteTunnelOnly);
	
	if (CONFIG.SSHLocalTunnel)
		::EnableWindow(GetDlgItem(IDC_TUNNELING_LOCAL_ONLY), TRUE);

	if (CONFIG.SSHRemoteTunnel)
		::EnableWindow(GetDlgItem(IDC_TUNNELING_REMOTE_ONLY), TRUE);

#undef CONFIG

	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageTunneling::OnLocalTunnelingChecked(WORD wID, WORD wCode, HWND hWnd, BOOL &bHandled)
{
	if (IsDlgButtonChecked(IDC_TUNNELING_LOCAL))
		::EnableWindow(GetDlgItem(IDC_TUNNELING_LOCAL_ONLY), TRUE);
	else
		::EnableWindow(GetDlgItem(IDC_TUNNELING_LOCAL_ONLY), FALSE);

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);

	return TRUE;
}

LRESULT CPageTunneling::OnRemoteTunnelingChecked(WORD wID, WORD wCode, HWND hWnd, BOOL &bHandled)
{
	if (IsDlgButtonChecked(IDC_TUNNELING_REMOTE))
		::EnableWindow(GetDlgItem(IDC_TUNNELING_REMOTE_ONLY), TRUE);
	else
		::EnableWindow(GetDlgItem(IDC_TUNNELING_REMOTE_ONLY), FALSE);

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);

	return TRUE;
}

LRESULT CPageTunneling::OnChange(WORD wCode, WORD wID, HWND hWnd)
{
	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);

	return TRUE;
}
BOOL CPageTunneling::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}

// CPageAuth

LRESULT CPageAuth::OnInitDialog(HWND hWnd, LPARAM lParam) 
{

#define CONFIG HANDLER.Config

	SetDlgItemText(IDC_AUTH_PUBLICKEYFOLDER, CONFIG.SSHPublickeyPath);
	
	switch (CONFIG.SSHPasswordAuth)
	{
		case 1:
			CheckDlgButton(IDC_AUTH_PASSWORD_DISABLED, TRUE);
			break;
		
		case 2:
			CheckDlgButton(IDC_AUTH_PASSWORD_REQUIRED, TRUE);
			break;

		default:
			CheckDlgButton(IDC_AUTH_PASSWORD_ALLOWED, TRUE);
			break;
	}
	
	switch (CONFIG.SSHPublickeyAuth)
	{
		case 1:
			CheckDlgButton(IDC_AUTH_PUBLICKEY_DISABLED, TRUE);
			break;
		
		case 2:
			CheckDlgButton(IDC_AUTH_PUBLICKEY_REQUIRED, TRUE);
			break;

		default:
			CheckDlgButton(IDC_AUTH_PUBLICKEY_ALLOWED, TRUE);
			break;
	}
#undef CONFIG
	
	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_AUTH));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);
	
	SetMsgHandled(FALSE);
	return TRUE;
}

LRESULT CPageAuth::OnBrowse(WORD Code, WORD Control, HWND hWnd)
{
	string strPath;

	if (GetFolder(strPath, "Select your public key folder", this->m_hWnd))
		SetDlgItemText(IDC_AUTH_PUBLICKEYFOLDER, strPath.c_str());

	return TRUE;
}

LRESULT CPageAuth::OnChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);

	return TRUE;
}

BOOL CPageAuth::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}

// CPageLogging 

LRESULT CPageLogging::OnInitDialog(HWND hWnd, LPARAM lParam) 
{	
#define CONFIG HANDLER.Config

	CheckDlgButton(IDC_LOGGING_LOGEVENTS, CONFIG.LogEvents);
	SetDlgItemText(IDC_LOGGING_LOGFILE, CONFIG.LogFilePath);
	CheckDlgButton(IDC_LOGGING_RESOLVEIP, CONFIG.LogResolveIP);

	if (CONFIG.LogEvents)
	{
		::EnableWindow(GetDlgItem(IDC_LOGGING_GROUP), TRUE);
		::EnableWindow(GetDlgItem(IDC_LABEL_LOGGING_LOGFILE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE_BROWSE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_RESOLVEIP), TRUE);
	}
	
#undef CONFIG

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_LOGGING));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);

	SetMsgHandled(FALSE);
	return TRUE;
}


LRESULT CPageLogging::OnChange(WORD Code, WORD Control, HWND hWnd)
{

	if (m_hPageIcon)
	{
		if ((Code == BN_CLICKED) && (Control == IDC_LOGGING_LOGFILE_BROWSE))
			return TRUE;
		
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	}
	
	return TRUE;
}

LRESULT CPageLogging::OnBrowse(WORD Code, WORD Control, HWND hWnd)
{
	int i = 0;

	if (Control == IDC_LOGGING_LOGFILE_BROWSE)
	{
		CFileDialog dlg(TRUE, "*.txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.txt (Text Files)\0\0");
		dlg.m_ofn.lpstrTitle = "Please select your log file";
		
		i = dlg.DoModal();
		
		if (i == 1)
			SetDlgItemText(IDC_LOGGING_LOGFILE, dlg.m_szFileName);
	}

	return TRUE;
}

LRESULT CPageLogging::OnLogFileChecked(WORD wID, WORD wCode, HWND hWnd)
{
	if (IsDlgButtonChecked(IDC_LOGGING_LOGEVENTS))
	{
		::EnableWindow(GetDlgItem(IDC_LOGGING_GROUP), TRUE);
		::EnableWindow(GetDlgItem(IDC_LABEL_LOGGING_LOGFILE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE_BROWSE), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE_OPEN), TRUE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_RESOLVEIP), TRUE);
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_LOGGING_GROUP), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_LOGGING_LOGFILE), FALSE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE), FALSE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE_BROWSE), FALSE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_LOGFILE_OPEN), FALSE);
		::EnableWindow(GetDlgItem(IDC_LOGGING_RESOLVEIP), FALSE);
	}
	
	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

LRESULT CPageLogging::OnLogFileOpen(WORD wID, WORD wCode, HWND hWnd)
{	
	char strLogFile[MAX_PATH+1];

	GetDlgItemText(IDC_LOGGING_LOGFILE, strLogFile, MAX_PATH + 1);
	ShellExecute(m_hWnd, NULL, strLogFile, NULL, NULL, SW_SHOWNORMAL);
	
	return TRUE;
}

BOOL CPageLogging::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		return TRUE;
	}
}


// CPageOnlineUsers

LRESULT CPageOnlineUsers::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CSSHUser *user = NULL;
	CSSHUsers *users;
	short iUserCount;
	
	LVITEM lvitem = { 0 };
	int i = 0, j = 0;
	char buff[256];

	m_UsersBox = GetDlgItem(IDC_ONLINEUSERS_LIST);
	m_UsersBox.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
	m_UsersBox.DeleteAllItems();
	m_UsersBox.InsertColumn(0, "User", LVCFMT_LEFT, 50, 0);
	m_UsersBox.InsertColumn(1, "Session", LVCFMT_LEFT, 60, 1);
	m_UsersBox.InsertColumn(2, "Online since", LVCFMT_LEFT, 230, 2);
	
	

	m_SSH->get_Users(&users);
	users->get_Count(&iUserCount);

	if ( iUserCount != 0 ) 
	{
		for (i = 0; i < iUserCount ; i++)
		{
			
			users->get_Item(i, &user);
			CComBSTR2 login;
			user->get_Login(&login);
			
			// user name
			lvitem.mask = LVIF_TEXT|LVIF_PARAM;
			lvitem.iItem = 0;
			lvitem.iSubItem = 0;
			lvitem.pszText = (LPTSTR)login.ToString();
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = (LPARAM)user;
			
			j = m_UsersBox.InsertItem(&lvitem);
			
			// session type
			strcpy(buff,"SSH");
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = j;
			lvitem.iSubItem = 1;
			lvitem.pszText = (LPTSTR)buff;
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = 0;
			
			m_UsersBox.SetItem(&lvitem);
			
			// time 
			DATE dTimeConnected;
			user->get_TimeConnected(&dTimeConnected);
			DateToString(dTimeConnected, buff);
			
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = j;
			lvitem.iSubItem = 2;
			lvitem.pszText = (LPTSTR)buff;
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = 0;
			
			m_UsersBox.SetItem(&lvitem);
		}
	}


	m_Telnet->get_Users(&users);
	users->get_Count(&iUserCount);

	if ( iUserCount != 0 ) 
	{
		for (i=0;i < iUserCount; i++)
		{		
			users->get_Item(i, &user);
			CComBSTR2 login;
			user->get_Login(&login);
			
			// user name
			lvitem.mask = LVIF_TEXT|LVIF_PARAM;
			lvitem.iItem = 0;
			lvitem.iSubItem = 0;
			lvitem.pszText = (LPTSTR)login.ToString();
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = (LPARAM)user;
			
			j = m_UsersBox.InsertItem(&lvitem);
			
			// session type
			strcpy(buff,"TELNET");
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = j;
			lvitem.iSubItem = 1;
			lvitem.pszText = (LPTSTR)buff;
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = 0;
			
			m_UsersBox.SetItem(&lvitem);
			
			// time 
			DATE dTimeConnected;
			user->get_TimeConnected(&dTimeConnected);
			DateToString(dTimeConnected, buff);
			
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = j;
			lvitem.iSubItem = 2;
			lvitem.pszText = (LPTSTR)buff;
			lvitem.state = 0;
			lvitem.stateMask = 0;
			lvitem.iImage = 0;
			lvitem.lParam = 0;
			
			m_UsersBox.SetItem(&lvitem);
		}
	}

	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_USERS));
	
	SetMsgHandled(FALSE);
	return TRUE;
}

BOOL CPageOnlineUsers::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		
		return TRUE;
	}
}

LRESULT CPageOnlineUsers::OnItemActivate(LPNMHDR nmhdr)
{
	if (nmhdr->code == LVN_ITEMACTIVATE || nmhdr->code == LVN_COLUMNCLICK) 
	{

		int i = m_UsersBox.GetSelectedIndex();
		
		if (i > -1)
		{
			CWindow w = GetDlgItem(IDC_ONLINEUSERS_DISCONNECT);
			
			w.EnableWindow();
		}
	}
	
	return FALSE;
}

LRESULT CPageOnlineUsers::OnUserDisconnect(UINT Code, int Control, HWND hWnd)
{
	LVITEM lvitem;
	int index = m_UsersBox.GetSelectedIndex();
		
	if (index != -1)
	{;
		if (MessageBox("Are you sure you want to disconnect the selected user?","Warning", MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			lvitem.iItem = index;
			lvitem.mask = LVIF_PARAM;

			if (m_UsersBox.GetItem(&lvitem) == TRUE)
			{
				CSSHUser *user = (CSSHUser *) lvitem.lParam;
				user->Disconnect();
				::EnableWindow(GetDlgItem(IDC_ONLINEUSERS_DISCONNECT), FALSE);
			}
		}
	}

	return TRUE;
}

BOOL CPageOnlineUsers::AddSSHConnection(CSSHUser *User)
{
	LVFINDINFO lvfi = { 0 };
	lvfi.flags = LVFI_PARAM;
	LVITEM lvitem;
	int i = 0, j = 0;
	char buff[256];
	
	lvfi.lParam = (LPARAM) User;
	CComBSTR2 login;
	User->get_Login(&login);

	if (m_UsersBox.FindItem(&lvfi,-1) != -1) 
		return FALSE;
	
	// user name
	lvitem.mask = LVIF_TEXT|LVIF_PARAM;
	lvitem.iItem = 0;
	lvitem.iSubItem = 0;
	lvitem.pszText = (LPTSTR)login.ToString();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = (LPARAM) User;
	
	j = m_UsersBox.InsertItem(&lvitem);
	
	// session type
	strcpy(buff,"SSH");
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = j;
	lvitem.iSubItem = 1;
	lvitem.pszText = (LPTSTR)buff;
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = 0;
	
	m_UsersBox.SetItem(&lvitem);
	
	// time 
	DATE dTimeConnected;
	User->get_TimeConnected(&dTimeConnected);
	DateToString(dTimeConnected, buff);
	
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = j;
	lvitem.iSubItem = 2;
	lvitem.pszText = (LPTSTR)buff;
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = 0;
	
	m_UsersBox.SetItem(&lvitem);

	return TRUE;
}

BOOL CPageOnlineUsers::AddTelnetConnection(CSSHUser *User)
{
	LVFINDINFO lvfi = { 0 };
	lvfi.flags = LVFI_PARAM;
	LVITEM lvitem;
	int i = 0, j = 0;
	char buff[256];
	
	lvfi.lParam = (LPARAM) User;
	CComBSTR2 login;
	User->get_Login(&login);

	if (m_UsersBox.FindItem(&lvfi,-1) != -1) 
		return FALSE;
	
	// user name
	lvitem.mask = LVIF_TEXT|LVIF_PARAM;
	lvitem.iItem = 0;
	lvitem.iSubItem = 0;
	lvitem.pszText = (LPTSTR)login.ToString();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = (LPARAM) User;
	
	j = m_UsersBox.InsertItem(&lvitem);
	
	// session type
	strcpy(buff,"TELNET");
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = j;
	lvitem.iSubItem = 1;
	lvitem.pszText = (LPTSTR)buff;
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = 0;
	
	m_UsersBox.SetItem(&lvitem);
	
	// time 
	DATE dTimeConnected;
	User->get_TimeConnected(&dTimeConnected);
	DateToString(dTimeConnected, buff);
	
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = j;
	lvitem.iSubItem = 2;
	lvitem.pszText = (LPTSTR)buff;
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.iImage = 0;
	lvitem.lParam = 0;
	
	m_UsersBox.SetItem(&lvitem);

	return TRUE;
	
}

BOOL CPageOnlineUsers::RemoveConnection(CSSHUser *User)
{
	LVFINDINFO lvfi = { 0 };
	lvfi.flags = LVFI_PARAM;
	int index = -1;

	CComBSTR2 login;
	User->get_Login(&login);
		
	lvfi.lParam = (LPARAM) User;
	index = m_UsersBox.FindItem(&lvfi,-1);
		
	if (index != -1)
	{
		m_UsersBox.DeleteItem(index);
		return TRUE;
	}
	
	return FALSE;
}

LRESULT CPageUpdate::OnInitDialog(HWND hWnd, LPARAM lParam) 
{

#define CONFIG HANDLER.Config
	
	if (HANDLER.m_AppUpdateOK)
	{
		CheckDlgButton(IDC_UPDATE_CHECKONSTARTUP, CONFIG.UpdateCheckOnStartup);
		CheckDlgButton(IDC_UPDATE_DONTPROMPT, CONFIG.UpdateDontPrompt);
		CheckDlgButton(IDC_UPDATE_SHOWMESSAGES, CONFIG.UpdateShowMessages);

		if (CONFIG.UpdateCheckOnStartup)
		{
			::EnableWindow(GetDlgItem(IDC_UPDATE_OPTIONS), TRUE);
			::EnableWindow(GetDlgItem(IDC_UPDATE_DONTPROMPT), TRUE);
			::EnableWindow(GetDlgItem(IDC_UPDATE_SHOWMESSAGES), TRUE);
		}
		else
		{
			::EnableWindow(GetDlgItem(IDC_UPDATE_OPTIONS), FALSE);
			::EnableWindow(GetDlgItem(IDC_UPDATE_DONTPROMPT), FALSE);
			::EnableWindow(GetDlgItem(IDC_UPDATE_SHOWMESSAGES), FALSE);
		}
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_UPDATE_OPTIONS), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_DONTPROMPT), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_SHOWMESSAGES), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_CHECKONSTARTUP), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_CHECKNOW), FALSE);
	}
	
#undef CONFIG
	
	m_hPageIcon = LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_UPDATE));
	SendDlgItemMessage(IDC_PAGEICON, STM_SETICON, (WPARAM) m_hPageIcon, 0);
	
	SetMsgHandled(FALSE);
	return TRUE;
}

BOOL CPageUpdate::OnQueryCancel() 
{
	if (CancelPrompt() == IDYES)
		return FALSE;
	else
	{
		DestroyIcon(m_hPageIcon);
		
		return TRUE;
	}
}

LRESULT CPageUpdate::OnUpdateCheckNow(WORD wCode, WORD wID, HWND hWnd)
{
	HANDLER.CheckForUpdate(FALSE);

	return TRUE;
}

LRESULT CPageUpdate::OnUpdateCheckOnStartupChecked(WORD wCode, WORD wID, HWND hWnd)
{
	if (IsDlgButtonChecked(IDC_UPDATE_CHECKONSTARTUP))
	{
		::EnableWindow(GetDlgItem(IDC_UPDATE_OPTIONS), TRUE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_DONTPROMPT), TRUE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_SHOWMESSAGES), TRUE);
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_UPDATE_OPTIONS), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_DONTPROMPT), FALSE);
		::EnableWindow(GetDlgItem(IDC_UPDATE_SHOWMESSAGES), FALSE);
	}
	
	if (m_hPageIcon)
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	
	return TRUE;
}

LRESULT CPageUpdate::OnChange(WORD Code, WORD Control, HWND hWnd)
{
	if (m_hPageIcon)
	{
		if ((Code == BN_CLICKED) && (Control == IDC_UPDATE_CHECKNOW))
			return TRUE;
		
		::EnableWindow(HANDLER.m_Sheet->GetDlgItem(ID_APPLY_NOW), TRUE);
	}
	
	return TRUE;
}