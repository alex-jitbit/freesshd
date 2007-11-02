#ifndef __ATL_RESIZABLE_PROPERTY_SHEET_H__ 
#define __ATL_RESIZABLE_PROPERTY_SHEET_H__ 

#pragma once

#define PACKVERSION(major,minor) MAKELONG(minor,major)

BOOL VisualStyles();
DWORD GetDllVersion(LPCTSTR lpszDllName);

// Resizable Property Sheet Implemenation based on CPropertySheetImpl by
// Benjamin Kalytta (http://www.kalytta.com)
// Version: 1.1 
// modifyed by free2000fly 

#ifndef _WTL_NEW_PAGE_NOTIFY_HANDLERS 
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS 
#endif  // _WTL_NEW_PAGE_NOTIFY_HANDLERS 

#if _WIN32_WINNT  <  0x0501
#pragma message ("Double buffered windows using WS_EX_COMPOSITED requires Windows XP and higher, I'll set _WIN32_WINNT 0x0501")
#define _WIN32_WINNT 0x0501
#endif

#ifndef __ATLCRACK_H__
#include  < atlcrack.h > 
#endif

namespace WTL 
{ 

#define BEGIN_DLGRESIZE_MAP_EX(thisClass) \
	static void *ResizeMapStart; \
	static int ResizeMapIdx; \
	static const _AtlDlgResizeMap* GetDlgResizeMap() { \
		static _AtlDlgResizeMap ResizeMap[] = { 

#define ALT_DLGRESIZE_MAP_EX() \
		{ -1, 0 }, \

#define END_DLGRESIZE_MAP_EX() \
			{ -1, 0 }, \
			{ -1, 0 }, \
		}; \
		ResizeMapStart = ResizeMap; \
		return (_AtlDlgResizeMap*) (&ResizeMap[ResizeMapIdx]); \
	}


template  < class T, class TBase = CPropertySheetWindow > 
class ATL_NO_VTABLE CResizablePropertySheetImpl 
    : public CPropertySheetImpl < T, TBase  >  
    , public CDialogResize < T >  
    , public CMessageFilter 
{ 
private:
	bool EnableDoubleBuffering;
	bool IsWizard;
	
	enum {
		ID_PSHEET_OK = IDOK,
		ID_PSHEET_APPLY = ID_APPLY_NOW,
		ID_PSHEET_CANCEL = IDCANCEL,
		ID_PSHEET_HELP = IDHELP,
		ID_PSHEET_TAB = ATL_IDC_TAB_CONTROL,
		ID_PSHEET_PREV = ID_WIZBACK,
		ID_PSHEET_NEXT = ID_WIZNEXT,
		ID_PSHEET_FINISH = ID_WIZFINISH,
		ID_PSHEET_BOTTOMFRAME = ID_WIZFINISH + 1,
		ID_PSHEET_TOPFRAME = ID_WIZFINISH + 2,
	};
	
public:

	CResizablePropertySheetImpl(ATL::_U_STRINGorID title = (LPCTSTR) NULL, UINT uStartPage = 0, 
        HWND hWndParent = NULL, bool EnableDoubleBuffering = false, bool IsWizard = false) 
        : CPropertySheetImpl < T, TBase >  (title, uStartPage, hWndParent) 
    {
		this->EnableDoubleBuffering = EnableDoubleBuffering;
		this->IsWizard = IsWizard;
		if (IsWizard) {
			SetWizardMode();
		}
	}

	static int CALLBACK PropSheetCallback(HWND hWnd, UINT uMsg, LPARAM lParam) 
    {
		if (uMsg == PSCB_PRECREATE) { 
            LPDLGTEMPLATE Template = (LPDLGTEMPLATE) lParam;
			// remove dialog border styles
			Template->style &= ~DS_MODALFRAME;

			// add child window and clipping styles
			Template->style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;
			return 0;
		} 
		return CPropertySheetImpl < T > ::PropSheetCallback(hWnd, uMsg, lParam);
	}

    void OnSheetInitialized()
    {
        //_Module.GetMessageLoop()->AddMessageFilter(this); 
    }

    virtual void OnFinalMessage(HWND hWnd) 
    { 
//         hWnd; 
//         CMessageLoop* pLoop = _Module.GetMessageLoop(); 
//         ATLASSERT(pLoop != NULL);
//         pLoop- > RemoveMessageFilter(this); 
    } 

    void SetWizardMode() {
		SetActiveResizeMap(1);
		IsWizard = true;
		CPropertySheetImpl < T > ::SetWizardMode();
	}

	void SetHeader(LPCTSTR szbmHeader) {
		SetActiveResizeMap(1);
		IsWizard = true;
		CPropertySheetImpl < T > ::SetHeader(szbmHeader);
	}

	void SetHeader(HBITMAP hbmHeader) {
		SetActiveResizeMap(1);
		IsWizard = true;
		return CPropertySheetImpl < T > ::SetHeader(hbmHeader);
	}
	
	void SetWatermark(LPCTSTR szbmWatermark, HPALETTE hplWatermark = NULL) {
		SetActiveResizeMap(1);
		IsWizard = true;
		CPropertySheetImpl < T > ::SetWatermark(szbmWatermark, hplWatermark);
	}

	void SetWatermark(HBITMAP hbmWatermark, HPALETTE hplWatermark = NULL) {
		SetActiveResizeMap(1);
		IsWizard = true;
		return CPropertySheetImpl < T > ::SetWatermark(hbmWatermark, hplWatermark);
	}

	// This is required to handle modeless Dialog messages correctly 
	virtual BOOL PreTranslateMessage(LPMSG pMsg) 
    { 
		if (IsDialogMessage(pMsg)) {
			if (!::IsWindow(m_hWnd) || GetActivePage() == NULL) {
				// Not really clean code, but better than creating an 
				// extra property sheet Message Loop class 
				PostQuitMessage(0);
			}
			return TRUE;
		}
		return FALSE;
	}

	// --- CDialogResize enhancement --- 

	void SetActiveResizeMap(int MapIdx) 
    {
		if (ResizeMapStart == NULL) {
			// ResizeMapStart not yet initialized 
			GetDlgResizeMap();
		}
		int Idx = 0;
		bool WasMapEnd = false;
		while (true) 
        {
			if (((_AtlDlgResizeMap*) ResizeMapStart)[Idx].m_nCtlID == -1 && 
                ((_AtlDlgResizeMap*) ResizeMapStart)[Idx].m_dwResizeFlags == 0) 
            {
				if (WasMapEnd) break;
				MapIdx--;
				if (MapIdx == 0) {
					ResizeMapIdx = Idx + 1;
					break;
				}
				WasMapEnd = true;
			} else {
				WasMapEnd = false;
			}
			Idx++;
		}
	}
	
	void OnSize(WPARAM wParam, CSize Size) 
    { 
		// Resize Property Sheet controls manually first 
		BOOL Handled = FALSE;
		CDialogResize < T > ::OnSize(WM_SIZE, 0, MAKELONG(Size.cx, Size.cy), Handled);
		UpdatePropertyPage(GetActivePage());
		SetMsgHandled(FALSE);
	}

	LRESULT OnSetActive(LPNMHDR Hdr) 
    {
		UpdatePropertyPage((HWND) ((LPPSHNOTIFY) Hdr)->lParam);
		SetMsgHandled(FALSE);
		return TRUE;
	} 

    LRESULT OnWmShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    { 
        uMsg, wParam, lParam, bHandled; 
        { 
            DlgResize_Init();
            if (EnableDoubleBuffering) {
                SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_COMPOSITED);
            }
        } 

        bHandled = FALSE; 
        return S_OK; 
    } 

