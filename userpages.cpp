#include "stdafx.h"
#include "user.h"
#include "userpages.h"

BOOL CUserPage1::OnQueryCancel()
{
	return FALSE;
}

LRESULT CUserPage1::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_LoginBox = GetDlgItem(IDC_USERPAGE1_LOGIN);
	m_AuthBox = GetDlgItem(IDC_USERPAGE1_AUTH);
	m_PassBox = GetDlgItem(IDC_USERPAGE1_PASSWORD);
	m_Pass2Box = GetDlgItem(IDC_USERPAGE1_PASSWORD2);
	m_DomainBox = GetDlgItem(IDC_USERPAGE1_DOMAIN);

	m_PassBox.SetPasswordChar('*');
	m_Pass2Box.SetPasswordChar('*');
	
	m_AuthBox.AddString("NT authentication");
	m_AuthBox.AddString("Password stored as SHA1 hash");
	m_AuthBox.AddString("Public key (SSH only)");

	m_AuthBox.SetCurSel(0);
	
	// configuration reading

	m_LoginBox.SetWindowText(m_Parent->m_User->GetName());
	m_AuthBox.SetCurSel(m_Parent->m_User->GetAuthType());
	
	CheckDlgButton(IDC_USERPAGE1_SHELL, m_Parent->m_User->HasShellAccess());
	CheckDlgButton(IDC_USERPAGE1_SFTP, m_Parent->m_User->HasSFTPAccess());
	CheckDlgButton(IDC_USERPAGE1_TUNNELING, m_Parent->m_User->HasTunnelAccess());

	if (m_Parent->m_User->GetAuthType() == 0)
	{
		m_PassBox.EnableWindow(FALSE);
		m_Pass2Box.EnableWindow(FALSE);

		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);

		m_DomainBox.SetWindowText(m_Parent->m_User->GetDomain());
		
	}

	if (m_Parent->m_User->GetAuthType() == 1)
	{
		if (m_Parent->m_User->IsPasswordSet())
		{	
			if (VisualStyles())
				Edit_SetCueBannerText(m_PassBox, L" Start typing here to change password");	
			else
			{
				m_PassBox.SetPasswordChar(0);
				m_PassBox.SetWindowText("[Press a key here to change password]");
			}

			m_Pass2Box.EnableWindow(FALSE);
			::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);
			
			m_fPassChanged = false;
		}
		else
			m_fPassChanged = true;
		
		
		m_DomainBox.EnableWindow(FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_DOMAIN), FALSE);
	}

	if (m_Parent->m_User->GetAuthType() == 2)
	{
		m_DomainBox.EnableWindow(FALSE);
		m_PassBox.EnableWindow(FALSE);
		m_Pass2Box.EnableWindow(FALSE);

		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_DOMAIN), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);
	}


	m_fInit = true;

	SetMsgHandled(FALSE);

	return TRUE;
}

LRESULT CUserPage1::OnAuthChange(WORD wCode, WORD wID, HWND hWnd)
{
	if (m_AuthBox.GetCurSel() == 0)
	{
		m_PassBox.EnableWindow(FALSE);
		m_Pass2Box.EnableWindow(FALSE);

		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);

		m_DomainBox.EnableWindow();
		
	}

	if (m_AuthBox.GetCurSel() == 1)
	{
		m_PassBox.EnableWindow();
		m_Pass2Box.EnableWindow();
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD), TRUE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), TRUE);
		
		if ( (m_Parent->m_User->GetAuthType() == 1) && ( m_Parent->m_User->IsPasswordSet() ) && !m_fPassChanged)
		{	
			m_Pass2Box.EnableWindow(FALSE);
			::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);		
		}
			
		m_DomainBox.EnableWindow(FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_DOMAIN), FALSE);
	}

	if (m_AuthBox.GetCurSel() == 2)
	{
		m_DomainBox.EnableWindow(FALSE);
		m_PassBox.EnableWindow(FALSE);
		m_Pass2Box.EnableWindow(FALSE);

		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_DOMAIN), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD), FALSE);
		::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), FALSE);
	}

	return TRUE;
}

LRESULT CUserPage1::OnPasswordChange(WORD wCode, WORD wID, HWND hWnd)
{
	if (m_fInit)
		if (!m_fPassChanged)
		{
			char strTmp[1024];
			
			if (GetDlgItemText(IDC_USERPAGE1_PASSWORD, strTmp, 1024))
			{
				m_Pass2Box.EnableWindow(TRUE);
				::EnableWindow(GetDlgItem(IDC_LABEL_USERPAGE1_PASSWORD2), TRUE);
				
				if (VisualStyles())
					Edit_SetCueBannerText(GetDlgItem(IDC_USERPAGE1_PASSWORD), _L(""));
				else
				{
					m_PassBox.SetPasswordChar('*');
					m_PassBox.SetWindowText("");
				}

				m_fPassChanged = true;
			}

		}

	return TRUE;
}