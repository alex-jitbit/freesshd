#include "resource.h"
#include "ResizablePropertySheet.h"

// Global event handlers

#define HANDLER _Module.freeSSHdHandler

#ifdef _ISFREEOPEN
	LPSTR GetErrorString(int err);
#endif

class CPageStatus : public CResizablePropertyPageImpl<CPageStatus> 
{
	
public:
	enum { IDD = IDD_PAGE_STATUS };
	HICON m_hiconOK;
	HICON m_hiconBAD;
	HBITMAP m_hbmpLogo;
	HDC m_hdcLogo;

	CHyperLink m_TelnetStartStop;
	CHyperLink m_SSHStartStop;
	
	CPageStatus(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageStatus>(title, IsExterior, EnableDoubleBuffer), m_hbmpLogo(NULL) {}
	~CPageStatus()
	{
		if (m_hbmpLogo)
			DeleteObject(m_hbmpLogo);
	}

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnTelnetStatusLineClicked(WORD Code, WORD Control, HWND hWnd);
	LRESULT OnSSHStatusLineClicked(WORD Code, WORD Control, HWND hWnd);
	BOOL OnSetActive();
	BOOL OnQueryCancel();
	BOOL OnApply();


	BOOL UpdateTelnetInfo(BOOL Disconnected = FALSE);
	BOOL UpdateSSHInfo(BOOL Disconnected = FALSE);

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_STATUS_TELNET_LINE1, BN_CLICKED, OnTelnetStatusLineClicked)
		COMMAND_HANDLER_EX(IDC_STATUS_TELNET_LINE2, BN_CLICKED, OnTelnetStatusLineClicked)
		COMMAND_HANDLER_EX(IDC_STATUS_SSH_LINE1, BN_CLICKED, OnSSHStatusLineClicked)
		COMMAND_HANDLER_EX(IDC_STATUS_SSH_LINE2, BN_CLICKED, OnSSHStatusLineClicked)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageStatus>)
		CHAIN_MSG_MAP(CDialogResize<CPageStatus>)
	END_MSG_MAP()
	
	BEGIN_DLGRESIZE_MAP(CPageStatus)
		DLGRESIZE_CONTROL(IDC_LABEL_VERSION, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_LABEL_POWEREDBY, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_WEONLYDOLOGO, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_STATUS_TELNET, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS_TELNET_LINE1, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS_TELNET_LINE2, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS_SSH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS_SSH_LINE1, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS_SSH_LINE2, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()
};

class CPageTelnet : public CResizablePropertyPageImpl<CPageTelnet> 
{
public:

	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_TELNET };

	CPageTelnet(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageTelnet>(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL) {}

	
	LRESULT OnChange(WORD Code, WORD Control, HWND hWnd);
	LRESULT OnBrowse(WORD Code, WORD Control, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnSetActive();
	BOOL OnQueryCancel();
	
//	LRESULT OnEditSetFocus(WORD Code, WORD Control, HWND hWnd);

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnChange)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnChange)
		COMMAND_HANDLER_EX(IDC_TELNET_AUTOSTART, BN_CLICKED, OnChange)
		COMMAND_HANDLER_EX(IDC_TELNET_NEWCONSOLE, BN_CLICKED, OnChange)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnBrowse)
		//COMMAND_CODE_HANDLER_EX(EN_SETFOCUS, OnEditSetFocus)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageTelnet>)
		CHAIN_MSG_MAP(CDialogResize<CPageTelnet>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageTelnet)
	  //DLGRESIZE_CONTROL(IDC_TELNET_LISTENADDRESS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TELNET_BANNERMESSAGE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TELNET_BANNERMESSAGE_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TELNET_CMD, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TELNET_CMD_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageSSH : public CResizablePropertyPageImpl<CPageSSH> {
