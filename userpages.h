#ifndef _FREESSHD_USER_PAGES_
#define	_FREESSHD_USER_PAGES_

#include "resource.h"
#include "ResizablePropertySheet.h"

class CUserPage1 : public CResizablePropertyPageImpl<CUserPage1> 
{
	
public:
	bool m_fInit, m_fPassChanged;
	CUserDlg *m_Parent; 
	CComboBox m_AuthBox;
	CEdit m_LoginBox, m_PassBox, m_Pass2Box, m_DomainBox;

	enum { IDD = IDD_USERPAGE_1 };
	
	CUserPage1(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CUserPage1>(title, IsExterior, EnableDoubleBuffer),m_Parent(NULL), m_fInit(false), m_fPassChanged(true){}

	LRESULT OnAuthChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnPasswordChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_USERPAGE1_AUTH, CBN_SELCHANGE, OnAuthChange)
		COMMAND_HANDLER_EX(IDC_USERPAGE1_PASSWORD, EN_CHANGE, OnPasswordChange);
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CUserPage1>)
		CHAIN_MSG_MAP(CDialogResize<CUserPage1>)
	END_MSG_MAP()
	
	BEGIN_DLGRESIZE_MAP(CUserPage1)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_USERPAGE1_LOGIN, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_USERPAGE1_AUTH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_USERPAGE1_PASSWORD, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_USERPAGE1_PASSWORD2, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_USERPAGE1_DOMAIN, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()
};

#endif