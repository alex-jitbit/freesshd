
FreeSSHDServiceps.dll: dlldata.obj FreeSSHDService_p.obj FreeSSHDService_i.obj
	link /dll /out:FreeSSHDServiceps.dll /def:FreeSSHDServiceps.def /entry:DllMain dlldata.obj FreeSSHDService_p.obj FreeSSHDService_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del FreeSSHDServiceps.dll
	@del FreeSSHDServiceps.lib
	@del FreeSSHDServiceps.exp
	@del dlldata.obj
	@del FreeSSHDService_p.obj
	@del FreeSSHDService_i.obj