public:

	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_SSH };

	CPageSSH(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageSSH>(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL) {}
	
	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnBrowse(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnNewRSAKey(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnNewDSAKey(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnSetActive();
	BOOL OnQueryCancel();
	void CheckHostKeys();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_SSH_RSAKEY_NEW, BN_CLICKED, OnNewRSAKey)
		COMMAND_HANDLER_EX(IDC_SSH_DSAKEY_NEW, BN_CLICKED, OnNewDSAKey)
		COMMAND_HANDLER_EX(IDC_SSH_AUTOSTART,BN_CLICKED, OnChange)
		COMMAND_HANDLER_EX(IDC_SSH_NEWCONSOLE,BN_CLICKED, OnChange)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnBrowse)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnChange)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageSSH>)
		CHAIN_MSG_MAP(CDialogResize<CPageSSH>)
	END_MSG_MAP()
	
	BEGIN_DLGRESIZE_MAP(CPageSSH)
	  //DLGRESIZE_CONTROL(IDC_SSH_LISTENADDRESS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SSH_BANNERMESSAGE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SSH_BANNERMESSAGE_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SSH_CMD, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SSH_CMD_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SSH_RSAKEY, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_SSH_RSAKEY_STATUS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SSH_RSAKEY_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SSH_RSAKEY_NEW, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SSH_DSAKEY, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_SSH_DSAKEY_STATUS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SSH_DSAKEY_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SSH_DSAKEY_NEW, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_SSH_RSAKEY_BITS, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_SSH_DSAKEY_BITS, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageUsers : public CResizablePropertyPageImpl<CPageUsers> 
{
public:
	
	static int WINAPI ListBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WNDPROC m_ListBoxWndProc;
	
	CListBox m_UsersBox;
	HICON m_hPageIcon, m_hYes, m_hNo, m_hUser;
	enum { IDD = IDD_PAGE_USERS };

	CPageUsers(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageUsers>(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}
	
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnMeasureItem(UINT CtlId, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	LRESULT OnDrawItem(UINT CtlId, LPDRAWITEMSTRUCT lpDrawItemStruct);
	BOOL OnSetActive();
	BOOL OnQueryCancel();
	LRESULT OnAddUser(UINT Code, int Control, HWND hWnd);
	LRESULT OnChangeUser(UINT Code, int Control, HWND hWnd);
	LRESULT OnRemoveUser(UINT Code, int Control, HWND hWnd);
	LRESULT OnItemActivate(UINT Code, int Control, HWND hWnd);

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		COMMAND_HANDLER_EX(IDC_USERS_ADD, BN_CLICKED, OnAddUser)
		COMMAND_HANDLER_EX(IDC_USERS_CHANGE, BN_CLICKED, OnChangeUser)
		COMMAND_HANDLER_EX(IDC_USERS_REMOVE, BN_CLICKED, OnRemoveUser)
		COMMAND_ID_HANDLER_EX(IDC_USERS_LIST, OnItemActivate)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageUsers>)
		CHAIN_MSG_MAP(CDialogResize<CPageUsers>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageUsers)
		DLGRESIZE_CONTROL(IDC_USERS_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LABEL_USERS_SHELL, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_USERS_SFTP, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LABEL_USERS_TUNNEL, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageHostRestrictions : public CResizablePropertyPageImpl<CPageHostRestrictions> {
public:

	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_HOSTRESTRICTIONS };
 
	CPageHostRestrictions(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageHostRestrictions >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL) {}
	
	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnQueryCancel();
	

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnChange)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageHostRestrictions>)
		CHAIN_MSG_MAP(CDialogResize<CPageHostRestrictions>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageHostRestrictions)
		DLGRESIZE_CONTROL(IDC_HOSTRESTRICTIONS_IPADDRESSES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LABEL_HOSTRESTRICTIONS_HINT, DLSZ_SIZE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageCrypto : public CResizablePropertyPageImpl<CPageCrypto> {
public:

	HICON m_hPageIcon;

	enum { IDD = IDD_PAGE_CRYPTO };
 
	CPageCrypto(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageCrypto >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}

	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnQueryCancel();
	
	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageCrypto>)
		CHAIN_MSG_MAP(CDialogResize<CPageCrypto>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageCrypto)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_CRYPTO_CIPHER_GROUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_CRYPTO_MACS_GROUP, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()
};

class CPageSFTP : public CResizablePropertyPageImpl<CPageSFTP> {
public:

	HICON m_hPageIcon;

	enum { IDD = IDD_PAGE_SFTP };
 
	CPageSFTP(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageSFTP >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}

	LRESULT OnEditChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnBrowse(WORD Code, WORD Control, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_SFTP_HOME_BROWSE, BN_CLICKED, OnBrowse)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageSFTP>)
		CHAIN_MSG_MAP(CDialogResize<CPageSFTP>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageSFTP)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SFTP_HOME, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SFTP_HOME_BROWSE, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageTunneling : public CResizablePropertyPageImpl<CPageTunneling> {
public:

	HICON m_hPageIcon;

	enum { IDD = IDD_PAGE_TUNNELING };
 
	CPageTunneling(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageTunneling >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnLocalTunnelingChecked(WORD wID, WORD wCode, HWND hWnd, BOOL &bHandled);
	LRESULT OnRemoteTunnelingChecked(WORD wID, WORD wCode, HWND hWnd, BOOL &bHandled);
	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER(IDC_TUNNELING_LOCAL, BN_CLICKED, OnLocalTunnelingChecked);
		COMMAND_HANDLER(IDC_TUNNELING_REMOTE, BN_CLICKED, OnRemoteTunnelingChecked);
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageTunneling>)
		CHAIN_MSG_MAP(CDialogResize<CPageTunneling>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageTunneling)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageAuth : public CResizablePropertyPageImpl<CPageAuth> {
public:

	HICON m_hPageIcon;

	enum { IDD = IDD_PAGE_AUTH };
 
	CPageAuth(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageAuth >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnChange(WORD Code, WORD wID, HWND hWnd);
	LRESULT OnBrowse(WORD Code, WORD wID, HWND hWnd);
	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnChange)
		COMMAND_HANDLER_EX(IDC_AUTH_PUBLICKEYFOLDER_BROWSE, BN_CLICKED, OnBrowse)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageAuth>)
		CHAIN_MSG_MAP(CDialogResize<CPageAuth>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageAuth)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_AUTH_PUBLICKEYFOLDER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_AUTH_PUBLICKEYFOLDER_BROWSE, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageLogging : public CResizablePropertyPageImpl<CPageLogging> {
public:

	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_LOGGING };
 
	CPageLogging(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageLogging >(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}
	
	LRESULT OnBrowse(WORD Code, WORD Control, HWND hWnd);
	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnLogFileChecked(WORD wID, WORD wCode, HWND hWnd);
	LRESULT OnLogFileOpen(WORD wID, WORD wCode, HWND hWnd);

	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnChange)
		COMMAND_HANDLER_EX(IDC_LOGGING_LOGFILE_BROWSE, BN_CLICKED, OnBrowse)
		COMMAND_HANDLER_EX(IDC_LOGGING_LOGEVENTS, BN_CLICKED, OnLogFileChecked)
		COMMAND_HANDLER_EX(IDC_LOGGING_LOGFILE_OPEN, BN_CLICKED, OnLogFileOpen)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageLogging>)
		CHAIN_MSG_MAP(CDialogResize<CPageLogging>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageLogging)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LOGGING_LOGFILE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LOGGING_LOGFILE_BROWSE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LOGGING_LOGFILE_OPEN, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LOGGING_GROUP, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()
};

