// maindlg.cpp : implementation of the CSettingsDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "maindlg.h"
#include "pages.h"

BOOL CheckIfEmpty(UINT PageIndex, HWND hWnd)
{
	LPWSTR pszTitle = L"Error";
	
	if (GetWindowTextLength(hWnd) == 0)
	{	
		HANDLER.m_Sheet->SetActivePage(PageIndex);

		if (VisualStyles())
		{
			EDITBALLOONTIP balloon;
		 
			
			balloon.cbStruct = sizeof(EDITBALLOONTIP); 
			balloon.pszText = L"Field cannot be blank."; 
			balloon.pszTitle = pszTitle; 
			balloon.ttiIcon = TTI_ERROR; 
			
			Edit_ShowBalloonTip(hWnd, &balloon);
		}
		else
		{
			MessageBoxW(hWnd, L"Field cannot be blank.", pszTitle, MB_ICONERROR);
		}
		
		return TRUE;
	}	

	return FALSE;
}

BOOL VisualStyles()
{
	return GetDllVersion("comctl32.dll") >= PACKVERSION(6,0);
}

DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    hinstDll = LoadLibrary(lpszDllName);
	
    if (hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;

        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

        /*Because some DLLs may not implement this function, you
         *must test for it explicitly. Depending on the particular 
         *DLL, the lack of a DllGetVersion function may
         *be a useful indicator of the version.
        */
        
        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

void CSettingsDlg::OnShowWindow(BOOL, int)
{
	
	if (!HANDLER.IsInstalled())
	{
		MessageBox(_T("freeSSHd could not be found. Did you finish installation without any errors?"), _T("Error"), MB_ICONERROR);
		PostQuitMessage(1);
		return;
	}
	
	CenterWindow(GetDesktopWindow());
	
	SetMsgHandled(FALSE);
}

BOOL CSettingsDlg::OnApply(WORD Code, WORD Id, HWND hWnd)
{
	BOOL fRet = OnOK(Code, Id, hWnd);
	
	if (fRet)
		::EnableWindow(GetDlgItem(ID_APPLY_NOW), FALSE);

	return fRet;
}

BOOL CSettingsDlg::OnOK(WORD Code, WORD Id, HWND hWnd)
{
	char strBuff[MAX_PATH+1];
	UINT uiTmp = 0;
	int i = 0;

	HANDLER.EditConfig_Start();

	// Telnet tab
	
	if (HANDLER.m_pgPageTelnet->IsWindow())
	{
		HANDLER.m_pgPageTelnet->GetDlgItemText(IDC_TELNET_LISTENADDRESS, strBuff, 256);
		
		while(strBuff[i] && strBuff[i] != ' ')
			i++;
		
		strBuff[i] = 0;

		m_fRestartTelnet = strncmp(strBuff, HANDLER.Config.TelnetListenAddress,256) ? true:false;
		
		
		strncpy(HANDLER.Config.TelnetListenAddress, strBuff, 256);
		
		uiTmp = HANDLER.m_pgPageTelnet->GetDlgItemInt(IDC_TELNET_PORT, NULL, FALSE);

		if (CheckIfEmpty(1, HANDLER.m_pgPageTelnet->GetDlgItem(IDC_TELNET_PORT)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}
		
		if (HANDLER.Config.TelnetListenPort != uiTmp)
			m_fRestartTelnet = true;

		HANDLER.Config.TelnetListenPort = uiTmp;
		HANDLER.Config.TelnetMaxConnections = HANDLER.m_pgPageTelnet->GetDlgItemInt(IDC_TELNET_MAXCONNECTIONS, NULL, FALSE);
		HANDLER.Config.TelnetTimeout = HANDLER.m_pgPageTelnet->GetDlgItemInt(IDC_TELNET_IDLETIMEOUT, NULL, FALSE);
		
		HANDLER.m_pgPageTelnet->GetDlgItemText(IDC_TELNET_BANNERMESSAGE, HANDLER.Config.TelnetBanner, MAX_PATH);
		
		if (CheckIfEmpty(1, HANDLER.m_pgPageTelnet->GetDlgItem(IDC_TELNET_CMD)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}
		
		HANDLER.m_pgPageTelnet->GetDlgItemText(IDC_TELNET_CMD, HANDLER.Config.TelnetCMD, MAX_PATH);
		HANDLER.Config.TelnetRun = HANDLER.m_pgPageTelnet->IsDlgButtonChecked(IDC_TELNET_AUTOSTART);

		uiTmp = HANDLER.m_pgPageTelnet->IsDlgButtonChecked(IDC_TELNET_NEWCONSOLE);
		
		if (uiTmp != HANDLER.Config.TelnetNewConsole)
			m_fRestartTelnet = true;
		
		HANDLER.Config.TelnetNewConsole = uiTmp;	
	}	

	// end of Telnet tab

	// SSH tab
	
	if (HANDLER.m_pgPageSSH->IsWindow())
	{		
		HANDLER.m_pgPageSSH->GetDlgItemText(IDC_SSH_LISTENADDRESS, strBuff, 256);
		
		while(strBuff[i] && strBuff[i] != ' ')
			i++;
		
		strBuff[i] = 0;

		m_fRestartSSH = strncmp(strBuff, HANDLER.Config.SSHListenAddress,256) ? true:false;
		
		
		strncpy(HANDLER.Config.SSHListenAddress, strBuff, 256);
		
		if (CheckIfEmpty(2, HANDLER.m_pgPageSSH->GetDlgItem(IDC_SSH_PORT)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}

		uiTmp = HANDLER.m_pgPageSSH->GetDlgItemInt(IDC_SSH_PORT, NULL, FALSE);
		
		if (HANDLER.Config.SSHListenPort != uiTmp)
			m_fRestartSSH = true;
	
		HANDLER.Config.SSHListenPort = uiTmp; 
		HANDLER.Config.SSHMaxConnections = HANDLER.m_pgPageSSH->GetDlgItemInt(IDC_SSH_MAXCONNECTIONS, NULL, FALSE);
		HANDLER.Config.SSHTimeout = HANDLER.m_pgPageSSH->GetDlgItemInt(IDC_SSH_IDLETIMEOUT, NULL, FALSE);
		
		HANDLER.m_pgPageSSH->GetDlgItemText(IDC_SSH_BANNERMESSAGE, HANDLER.Config.SSHBanner, MAX_PATH);

		if (CheckIfEmpty(2, HANDLER.m_pgPageSSH->GetDlgItem(IDC_SSH_CMD)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}

		HANDLER.m_pgPageSSH->GetDlgItemText(IDC_SSH_CMD, HANDLER.Config.SSHCMD, MAX_PATH);
		
		HANDLER.m_pgPageSSH->GetDlgItemText(IDC_SSH_DSAKEY, strBuff, MAX_PATH);
		if (strncmp(strBuff, HANDLER.Config.DSAKeyPath,MAX_PATH))
			m_fRestartSSH = true;;
		strncpy(HANDLER.Config.DSAKeyPath, strBuff, MAX_PATH);

		HANDLER.m_pgPageSSH->GetDlgItemText(IDC_SSH_RSAKEY, strBuff, MAX_PATH);
		if (strncmp(strBuff, HANDLER.Config.RSAKeyPath,MAX_PATH))
			m_fRestartSSH = true;;
		strncpy(HANDLER.Config.RSAKeyPath, strBuff, MAX_PATH);

		if ((strlen(HANDLER.Config.RSAKeyPath) == 0)  && CheckIfEmpty(2, HANDLER.m_pgPageSSH->GetDlgItem(IDC_SSH_DSAKEY)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}

		HANDLER.Config.SSHRun = HANDLER.m_pgPageSSH->IsDlgButtonChecked(IDC_SSH_AUTOSTART);

		uiTmp = HANDLER.m_pgPageSSH->IsDlgButtonChecked(IDC_SSH_NEWCONSOLE);
		
		if (uiTmp != HANDLER.Config.SSHNewConsole)
			m_fRestartSSH = true;
		
		HANDLER.Config.SSHNewConsole = uiTmp;	
	}

	// end of SSH tab

	// Authentication tab
	
	if (HANDLER.m_pgPageAuth->IsWindow())
	{
		HANDLER.m_pgPageAuth->GetDlgItemText(IDC_AUTH_PUBLICKEYFOLDER, HANDLER.Config.SSHPublickeyPath, MAX_PATH);
		
		if (CheckIfEmpty(3, HANDLER.m_pgPageAuth->GetDlgItem(IDC_AUTH_PUBLICKEYFOLDER)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}

		uiTmp = 0;
		if (HANDLER.m_pgPageAuth->IsDlgButtonChecked(IDC_AUTH_PASSWORD_DISABLED))
			uiTmp = 1;
		if (HANDLER.m_pgPageAuth->IsDlgButtonChecked(IDC_AUTH_PASSWORD_REQUIRED))
			uiTmp = 2;

		HANDLER.Config.SSHPasswordAuth = uiTmp;

		uiTmp = 0;
		if (HANDLER.m_pgPageAuth->IsDlgButtonChecked(IDC_AUTH_PUBLICKEY_DISABLED))
			uiTmp = 1;
		if (HANDLER.m_pgPageAuth->IsDlgButtonChecked(IDC_AUTH_PUBLICKEY_REQUIRED))
			uiTmp = 2;

		HANDLER.Config.SSHPublickeyAuth = uiTmp;
	}

	// end of Authentication tab

	// Encryption tab

	if (HANDLER.m_pgPageCrypto->IsWindow())
	{
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_AES))
			HANDLER.Config.SSHCiphers = encAES;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_AES128))
			HANDLER.Config.SSHCiphers = encAES128;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_AES192))
			HANDLER.Config.SSHCiphers = encAES192;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_AES256))
			HANDLER.Config.SSHCiphers = encAES256;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_3DES))
			HANDLER.Config.SSHCiphers = enc3DES;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_BLOWFISH))
			HANDLER.Config.SSHCiphers = encBLOWFISH;
		else
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_CIPHER_CAST128))
			HANDLER.Config.SSHCiphers = encCAST128;
		else
			HANDLER.Config.SSHCiphers = encAny;
		
		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_MAC_HMAC))
			HANDLER.Config.SSHMACs=1;

		if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_MAC_HMAC))
			if (HANDLER.m_pgPageCrypto->IsDlgButtonChecked(IDC_CRYPTO_MAC_MD5))
				HANDLER.Config.SSHMACs = 2;
			else 
				HANDLER.Config.SSHMACs = 65536;
	}

	// end of Encryption tab

	// Tunneling tab

	if (HANDLER.m_pgPageTunneling->IsWindow())
	{
		HANDLER.Config.SSHLocalTunnel = HANDLER.m_pgPageTunneling->IsDlgButtonChecked(IDC_TUNNELING_LOCAL) ? TRUE:FALSE;
		HANDLER.Config.SSHLocalTunnelOnly = HANDLER.m_pgPageTunneling->IsDlgButtonChecked(IDC_TUNNELING_LOCAL_ONLY) ? TRUE:FALSE;
		HANDLER.Config.SSHRemoteTunnel = HANDLER.m_pgPageTunneling->IsDlgButtonChecked(IDC_TUNNELING_REMOTE) ? TRUE:FALSE;
		HANDLER.Config.SSHRemoteTunnelOnly = HANDLER.m_pgPageTunneling->IsDlgButtonChecked(IDC_TUNNELING_REMOTE_ONLY) ? TRUE:FALSE;	
	}

	// end of Tunneling tab
		
	// SFTP tab

	if (HANDLER.m_pgPageSFTP->IsWindow())
	{
		HANDLER.m_pgPageSFTP->GetDlgItemText(IDC_SFTP_HOME, HANDLER.Config.SFTPHomePath, MAX_PATH);

		if (CheckIfEmpty(6, HANDLER.m_pgPageSFTP->GetDlgItem(IDC_SFTP_HOME)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}
	}

	// end of SFTP tab
	
	// Users tab
	// end of Users tab

	// Host restrictions tab

	if (HANDLER.m_pgPageHostRestrictions->IsWindow())
	{
		HANDLER.Config.HostRestrictionsAllow = HANDLER.m_pgPageHostRestrictions->IsDlgButtonChecked(IDC_HOSTRESTRICTIONS_ALLOW) ? TRUE:FALSE;
		HANDLER.m_pgPageHostRestrictions->GetDlgItemText(IDC_HOSTRESTRICTIONS_IPADDRESSES, HANDLER.Config.HostRestrictions, 2048);
	}
	
	// end of Host restrictions tab

	// Logging tab

	if (HANDLER.m_pgPageLogging->IsWindow())
	{
		HANDLER.Config.LogEvents = HANDLER.m_pgPageLogging->IsDlgButtonChecked(IDC_LOGGING_LOGEVENTS) ? TRUE:FALSE;
		HANDLER.Config.LogResolveIP = HANDLER.m_pgPageLogging->IsDlgButtonChecked(IDC_LOGGING_RESOLVEIP) ? TRUE:FALSE;
		HANDLER.m_pgPageLogging->GetDlgItemText(IDC_LOGGING_LOGFILE, HANDLER.Config.LogFilePath, MAX_PATH);

		if (HANDLER.Config.LogEvents && CheckIfEmpty(9, HANDLER.m_pgPageLogging->GetDlgItem(IDC_LOGGING_LOGFILE)))
		{
			HANDLER.EditConfig_Failed();
			return FALSE;
		}
	}

	// end of Logging tab

	// Automatic Updates tab
	
	if (HANDLER.m_pgPageUpdate->IsWindow())
	{
		HANDLER.Config.UpdateCheckOnStartup = HANDLER.m_pgPageUpdate->IsDlgButtonChecked(IDC_UPDATE_CHECKONSTARTUP) ? TRUE:FALSE;
		HANDLER.Config.UpdateDontPrompt = HANDLER.m_pgPageUpdate->IsDlgButtonChecked(IDC_UPDATE_DONTPROMPT) ? TRUE:FALSE;
		HANDLER.Config.UpdateShowMessages = HANDLER.m_pgPageUpdate->IsDlgButtonChecked(IDC_UPDATE_SHOWMESSAGES) ? TRUE:FALSE;
	}

	// end of Automatic Updates tab
	
	if (Id != ID_APPLY_NOW)
	{
		HANDLER.SaveConfig();
		HANDLER.LoadConfig();
	}	

	if (m_fRestartSSH && HANDLER.SSH_IsActive() && (MessageBox("In order for some changes to become active, SSH server needs to be restarted.\nWould you like to do that now?", "freeSSHd settings", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES))
	{
		HANDLER.SSH_Stop();
		HANDLER.SSH_Start();
	}
	else
		if (HANDLER.SSH_IsActive())
			ReloadSSH();
	
	
	if (m_fRestartTelnet && HANDLER.Telnet_IsActive() && (MessageBox( "In order for some changes to become active, Telnet server needs to be restarted.\nWould you like to do that now?", "freeSSHd settings", MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES))
	{
		HANDLER.Telnet_Stop();
		HANDLER.Telnet_Start();
	}
	else
		if (HANDLER.Telnet_IsActive())
			ReloadTelnet();
	
	SetMsgHandled(FALSE);
	
  return FALSE;
}
