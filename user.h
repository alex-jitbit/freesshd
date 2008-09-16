#ifndef _FREESSHD_USER_TAB_
#define _FREESSHD_USER_TAB_

#include "freeSSHdHandler.h"
#include "freeSSHdUser.h"
#include "ResizablePropertySheet.h"


#define HANDLER _Module.freeSSHdHandler

class CUserPage1;
class CUserPage2;

class CUserDlg : public CResizablePropertySheetImpl<CUserDlg> 
{
private:
	bool HasCentered;
public:
	bool m_fNewUser;
	CfreeSSHdUser *m_User;
	CUserPage1 *m_pgPage1;
//	CUserPage2 *m_pgPage2;

	CUserDlg(::ATL::_U_STRINGorID title = (LPCTSTR)NULL, UINT uStartPage = 0, HWND hWndParent = NULL, bool EnableDoubleBuffering = false, bool IsWizard = false)
		: CResizablePropertySheetImpl<CUserDlg>(title, uStartPage, hWndParent, EnableDoubleBuffering, IsWizard),
		HasCentered (false), m_User(NULL), m_fNewUser(false){
	}

	void OnShowWindow(BOOL, int);
	BOOL OnOK(WORD Code, WORD Id, HWND hWnd);

	BEGIN_MSG_MAP_EX(CUserDlg)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK);
		CHAIN_MSG_MAP(CResizablePropertySheetImpl<CUserDlg>)
	END_MSG_MAP()
		

};
 
#endif