class CPageOnlineUsers : public CResizablePropertyPageImpl<CPageOnlineUsers> 
{
public:
	CListViewCtrl m_UsersBox;
	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_ONLINEUSERS };

	BOOL AddSSHConnection(CSSHUser *User);
	BOOL AddTelnetConnection(CSSHUser *User);
	BOOL RemoveConnection(CSSHUser *User);
	
	CPageOnlineUsers(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageOnlineUsers>(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}
	
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	BOOL OnQueryCancel();
//	LRESULT OnItemActivate(UINT Code, int Control, HWND hWnd);
	LRESULT OnItemActivate(LPNMHDR nmhdr);
	LRESULT OnUserDisconnect(UINT Code, int Control, HWND hWnd);

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		NOTIFY_ID_HANDLER_EX(IDC_ONLINEUSERS_LIST, OnItemActivate)
		//COMMAND_ID_HANDLER_EX(IDC_ONLINEUSERS_LIST, OnItemActivate)
		COMMAND_ID_HANDLER_EX(IDC_ONLINEUSERS_DISCONNECT, OnUserDisconnect)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageOnlineUsers>)
		CHAIN_MSG_MAP(CDialogResize<CPageOnlineUsers>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageOnlineUsers)
		DLGRESIZE_CONTROL(IDC_ONLINEUSERS_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_ONLINEUSERS_DISCONNECT, DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()
};

class CPageUpdate : public CResizablePropertyPageImpl<CPageUpdate> 
{
public:
	HICON m_hPageIcon;
	enum { IDD = IDD_PAGE_UPDATE };

	CPageUpdate(::ATL::_U_STRINGorID title = (LPCTSTR) NULL, bool IsExterior = false, bool EnableDoubleBuffer = false) : CResizablePropertyPageImpl<CPageUpdate>(title, IsExterior, EnableDoubleBuffer), m_hPageIcon(NULL){}
	
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnChange(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnUpdateCheckNow(WORD wCode, WORD wID, HWND hWnd);
	LRESULT OnUpdateCheckOnStartupChecked(WORD wCode, WORD wID, HWND hWnd);

	BOOL OnQueryCancel();

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_UPDATE_CHECKONSTARTUP, BN_CLICKED, OnUpdateCheckOnStartupChecked)
		COMMAND_HANDLER_EX(IDC_UPDATE_CHECKNOW, BN_CLICKED, OnUpdateCheckNow)
		COMMAND_CODE_HANDLER_EX(BN_CLICKED, OnChange)
		CHAIN_MSG_MAP(CResizablePropertyPageImpl<CPageUpdate>)
		CHAIN_MSG_MAP(CDialogResize<CPageUpdate>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CPageUpdate)
		DLGRESIZE_CONTROL(IDC_PAGEICON, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_UPDATE_OPTIONS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_UPDATE_CHECKNOW, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()
};