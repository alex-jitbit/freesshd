// wodAppUpdateEvents.cpp: implementation of the wodAppUpdateEvents class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "freesshdservice.h"
#include "log.h"
#include "ComBSTR2.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define HANDLER _Module.freeSSHdHandler

//////////////////////////////////////////////////////////////////////
// ATL function info
//////////////////////////////////////////////////////////////////////

#ifdef _ISFREEOPEN
_ATL_FUNC_INFO INFO_CloseApp = {CC_STDCALL, VT_EMPTY, 0, 0};
_ATL_FUNC_INFO INFO_CheckDone = {CC_STDCALL, VT_EMPTY, 3, {VT_I4, VT_I4, VT_BSTR}};
_ATL_FUNC_INFO INFO_StateChange = {CC_STDCALL, VT_EMPTY, 1, {VT_I4}};
_ATL_FUNC_INFO INFO_UpdateDone = {CC_STDCALL, VT_EMPTY, 2, {VT_I4, VT_BSTR}};
_ATL_FUNC_INFO INFO_FileStart = {CC_STDCALL, VT_EMPTY, 1, {VT_DISPATCH}};
_ATL_FUNC_INFO INFO_FileProgress = {CC_STDCALL, VT_EMPTY, 3, {VT_DISPATCH, VT_I4, VT_I4}};
_ATL_FUNC_INFO INFO_FileDone = {CC_STDCALL, VT_EMPTY, 3, {VT_DISPATCH, VT_I4, VT_BSTR}};
_ATL_FUNC_INFO INFO_DownloadDone = {CC_STDCALL, VT_EMPTY, 2, {VT_I4, VT_BSTR}};
_ATL_FUNC_INFO INFO_PrevDetected = {CC_STDCALL, VT_EMPTY, 0, 0};
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _ISFREEOPEN
wodAppUpdateEvents::wodAppUpdateEvents (WODAPPUPDATECOMLib::IwodAppUpdateComPtr pWodAppUpdateCom)
{
    m_pWodAppUpdateCom = pWodAppUpdateCom;
    DispEventAdvise ( (IUnknown*)m_pWodAppUpdateCom);
}

wodAppUpdateEvents::~wodAppUpdateEvents ()
{
	DispEventUnadvise ( (IUnknown*)m_pWodAppUpdateCom);
	m_pWodAppUpdateCom.Release();
}

//////////////////////////////////////////////////////////////////////
// Events
//////////////////////////////////////////////////////////////////////


void _stdcall wodAppUpdateEvents::CloseApp()
{

}
#endif

#ifdef _ISFREEOPEN
void _stdcall wodAppUpdateEvents::CheckDone(long NewFiles, long ErrorCode, BSTR ErrorText)
#else
void  wodAppUpd_CheckDone(void *AppUpd, long NewFiles, long ErrorCode, char * ErrorText)
#endif
{
	if (ErrorCode == 0)
	{
		short iMessageCount = 0;	
#ifdef _ISFREEOPEN	
		WODAPPUPDATECOMLib::IUpdMessagesPtr Messages;
	
		HANDLER.m_Update->get_Messages(&Messages);

		Messages->get_Count(&iMessageCount);

#else
		WODAPPUPDCOMLib::AppUpd_Messages_GetCount(HANDLER.m_Update, &iMessageCount);
#endif
		for (int i = 0; i < iMessageCount; i++)
		{
			CComBSTR2 bstrText, bstrCaption;
			long MessageID = 0;

#ifdef _ISFREEOPEN	
			WODAPPUPDATECOMLib::IUpdMessagePtr Message;

			Messages->get_Item(i, &Message);
			
			Message->get_Text(&bstrText);
			Message->get_Caption(&bstrCaption);
			Message->get_ID(&MessageID);
#else
			void *message;
			char buff[16384];
			int len = 16384;

			WODAPPUPDCOMLib::AppUpd_Messages_GetMessage(HANDLER.m_Update, i, &message);
			WODAPPUPDCOMLib::AppUpd_Message_GetID(message, &MessageID);
			WODAPPUPDCOMLib::AppUpd_Message_GetCaption(message, buff, &len);
			bstrCaption = buff;
			len = 16384;
			WODAPPUPDCOMLib::AppUpd_Message_GetText(message, buff, &len);
			bstrText = buff;
#endif
			if (HANDLER.Config.UpdateLastMessageID < MessageID)
			{			
				if (HANDLER.Config.UpdateDontPrompt)
				{
					char buff[10240] = "freeSSHd.com message: title=\"";
					strncat(buff, bstrCaption.ToString(), 5000);
					strncat(buff, "\" text=\"", 8);
					strncat(buff, bstrText.ToString(), 5000);
					strncat(buff, "\"", 3);

					WriteLog(buff);
				}
				else
				{	
					CMessageDlg	dlg;

					dlg.SetText(bstrText.ToString());
					dlg.SetCaption(bstrCaption.ToString());
					
					dlg.DoModal();
				}	

				HANDLER.Config.UpdateLastMessageID = MessageID;
			}	
			
		}
		
		if (iMessageCount)
			HANDLER.SaveMessageID();

		if (NewFiles)
		{
			if (HANDLER.Config.UpdateDontPrompt)
			{
				if (HANDLER.SSH_UserCount() || HANDLER.Telnet_UserCount())
					if (MessageBox(NULL, "There are users connected. Do you want to proceed with updating?", "freeSSHd update", MB_YESNO) == IDNO)
						return;
#ifdef _ISFREEOPEN
				_Module.freeSSHdHandler.m_Update->Download();
#else
				WODAPPUPDCOMLib::AppUpd_Download(HANDLER.m_Update);
#endif
			}
			else
				if (MessageBox(NULL, "New version of freeSSHd is available. Download?", "freeSSHd update", MB_YESNO) == IDYES)
#ifdef _ISFREEOPEN
					_Module.freeSSHdHandler.m_Update->Download();	
#else
					WODAPPUPDCOMLib::AppUpd_Download(HANDLER.m_Update);
#endif
		}
		else
		{
			if (HANDLER.m_StartupUpdate)
				WriteLog("Your freeSSHd is up-to-date.");
			else
				MessageBox(NULL, "Your freeSSHd is up-to-date.", "freeSSHd update", MB_OK);
		}
	}
	else
		MessageBox(NULL, "Error connecting to freeSSHd.com.", "freeSSHd update", MB_ICONERROR);
}