	BEGIN_MSG_MAP_EX(CResizablePropertySheetImpl)
        MESSAGE_HANDLER(WM_SHOWWINDOW, OnWmShowWindow) 
		MSG_WM_SIZE(OnSize);
		NOTIFY_CODE_HANDLER_EX(PSN_SETACTIVE, OnSetActive)
		CHAIN_MSG_MAP(CPropertySheetImpl < T > )
		CHAIN_MSG_MAP(CDialogResize < T > )
	END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP_EX(CResizablePropertySheetImpl)
		DLGRESIZE_CONTROL(ID_PSHEET_OK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_APPLY, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_CANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_HELP, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_TAB, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	ALT_DLGRESIZE_MAP_EX()
		DLGRESIZE_CONTROL(ID_PSHEET_PREV, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_NEXT, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_FINISH, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_CANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_HELP, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_TOPFRAME, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(ID_PSHEET_BOTTOMFRAME, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_PSHEET_BOTTOMFRAME, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP_EX()
private:

	void UpdatePropertyPage(HWND hWnd) 
    {
		// Adjust property page size 
		CPropertyPageWindow Wnd = hWnd;
		CSize PageMargin;

		if (Wnd) {
			RECT rc, rctf, rcbf;
			GetClientRect(&rc);
			
			int Width = 0;
			int Height = 0;
			int Top = 0;
			int Left = 0;

			if (IsWizard) {
                ::GetWindowRect(GetDlgItem(ID_PSHEET_TOPFRAME), &rctf);
                ::GetWindowRect(GetDlgItem(ID_PSHEET_BOTTOMFRAME), &rcbf);
				ScreenToClient(&rctf);
				ScreenToClient(&rcbf);

				PageMargin.cx = 11;
				PageMargin.cy = 11;

				if (::GetProp(hWnd, _T("IsExterior"))) {
					Left = 0;
					Top = 0;
					Width = rc.right;
					Height = rcbf.top;
				} else {
					Top = rctf.top + PageMargin.cy;
					Left = PageMargin.cx;
					Width = rc.right - Left - PageMargin.cx;
					Height = rcbf.top - Top - PageMargin.cy;
				}
			} else {
				RECT rci;
				CTabCtrl Tab = GetTabControl();
				Tab.GetItemRect(HwndToIndex(Wnd), &rci);
				Tab.GetWindowRect(&rc);
				ScreenToClient(&rc);
				
				PageMargin.cx = 4;
				PageMargin.cy = 4;

				Top = rc.top + rci.bottom + PageMargin.cy;
				Left = rc.left + PageMargin.cx;
				Width = rc.right - PageMargin.cx - Left;
				Height = rc.bottom - PageMargin.cy - Top;
			}
			Wnd.SetWindowPos(NULL, Left, Top, Width, Height, SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
};

// Static class members declaration and initialization  

template  < class T, class TBase >  void *CResizablePropertySheetImpl < T, TBase > ::ResizeMapStart = NULL; 
template  < class T, class TBase >  int CResizablePropertySheetImpl < T, TBase > ::ResizeMapIdx = 0; 

template  < class T, class TBase = CPropertyPageWindow > 
class ATL_NO_VTABLE CResizablePropertyPageImpl 
    : public CPropertyPageImpl < T, TBase  > , public CDialogResize < T >  
{
private:
	bool IsDoubleBufferEnabled;
public:
	CResizablePropertyPageImpl(ATL::_U_STRINGorID title = (LPCTSTR) NULL, 
        bool IsExterior = false, bool bDoubleBuffer = false) 
        : CPropertyPageImpl < T >  (title) 
    {
		if (IsExterior) m_psp.dwFlags |= PSP_HIDEHEADER;
		IsDoubleBufferEnabled = bDoubleBuffer;
	}
	
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam) {
		if (m_psp.dwFlags & PSP_HIDEHEADER) {
			SetProp(m_hWnd, _T("IsExterior"), (HANDLE) 1);
		}
		if (IsDoubleBufferEnabled) {
			SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_COMPOSITED);
		}
		DlgResize_Init(false, false);
		SetMsgHandled(FALSE);
		return FALSE;
	}

	void OnDestroy() {
		RemoveProp(m_hWnd, _T("IsExterior"));
		SetMsgHandled(false);
	}

	void EnableDoubleBuffering() {
		if (m_hWnd) {
			SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) | WS_EX_COMPOSITED);
		} else {
			IsDoubleBufferEnabled = true;
		}
	}

	void DisableDoubleBuffering() {
		if (m_hWnd) {
			SetWindowLongPtr(GWL_EXSTYLE, GetWindowLongPtr(GWL_EXSTYLE) & ~WS_EX_COMPOSITED);
		} else {
			IsDoubleBufferEnabled = false;
		}
	}

	void AddPageFlags(UINT Flags) {
		m_psp.dwFlags |= Flags;
	}

	void RemovePageFlags(UINT Flags) {
		m_psp.dwFlags &= ~Flags;
	}

	BEGIN_MSG_MAP_EX(CResizablePropertyPageImpl)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy);
		// forward WM_NOTIFY message to parent (Property Sheet) manually 
		if (uMsg == WM_NOTIFY) {
			// since lParam in LPPSHNOTIFY struct isn't used, we'll use it 
			if (((LPNMHDR) lParam)->code == PSN_SETACTIVE) {
				((LPPSHNOTIFY) lParam)->lParam = (LPARAM) m_hWnd;
				::SendMessage(GetParent(), uMsg, wParam, lParam);
			}
		}
		CHAIN_MSG_MAP(CPropertyPageImpl < T > )
		CHAIN_MSG_MAP(CDialogResize < T > )
	END_MSG_MAP()
}; 


}; // namespace WTL 


#endif  // __ATL_RESIZABLE_PROPERTY_SHEET_H__ 