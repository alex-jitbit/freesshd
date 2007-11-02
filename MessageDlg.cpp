// MessageDlg.cpp: implementation of the CMessageDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageDlg::CMessageDlg()
{

}

CMessageDlg::~CMessageDlg()
{

}

LRESULT CMessageDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DlgResize_Init();
	
	CenterWindow();
	SetActiveWindow();

	SetDlgItemText(IDC_MESSAGEDLG_TEXT, m_Text);
	SetDlgItemText(IDC_MESSAGEDLG_CAPTION, m_Caption);

	SetIcon(LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME)));
	
	return TRUE;
}

LRESULT CMessageDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	EndDialog(wID);
	return 0;
}

void CMessageDlg::SetText(char *Text)
{
	if (IsWindow())
		SetDlgItemText(IDC_MESSAGEDLG_TEXT, Text);

	strncpy(m_Text, Text, 1023);
}

void CMessageDlg::SetCaption(char *Caption)
{
	if (IsWindow())
		SetDlgItemText(IDC_MESSAGEDLG_CAPTION, Caption);

	strncpy(m_Caption, Caption, 1023);
}