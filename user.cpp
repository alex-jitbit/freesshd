
#include "stdafx.h"
#include "resource.h"
#include "user.h"
#include "userpages.h"

void ShowBallonWarning(HWND Edit_hWnd, LPWSTR pszText)
{
	LPWSTR pszTitle = L"Warning";
	
	if (VisualStyles())
	{
		EDITBALLOONTIP balloon;
	 
		
		balloon.cbStruct = sizeof(EDITBALLOONTIP); 
		balloon.pszText = pszText; 
		balloon.pszTitle = pszTitle; 
		balloon.ttiIcon = TTI_WARNING; 
		
		Edit_ShowBalloonTip(Edit_hWnd, &balloon);
	}
	else
	{
		MessageBoxW(Edit_hWnd, pszText, pszTitle, MB_ICONWARNING);
	}

}	

void ShowBallonError(HWND Edit_hWnd, LPWSTR pszText)
{
	LPWSTR pszTitle = L"Error";
	
	if (VisualStyles())
	{
		EDITBALLOONTIP balloon;
	 
		
		balloon.cbStruct = sizeof(EDITBALLOONTIP); 
		balloon.pszText = pszText; 
		balloon.pszTitle = pszTitle; 
		balloon.ttiIcon = TTI_ERROR; 
		
		Edit_ShowBalloonTip(Edit_hWnd, &balloon);
	}
	else
	{
		MessageBoxW(Edit_hWnd, pszText, pszTitle, MB_ICONERROR);
	}

}

void CUserDlg::OnShowWindow(BOOL, int)
{
	SetIcon(LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_PAGE_USERS)));
	CenterWindow(GetDesktopWindow());
	SetMsgHandled(FALSE);
}

BOOL CUserDlg::OnOK(WORD Code, WORD Id, HWND hWnd)
{
	// General tab
	
	char strLogin[256];
	
	if (m_pgPage1->m_LoginBox.GetWindowText(strLogin, 256))
	{
		strLogin[255] = 0;
		if (HANDLER.GetUser(strLogin) && m_fNewUser)
		{
			ShowBallonError(m_pgPage1->m_LoginBox.m_hWnd, L"User already exists.");
			return FALSE;
		}
		m_User->SetName(strLogin);
	}
	else
	{
		ShowBallonError(m_pgPage1->m_LoginBox.m_hWnd, L"User must have a login name.");
		return FALSE;
	}

	if ( (m_pgPage1->m_AuthBox.GetCurSel() == 1) && (m_pgPage1->m_fPassChanged) )
	{
		char strPass1[1024];
		char strPass2[1024];

		if (!m_pgPage1->m_PassBox.GetWindowText(strPass1, 1024))
		{
			ShowBallonError(m_pgPage1->m_PassBox.m_hWnd, L"You must specify a password.");
			return FALSE;
		}
		
		m_pgPage1->m_Pass2Box.GetWindowText(strPass2, 1024);
		
		strPass1[1023] = 0;
		strPass2[1023] = 0;

		if ( strcmp(strPass1, strPass2) == 0)
		{
			m_User->SetAuthType(1);
			m_User->SetPassword(strPass1);
		}
		else
		{
			ShowBallonError(m_pgPage1->m_Pass2Box.m_hWnd, L"Passwords aren't identical.");
			return FALSE;
		}
	}
	
	m_User->SetAuthType(m_pgPage1->m_AuthBox.GetCurSel());
	
	char strDomain[256];

	if (m_pgPage1->m_DomainBox.GetWindowText(strDomain, 256))
	{
		strDomain[256] = 0;
		m_User->SetDomain(strDomain);
	}
	
	m_User->SetShellAccess(m_pgPage1->IsDlgButtonChecked(IDC_USERPAGE1_SHELL));
	m_User->SetSFTPAccess(m_pgPage1->IsDlgButtonChecked(IDC_USERPAGE1_SFTP));
	m_User->SetTunnelAccess(m_pgPage1->IsDlgButtonChecked(IDC_USERPAGE1_TUNNELING));
	
	SetMsgHandled(FALSE);
	
	return TRUE;
}
