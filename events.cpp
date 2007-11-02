#include "stdafx.h"
#include "events.h"

#ifdef _ISFREEOPEN

_ATL_FUNC_INFO INFO_Connecting				= {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_LoginPassword			=  {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR, VT_BSTR, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_Disconnected			= {CC_STDCALL, VT_EMPTY, 1, {VT_DISPATCH}};
_ATL_FUNC_INFO INFO_StateChanged			= {CC_STDCALL, VT_EMPTY, 3, {VT_DISPATCH, VT_I4, VT_I4}};
_ATL_FUNC_INFO INFO_Received				= {CC_STDCALL, VT_EMPTY, 3, {VT_DISPATCH, VT_I4, VT_I4}};
_ATL_FUNC_INFO INFO_ServiceRequest			= {CC_STDCALL, VT_EMPTY, 5, {VT_DISPATCH, VT_I4, VT_I4 | VT_BYREF, VT_BSTR | VT_BYREF, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_ServiceStart			= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_I4, VT_I4, VT_BSTR}};
_ATL_FUNC_INFO INFO_SftpEvent				= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR, VT_BSTR | VT_BYREF, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_SftpRename				= {CC_STDCALL, VT_EMPTY, 5, {VT_DISPATCH, VT_BSTR, VT_BSTR | VT_BYREF, VT_BSTR | VT_BYREF, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_PortBindRequest			= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR | VT_BYREF, VT_I4 | VT_BYREF, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_PortForwardRequest		= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR | VT_BYREF, VT_I4 | VT_BYREF, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_PortForwardConnect		= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR, VT_I4, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_PortForwardDisconnect	= {CC_STDCALL, VT_EMPTY, 3, {VT_DISPATCH, VT_BSTR, VT_I4}};
_ATL_FUNC_INFO INFO_LoginPubkey				= {CC_STDCALL, VT_EMPTY, 4, {VT_DISPATCH, VT_BSTR, VT_BSTR, VT_I4 | VT_BYREF}};
_ATL_FUNC_INFO INFO_CryptoInformation		= {CC_STDCALL, VT_EMPTY, 7, {VT_DISPATCH, VT_BSTR, VT_BSTR, VT_BSTR, VT_BSTR, VT_BSTR, VT_BOOL}}; 
 
/*
void (*CryptoInformation)(CwodSSHDComPtr Owner, CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept);*/


SSHEvents::SSHEvents (CwodSSHDComPtr SSH, CServiceModule *Service)
{
	m_SSHPtr = SSH;
	m_SSHPtr.AddRef();
	m_Service = Service;
	DispEventAdvise ( (IUnknown*)m_SSHPtr);
}

SSHEvents::~SSHEvents ()
{
	DispEventUnadvise ( (IUnknown*)m_SSHPtr);
	m_SSHPtr.Release();
}

void SSHEvents::Connecting(CSSHUser *User, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_Connecting(m_SSH, User, Action);
}

void SSHEvents::LoginPassword(CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_LoginPassword(m_SSH, User, Login, Password, Action);
}
		
void SSHEvents::Disconnected(CSSHUser *User)
{
	if (m_Service)
		m_Service->SSH_Disconnected(m_SSH, User);
}

void SSHEvents::StateChanged(CSSHUser *User, SSHStates NewState, SSHStates OldState)
{
	if (m_Service)
		m_Service->SSH_StateChanged(m_SSH, User, NewState, OldState);
}

void SSHEvents::Received(CSSHUser *User, int ServiceIndex, int BytesCount)
{
	if (m_Service)
		m_Service->SSH_Received(m_SSH, User, ServiceIndex, BytesCount);
}

void SSHEvents::ServiceRequest(CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_ServiceRequest(m_SSH, User, ServiceIndex, ServiceType, ServicePath, Action);
}

void SSHEvents::ServiceStart(CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName)
{
	if (m_Service)
		m_Service->SSH_ServiceStart(m_SSH, User, ServiceIndex, ServiceType, ServiceName);
}

void SSHEvents::SftpListDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpListDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpDownloadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpDownloadFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpUploadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpUploadFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpRemoveDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpRemoveDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpDeleteFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpDeleteFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpMakeDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpMakeDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void SSHEvents::SftpRename(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_SftpRename(m_SSH, User, RelativePath, ResolvedPath, NewName, Action);
}

void SSHEvents::PortBindRequest(CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_PortBindRequest(m_SSH, User, BindIP, BindPort, Action);
}

void SSHEvents::PortForwardRequest(CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_PortForwardRequest(m_SSH, User, RemoteHost, RemotePort, Action);
}

void SSHEvents::PortForwardConnect(CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_PortForwardConnect(m_SSH, User, RemoteHost, RemotePort, Action);
}

void SSHEvents::PortForwardDisconnect(CSSHUser *User, BSTR RemoteHost, long RemotePort)
{
	if (m_Service)
		m_Service->SSH_PortForwardDisconnect(m_SSH, User, RemoteHost, RemotePort);
}

void SSHEvents::LoginPubkey(CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action)
{
	if (m_Service)
		m_Service->SSH_LoginPubkey(m_SSH, User, Login, PublicKey, Action);
}

void SSHEvents::CryptoInformation(CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept)
{
	if (m_Service)
		m_Service->SSH_CryptoInformation(m_SSH, User, Protocol, RemoteName, SCcipher, CScipher, Keys, Accept);
}


TelnetEvents::TelnetEvents (CwodSSHDComPtr SSH, CServiceModule *Service)
{
	m_SSH = SSH;
	m_SSH.AddRef();
	m_Service = Service;
	DispEventAdvise ( (IUnknown*)m_SSH);
}

TelnetEvents::~TelnetEvents ()
{
	DispEventUnadvise ( (IUnknown*)m_SSH);
	m_SSH.Release();
}

void TelnetEvents::Connecting(CSSHUser *User, SSHActions *Action)
{
	if (m_Service)
		m_Service->Telnet_Connecting(m_SSH, User, Action);
}

void TelnetEvents::LoginPassword(CSSHUser *User, BSTR Login, BSTR Password, SSHActions *Action)
{
	if (m_Service)
		m_Service->Telnet_LoginPassword(m_SSH, User, Login, Password, Action);
}
		
void TelnetEvents::Disconnected(CSSHUser *User)
{
	if (m_Service)
		m_Service->Telnet_Disconnected(m_SSH, User);
}

void TelnetEvents::StateChanged(CSSHUser *User, SSHStates NewState, SSHStates OldState)
{
	if (m_Service)
		m_Service->Telnet_StateChanged(m_SSH, User, NewState, OldState);
}

void TelnetEvents::Received(CSSHUser *User, int ServiceIndex, int BytesCount)
{
	if (m_Service)
		m_Service->Telnet_Received(m_SSH, User, ServiceIndex, BytesCount);
}

void TelnetEvents::ServiceRequest(CSSHUser *User, int ServiceIndex, SSHServiceTypes *ServiceType, BSTR *ServicePath, SSHActions *Action)
{
	if (m_Service)
		m_Service->Telnet_ServiceRequest(m_SSH, User, ServiceIndex, ServiceType, ServicePath, Action);
}

void TelnetEvents::ServiceStart(CSSHUser *User, int ServiceIndex, SSHServiceTypes ServiceType, BSTR ServiceName)
{
	if (m_Service)
		m_Service->Telnet_ServiceStart(m_SSH, User, ServiceIndex, ServiceType, ServiceName);
}

void TelnetEvents::SftpListDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpListDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpDownloadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpDownloadFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpUploadFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpUploadFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpRemoveDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpRemoveDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpDeleteFile(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpDeleteFile(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpMakeDir(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpMakeDir(m_SSH, User, RelativePath, ResolvedPath, Action);
}

void TelnetEvents::SftpRename(CSSHUser *User, BSTR RelativePath, BSTR *ResolvedPath, BSTR *NewName, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_SftpRename(m_SSH, User, RelativePath, ResolvedPath, NewName, Action);
}

void TelnetEvents::PortBindRequest(CSSHUser *User, BSTR *BindIP, long *BindPort, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_PortBindRequest(m_SSH, User, BindIP, BindPort, Action);
}

void TelnetEvents::PortForwardRequest(CSSHUser *User, BSTR *RemoteHost, long *RemotePort, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_PortForwardRequest(m_SSH, User, RemoteHost, RemotePort, Action);
}

void TelnetEvents::PortForwardConnect(CSSHUser *User, BSTR RemoteHost, long RemotePort, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_PortForwardConnect(m_SSH, User, RemoteHost, RemotePort, Action);
}

void TelnetEvents::PortForwardDisconnect(CSSHUser *User, BSTR RemoteHost, long RemotePort)
{
	//if (m_Service)
	//	m_Service->Telnet_PortForwardDisconnect(m_SSH, User, RemoteHost, RemotePort);
}

void TelnetEvents::LoginPubkey(CSSHUser *User, BSTR Login, BSTR PublicKey, SSHActions *Action)
{
	//if (m_Service)
	//	m_Service->Telnet_LoginPubkey(m_SSH, User, Login, PublicKey, Action);
}

void TelnetEvents::CryptoInformation(CSSHUser *User, BSTR Protocol, BSTR RemoteName, BSTR SCcipher, BSTR CScipher, BSTR Keys, VARIANT_BOOL *Accept)
{
	if (m_Service)
		m_Service->Telnet_CryptoInformation(m_SSH, User, Protocol, RemoteName, SCcipher, CScipher, Keys, Accept);
}
#endif