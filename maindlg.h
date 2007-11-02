// maindlg.h : interface of the CSettingsDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include "freeSSHdHandler.h"
#include "ResizablePropertySheet.h"

#if !defined(AFX_MAINDLG_H__44A6BE96_E0FC_4283_A243_44C30B66EC2D__INCLUDED_)
#define AFX_MAINDLG_H__44A6BE96_E0FC_4283_A243_44C30B66EC2D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define HANDLER _Module.freeSSHdHandler


class CSettingsDlg : public CResizablePropertySheetImpl<CSettingsDlg> 
{
private:
	bool HasCentered;
public:
	CSettingsDlg(::ATL::_U_STRINGorID title = (LPCTSTR)NULL, UINT uStartPage = 0, HWND hWndParent = NULL, bool EnableDoubleBuffering = false, bool IsWizard = false)
		: CResizablePropertySheetImpl<CSettingsDlg>(title, uStartPage, hWndParent, EnableDoubleBuffering, IsWizard),
		HasCentered (false), m_fRestartSSH(false), m_fRestartTelnet(false){
	}

	bool m_fRestartSSH;
	bool m_fRestartTelnet;

	void OnShowWindow(BOOL, int);
	BOOL OnOK(WORD Code, WORD Id, HWND hWnd);
	BOOL OnApply(WORD Code, WORD Id, HWND hWnd);

	BEGIN_MSG_MAP_EX(CSettingsDlg)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK);
		COMMAND_ID_HANDLER_EX(ID_APPLY_NOW, OnApply)
		CHAIN_MSG_MAP(CResizablePropertySheetImpl<CSettingsDlg>)
	END_MSG_MAP()
		

};
 
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__44A6BE96_E0FC_4283_A243_44C30B66EC2D__INCLUDED_)
