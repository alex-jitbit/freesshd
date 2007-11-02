# Microsoft Developer Studio Project File - Name="FreeSSHDService_Open" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FreeSSHDService_Open - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FreeSSHDService_Open.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FreeSSHDService_Open.mak" CFG="FreeSSHDService_Open - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FreeSSHDService_Open - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FreeSSHDService_Open - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeSSHDService_Open - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Objs"
# PROP Intermediate_Dir "Objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_ISFREEOPEN" /D "_ATL_CRT_MIN" /FR /YX"stdafx.h" /FD /GZ /Zm"1000" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41a /d "_DEBUG"
# ADD RSC /l 0x41a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib Netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib userenv.lib /nologo /subsystem:windows /debug /machine:I386 /out:"OpenDebug\FreeSSHDService.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\Objs
TargetPath=.\OpenDebug\FreeSSHDService.exe
InputPath=.\OpenDebug\FreeSSHDService.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeSSHDService_Open - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Objs"
# PROP Intermediate_Dir "Objs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_ISFREEOPEN" /D "_ATL_NO_MSIMG" /D "_ATL_CRT_MIN" /D "_ZLIB" /Fr /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x41a /d "NDEBUG"
# ADD RSC /l 0x41a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib Netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib userenv.lib /nologo /subsystem:windows /machine:I386 /out:"OpenRelease/FreeSSHDService.exe"
# SUBTRACT LINK32 /debug /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\Objs
TargetPath=.\OpenRelease\FreeSSHDService.exe
InputPath=.\OpenRelease\FreeSSHDService.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	echo Server registration done! 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "FreeSSHDService_Open - Win32 Debug"
# Name "FreeSSHDService_Open - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\ComBSTR2.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\freeSSHdHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeSSHDService.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeSSHDService.idl
# ADD MTL /tlb ".\FreeSSHDService.tlb" /h "FreeSSHDService.h" /iid "FreeSSHDService_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\FreeSSHDService.rc
# End Source File
# Begin Source File

SOURCE=.\freeSSHdUser.cpp
# End Source File
# Begin Source File

SOURCE=.\gdiu.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\maindlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\pages.cpp
# End Source File
# Begin Source File

SOURCE=.\sha1.cpp
# End Source File
# Begin Source File

SOURCE=.\user.cpp
# End Source File
# Begin Source File

SOURCE=.\userpages.cpp
# End Source File
# Begin Source File

SOURCE=.\wildmat.cpp
# End Source File
# Begin Source File

SOURCE=.\wodAppUpdateEvents.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Buffer.h
# End Source File
# Begin Source File

SOURCE=.\ComBSTR2.h
# End Source File
# Begin Source File

SOURCE=.\events.h
# End Source File
# Begin Source File

SOURCE=.\freeSSHdHandler.h
# End Source File
# Begin Source File

SOURCE=.\FreeSSHDService.h
# End Source File
# Begin Source File

SOURCE=.\freeSSHdUser.h
# End Source File
# Begin Source File

SOURCE=.\gdiu.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\maindlg.h
# End Source File
# Begin Source File

SOURCE=.\MessageDlg.h
# End Source File
# Begin Source File

SOURCE=.\pages.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\user.h
# End Source File
# Begin Source File

SOURCE=.\userpages.h
# End Source File
# Begin Source File

SOURCE=.\wodAppUpdateEvents.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\images\authentication.bmp
# End Source File
# Begin Source File

SOURCE=.\images\bad.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\bad.ico
# End Source File
# Begin Source File

SOURCE=.\images\ciphers.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\ciphers.ico
# End Source File
# Begin Source File

SOURCE=.\images\compression.bmp
# End Source File
# Begin Source File

SOURCE=.\FreeSSHDService.rgs
# End Source File
# Begin Source File

SOURCE=.\images\host_restrictions.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\host_restrictions.ico
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Images\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Images\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\images\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Images\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\images\key_text.bmp
# End Source File
# Begin Source File

SOURCE=.\images\log.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\log.ico
# End Source File
# Begin Source File

SOURCE=.\images\madeby.bmp
# End Source File
# Begin Source File

SOURCE=.\images\no.ico
# End Source File
# Begin Source File

SOURCE=.\images\ok.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\ok.ico
# End Source File
# Begin Source File

SOURCE=.\images\settings_ssh.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\settings_ssh.ico
# End Source File
# Begin Source File

SOURCE=.\images\settings_telnet.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\settings_telnet.ico
# End Source File
# Begin Source File

SOURCE=.\images\sftp.bmp
# End Source File
# Begin Source File

SOURCE=.\images\status.bmp
# End Source File
# Begin Source File

SOURCE=.\images\status2.bmp
# End Source File
# Begin Source File

SOURCE=.\images\tunnel.bmp
# End Source File
# Begin Source File

SOURCE=.\images\update.ico
# End Source File
# Begin Source File

SOURCE=.\images\user.ico
# End Source File
# Begin Source File

SOURCE=.\images\user_restrictions.bmp
# End Source File
# Begin Source File

SOURCE=.\Images\user_restrictions.ico
# End Source File
# Begin Source File

SOURCE=.\images\weonlydo.bmp
# End Source File
# Begin Source File

SOURCE=.\images\weonlydosm.bmp
# End Source File
# Begin Source File

SOURCE=.\images\yes.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\freeSSHdservice.exe.manifest
# End Source File
# End Target
# End Project
