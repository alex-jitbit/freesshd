#ifdef _ISFREEOPEN
extern _ATL_FUNC_INFO INFO_CloseApp;
extern _ATL_FUNC_INFO INFO_CheckDone;
extern _ATL_FUNC_INFO INFO_StateChange;
extern _ATL_FUNC_INFO INFO_UpdateDone;
extern _ATL_FUNC_INFO INFO_FileStart;
extern _ATL_FUNC_INFO INFO_FileProgress;
extern _ATL_FUNC_INFO INFO_FileDone;
extern _ATL_FUNC_INFO INFO_DownloadDone;
extern _ATL_FUNC_INFO INFO_PrevDetected;

class wodAppUpdateEvents: public IDispEventSimpleImpl<1, wodAppUpdateEvents, &WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents> 
{
public:
    wodAppUpdateEvents (WODAPPUPDATECOMLib::IwodAppUpdateComPtr pWodAppUpdateCom);
    virtual ~wodAppUpdateEvents ();

	void _stdcall CloseApp();
	void _stdcall CheckDone(long NewFiles, long ErrorCode, BSTR ErrorText);
	void _stdcall StateChange(WODAPPUPDATECOMLib::UpdateStates OldState);
	void _stdcall UpdateDone(long ErrorCode, BSTR ErrorText);
    void _stdcall FileStart(WODAPPUPDATECOMLib::IUpdFile* File);
	void _stdcall FileProgress(WODAPPUPDATECOMLib::IUpdFile* File, long Position, long Total);
	void _stdcall FileDone(WODAPPUPDATECOMLib::IUpdFile* File, long ErrorCode, BSTR ErrorText);
	void _stdcall DownloadDone(long ErrorCode, BSTR ErrorText);
    void _stdcall PrevDetected();
    
    BEGIN_SINK_MAP (wodAppUpdateEvents)
        SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 0, CloseApp, &INFO_CloseApp)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 1, CheckDone, &INFO_CheckDone)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 2, StateChange, &INFO_StateChange)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 3, UpdateDone, &INFO_UpdateDone)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 4, FileStart, &INFO_FileStart)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 5, FileProgress, &INFO_FileProgress)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 6, FileDone, &INFO_FileDone)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 7, DownloadDone, &INFO_DownloadDone)
		SINK_ENTRY_INFO (1, WODAPPUPDATECOMLib::DIID__IwodAppUpdateComEvents, 8, PrevDetected, &INFO_PrevDetected)
    END_SINK_MAP ()

private:
    WODAPPUPDATECOMLib::IwodAppUpdateComPtr m_pWodAppUpdateCom;
};
#else
	void  wodAppUpd_CheckDone(void *AppUpd, long NewFiles, long ErrorCode, char * ErrorText);
	void  wodAppUpd_DownloadDone(void *AppUpd, long ErrorCode, char * ErrorText);
	void  wodAppUpd_PrevDetected(void *AppUpd);
#endif