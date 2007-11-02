// MessageDlg.h: interface for the CMessageDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEDLG_H__81E8CDF6_261C_4A1D_ABA2_D6FD853224B9__INCLUDED_)
#define AFX_MESSAGEDLG_H__81E8CDF6_261C_4A1D_ABA2_D6FD853224B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

class CMessageDlg: public CDialogImpl<CMessageDlg>, public CDialogResize<CMessageDlg>
{
public:
	enum { IDD = IDD_MESSAGEDLG };
	
	CMessageDlg();
	virtual ~CMessageDlg();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	BEGIN_MSG_MAP(CMessageDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK);
		CHAIN_MSG_MAP(CDialogResize<CMessageDlg>)
	END_MSG_MAP()	

	BEGIN_DLGRESIZE_MAP(CMessageDlg)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_MESSAGEDLG_CAPTION, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_MESSAGEDLG_TEXT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

	char m_Text[1024];
	char m_Caption[1024];
	
	void SetText(char *Text);
	void SetCaption(char *Caption);
};

#endif // !defined(AFX_MESSAGEDLG_H__81E8CDF6_261C_4A1D_ABA2_D6FD853224B9__INCLUDED_)