#ifdef _ISFREEOPEN
void _stdcall wodAppUpdateEvents::StateChange(WODAPPUPDATECOMLib::UpdateStates OldState)
{

}

void _stdcall wodAppUpdateEvents::UpdateDone(long ErrorCode, BSTR ErrorText)
{
}

void _stdcall wodAppUpdateEvents::FileStart(WODAPPUPDATECOMLib::IUpdFile* File)
{

}

void _stdcall wodAppUpdateEvents::FileProgress(WODAPPUPDATECOMLib::IUpdFile* File, long Position, long Total)
{

}

void _stdcall wodAppUpdateEvents::FileDone(WODAPPUPDATECOMLib::IUpdFile* File, long ErrorCode, BSTR ErrorText)
{

}
#endif

#ifdef _ISFREEOPEN
void _stdcall wodAppUpdateEvents::DownloadDone(long ErrorCode, BSTR ErrorText)
#else
void  wodAppUpd_DownloadDone(void *AppUpd, long ErrorCode, char * ErrorText)
#endif
{
	if (ErrorCode == 0)
	{
		if (HANDLER.Config.UpdateDontPrompt)
#ifdef _ISFREEOPEN
			_Module.freeSSHdHandler.m_Update->Update();	
#else
			WODAPPUPDCOMLib::AppUpd_Update(HANDLER.m_Update);
#endif
		else
			if (MessageBox(NULL, "Download successful. Replace now?", "freeSSHd update", MB_YESNO) == IDYES)
#ifdef _ISFREEOPEN
				_Module.freeSSHdHandler.m_Update->Update();	
#else
				WODAPPUPDCOMLib::AppUpd_Update(HANDLER.m_Update);
#endif	
	}
	else
	{ 
		if (!HANDLER.Config.UpdateDontPrompt)
			MessageBox(NULL, "There was an error downloading files.", "freeSSHd update", MB_ICONERROR);
	}
}

#ifdef _ISFREEOPEN
void _stdcall wodAppUpdateEvents::PrevDetected()
#else
void  wodAppUpd_PrevDetected(void *AppUpd)
#endif
{
	if (HANDLER.Config.UpdateDontPrompt)
#ifdef _ISFREEOPEN
			_Module.freeSSHdHandler.m_Update->Update();	
#else
			WODAPPUPDCOMLib::AppUpd_Update(HANDLER.m_Update);
#endif	
		else
		{
			if (MessageBox(NULL, "Previous download detected. Update now?", "freeSSHd update", MB_YESNO) == IDYES)
#ifdef _ISFREEOPEN
				_Module.freeSSHdHandler.m_Update->Update();	
#else
				WODAPPUPDCOMLib::AppUpd_Update(HANDLER.m_Update);
#endif
			else
#ifdef _ISFREEOPEN
				_Module.freeSSHdHandler.m_Update->Clear();	
#else
				WODAPPUPDCOMLib::AppUpd_Clear(HANDLER.m_Update);
#endif
		}	

}
