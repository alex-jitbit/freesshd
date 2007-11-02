#ifndef _EVENTS__H_
#define _EVENTS__H_

extern _ATL_FUNC_INFO INFO_Connecting;
extern _ATL_FUNC_INFO INFO_LoginPassword;
extern _ATL_FUNC_INFO INFO_Disconnected;
extern _ATL_FUNC_INFO INFO_StateChanged;
extern _ATL_FUNC_INFO INFO_Received;
extern _ATL_FUNC_INFO INFO_ServiceRequest;
extern _ATL_FUNC_INFO INFO_ServiceStart;
extern _ATL_FUNC_INFO INFO_SftpEvent;
extern _ATL_FUNC_INFO INFO_SftpRename;
extern _ATL_FUNC_INFO INFO_PortBindRequest;
extern _ATL_FUNC_INFO INFO_PortForwardRequest;
extern _ATL_FUNC_INFO INFO_PortForwardConnect;
extern _ATL_FUNC_INFO INFO_PortForwardDisconnect;
extern _ATL_FUNC_INFO INFO_LoginPubkey;
extern _ATL_FUNC_INFO INFO_CryptoInformation;


/*		void (*Connecting)(CwodSSHDComPtr Owner, CSSHUser *User, SSHActions *Action);
		void (*LoginPassword)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
		void (*Disconnected)(CwodSSHDComPtr Owner, CSSHUser *User);
		void (*StateChanged)(CwodSSHDComPtr Owner, CSSHUser *User, SSHStates NewState, SSHStates OldState);
		void (*Received)(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, int BytesCount);
		void (*ServiceRequest)(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
		void (*ServiceStart)(CwodSSHDComPtr Owner, CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
		void (*SftpListDir)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpDownloadFile)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpUploadFile)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpRemoveDir)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpDeleteFile)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpMakeDir)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void (*SftpRename)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
		void (*PortBindRequest)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
		void (*PortForwardRequest)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
		void (*PortForwardConnect)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
		void (*PortForwardDisconnect)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR RemoteHost, long RemotePort);
		void (*LoginPubkey)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
		void (*CryptoInformation)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);*/

class SSHEvents : public IDispEventSimpleImpl<1, SSHEvents, &DIID__IwodSSHDComEvents>
{
	public:
		SSHEvents (CwodSSHDComPtr SSH, CServiceModule *Service);
		virtual ~SSHEvents ();

		void __stdcall Connecting(CSSHUser *User, SSHActions *Action);
		void __stdcall LoginPassword(CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
		void __stdcall Disconnected(CSSHUser *User);
		void __stdcall StateChanged(CSSHUser *User, SSHStates NewState, SSHStates OldState);
		void __stdcall Received(CSSHUser *User, int ServiceIndex, int BytesCount);
		void __stdcall ServiceRequest(CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
		void __stdcall ServiceStart(CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
		void __stdcall SftpListDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpDownloadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpUploadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpRemoveDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpDeleteFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpMakeDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpRename(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
		void __stdcall PortBindRequest(CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
		void __stdcall PortForwardRequest(CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
		void __stdcall PortForwardConnect(CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
		void __stdcall PortForwardDisconnect(CSSHUser *User, BSTR RemoteHost, long RemotePort);
		void __stdcall LoginPubkey(CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
		void __stdcall CryptoInformation(CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);
		
		BEGIN_SINK_MAP (SSHEvents)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 1, Connecting, &INFO_Connecting)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 2, LoginPassword, &INFO_LoginPassword)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 3, Disconnected, &INFO_Disconnected)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 4, StateChanged, &INFO_StateChanged)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 5, Received, &INFO_Received)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 6, ServiceRequest, &INFO_ServiceRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 7, ServiceStart, &INFO_ServiceStart)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 8, SftpListDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 9, SftpDownloadFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 10, SftpUploadFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 11, SftpRemoveDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 12, SftpDeleteFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 13, SftpMakeDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 14, SftpRename, &INFO_SftpRename)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 15, PortBindRequest, &INFO_PortBindRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 16, PortForwardRequest, &INFO_PortForwardRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 17, PortForwardConnect, &INFO_PortForwardConnect)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 18, LoginPubkey, &INFO_LoginPubkey)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 19, CryptoInformation, &INFO_CryptoInformation)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 20, PortForwardDisconnect, &INFO_PortForwardDisconnect)
		END_SINK_MAP ()

	private:
		CServiceModule *m_Service;
		CwodSSHDComPtr m_SSHPtr;
};

class TelnetEvents : public IDispEventSimpleImpl<1, TelnetEvents, &DIID__IwodSSHDComEvents>
{
	public:
		TelnetEvents (CwodSSHDComPtr SSH, CServiceModule *Service);
		virtual ~TelnetEvents ();

		void __stdcall Connecting(CSSHUser *User, SSHActions *Action);
		void __stdcall LoginPassword(CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action);
		void __stdcall Disconnected(CSSHUser *User);
		void __stdcall StateChanged(CSSHUser *User, SSHStates NewState, SSHStates OldState);
		void __stdcall Received(CSSHUser *User, int ServiceIndex, int BytesCount);
		void __stdcall ServiceRequest(CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action);
		void __stdcall ServiceStart(CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName);
		void __stdcall SftpListDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpDownloadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpUploadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpRemoveDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpDeleteFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpMakeDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action);
		void __stdcall SftpRename(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action);
		void __stdcall PortBindRequest(CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action);
		void __stdcall PortForwardRequest(CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action);
		void __stdcall PortForwardConnect(CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action);
		void __stdcall PortForwardDisconnect(CSSHUser *User, BSTR RemoteHost, long RemotePort);
		void __stdcall LoginPubkey(CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action);
		void __stdcall CryptoInformation(CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);
		
		BEGIN_SINK_MAP (TelnetEvents)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 1, Connecting, &INFO_Connecting)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 2, LoginPassword, &INFO_LoginPassword)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 3, Disconnected, &INFO_Disconnected)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 4, StateChanged, &INFO_StateChanged)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 5, Received, &INFO_Received)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 6, ServiceRequest, &INFO_ServiceRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 7, ServiceStart, &INFO_ServiceStart)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 8, SftpListDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 9, SftpDownloadFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 10, SftpUploadFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 11, SftpRemoveDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 12, SftpDeleteFile, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 13, SftpMakeDir, &INFO_SftpEvent)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 14, SftpRename, &INFO_SftpRename)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 15, PortBindRequest, &INFO_PortBindRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 16, PortForwardRequest, &INFO_PortForwardRequest)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 17, PortForwardConnect, &INFO_PortForwardConnect)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 18, LoginPubkey, &INFO_LoginPubkey)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 19, CryptoInformation, &INFO_CryptoInformation)
			SINK_ENTRY_INFO (1, DIID__IwodSSHDComEvents, 20, PortForwardDisconnect, &INFO_PortForwardDisconnect)
		END_SINK_MAP ()

	private:
		CServiceModule *m_Service;
		CwodSSHDComPtr m_SSH;
};

#endif