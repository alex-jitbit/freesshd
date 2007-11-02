#include "stdafx.h"
#include "log.h"
#include "ComBSTR2.h"
#include <stdio.h>
#include <time.h>
//
#ifdef _ISFREEEXE
#include "../../component/wodSSHDCom.h"
#include "../../component/SSHKeyPair.h"
#include "../../component/SSHUsers.h"
#endif
//

void GetHostname(char *ip, char *hostname);

void GetTimestamp(char *time)
{
	SYSTEMTIME st;
	
	GetLocalTime(&st);
	
	sprintf(time, "%02d-%02d-%d %02d:%02d:%02d",st.wMonth, st.wDay, st.wYear,st.wHour,st.wMinute,st.wSecond);
	
}
void WriteLog(char *text)
{
	char buff[14000];
	memset(buff, 0, 14000*sizeof(char));
		
	if (_Module.freeSSHdHandler.Config.LogEvents == TRUE)
	{
		FILE *f = fopen(_Module.freeSSHdHandler.Config.LogFilePath,"at");
		if (f == NULL)
			_Module.LogEvent("Could not write to log file!");
		else
		{
			GetTimestamp(buff);
			strncat(buff," ", 2);
			strncat(buff,text, 13000);
			fprintf(f,"%s\n",buff);
		
			fclose(f);
		}
	}
}

void WriteConnectionLog(CSSHUser *User, int service, enum conn_state state, int auth_method)
{
	char buff[10240];
	memset(buff, 0, 10240*sizeof(char));

	in_addr ia;
	HOSTENT *he;
	int bResolved = FALSE;

	CComBSTR2 cbLogin, cbHostname;
	User->get_Login(&cbLogin);
	User->get_RemoteIP(&cbHostname);

	if (_Module.freeSSHdHandler.Config.LogEvents == TRUE)
	{
		FILE *f = fopen(_Module.freeSSHdHandler.Config.LogFilePath,"at");
		if (f == NULL)
			_Module.LogEvent("Could not write to log file!");
		else
		{
			GetTimestamp(buff);
			
			if (_Module.freeSSHdHandler.Config.LogResolveIP == TRUE)
			{
				ia.s_addr = inet_addr(cbHostname.ToString());
			
				if (ia.s_addr != INADDR_NONE) 
				{
					he = gethostbyaddr((char *)&ia,sizeof(struct in_addr),AF_INET);
					if (he != NULL) 
					{
						strncat(buff," HOST ", 6);
						strncat(buff, he->h_name, 256);
						bResolved = TRUE;
					}
				}
			}

			if (bResolved == FALSE) 
			{
				strncat(buff," IP ", 4);
				strncat(buff,cbHostname.ToString(), 256);
			}
			
			if (service == 0) strncat(buff," SSH ", 10);
			if (service == 1) strncat(buff," TELNET ", 10);

			switch (state)
			{
				case success:
					strncat(buff,cbLogin.ToString(), 257);

					if (auth_method == 0) 
						strncat(buff," successfully logged on using password.", 50);

					if (auth_method == 1)	
						strncat(buff," successfully logged on using public key.", 50);

					if (auth_method == 2)	
						strncat(buff," successfully logged on using password and public key.", 70);
					break;
				case connecting:
					//strcat(buff,cbLogin.ToString());
					strncat(buff,"connection attempt.", 40);
					break;
				case bad_pass:
					strncat(buff,cbLogin.ToString(), 257);
					strncat(buff," submitted a bad password.", 40);
					break;
				case bad_key:
					strncat(buff,cbLogin.ToString(), 257);
					strncat(buff," submitted a bad key.", 40);
					break;
				case host_restriction:
					strncat(buff,cbLogin.ToString(), 257);
					strncat(buff," can't connect. IP isn't allowed due to host restrictions.", 100);
					break;
				case disconnect:
					strncat(buff,cbLogin.ToString(), 257);
					strncat(buff," disconnected.",  40);
					break;
				case user_unknown:
					strncat(buff,cbLogin.ToString(), 257);
					strncat(buff, ": user unknown.", 40);
					break;
			}
			
			fprintf(f,"%s\n",buff);
			
			fclose(f);
		}
	}
}

void WriteServiceLog(CSSHUser *User, enum service_type service, int state)
{
	char buff[10240];
	memset(buff, 0, 10240*sizeof(char));

	CComBSTR2 cbLogin;
	User->get_Login(&cbLogin);
	
	if (_Module.freeSSHdHandler.Config.LogEvents == TRUE)
	{
		FILE *f = fopen(_Module.freeSSHdHandler.Config.LogFilePath,"at");
		if (f == NULL)
			_Module.LogEvent("Could not write to log file!");
		else
		{
			GetTimestamp(buff);
			strncat(buff," ",2);
			switch (service)
			{
				case shell:
					strncat(buff,"Shell", 6);
					break;
				case sftp:
					strncat(buff,"SFTP", 5);
					break;
				case tunnel:
					strncat(buff,"Tunneling", 10);
				break;
			}
			
			switch (state)
			{
				case 0:
					//strcat(buff,User);
					strncat(buff," service granted to user ", 40);
					break;
				case 1:
					//strcat(buff,User);
					strncat(buff," service denied to user ", 40);
					break;
				
			}
			
			strncat(buff,cbLogin.ToString(), 257);

			fprintf(f,"%s.\n",buff);
			
			fclose(f);
		}
	}
}

void WriteUserLog(CSSHUser *User, char *text)
{
	char buff[14000];
	memset(buff, 0, 14000*sizeof(char));

	in_addr ia;
	HOSTENT *he;
	int bResolved = FALSE;

	CComBSTR2 cbLogin, cbHostname;
	User->get_Login(&cbLogin);
	User->get_RemoteIP(&cbHostname);
	
	if (_Module.freeSSHdHandler.Config.LogEvents == TRUE)
	{
		FILE *f = fopen(_Module.freeSSHdHandler.Config.LogFilePath,"at");

		if (f == NULL)
			_Module.LogEvent("Could not write to log file!");
		else
		{
			GetTimestamp(buff);
			
			if (_Module.freeSSHdHandler.Config.LogResolveIP == TRUE)
			{
				ia.s_addr = inet_addr(cbHostname.ToString());
			
				if (ia.s_addr != INADDR_NONE) 
				{
					he = gethostbyaddr((char *)&ia,sizeof(struct in_addr),AF_INET);
					
					if (he != NULL) 
					{
						strncat(buff," HOST ", 256);
						strncat(buff, he->h_name, 256);
						bResolved = TRUE;

					}
				}
			}

			if (bResolved == FALSE) 
			{
				strncat(buff," IP ", 256);
				strncat(buff,cbHostname.ToString(), 256);
			}
			
			strncat(buff," ", 2);
			strncat(buff,cbLogin.ToString(), 257);
			strncat(buff," ", 2);
			strncat(buff,text, 1024);
			
			fprintf(f,"%s\n",buff);
		
			fclose(f);
		}
	}
}

void UpdateNotifyIcon()
{
	char text[64];
	
	if (_Module.m_hWnd)
	{
	
	
		if (!_Module.freeSSHdHandler.SSH_IsActive())
			strcpy(text,"SSH not active.");
		else
		{
			strcpy(text,"SSH active on ");
			strcat(text,_Module.freeSSHdHandler.Config.SSHListenAddress);
		}

		if (!_Module.freeSSHdHandler.Telnet_IsActive())
			strcat(text,"\r\nTelnet not active.");
		else 
		{
			strcat(text,"\r\nTelnet active on ");
			strcat(text, _Module.freeSSHdHandler.Config.TelnetListenAddress);
		}
	
		strcpy(_Module.m_NotifyIcon.szTip,text);
		Shell_NotifyIcon(NIM_MODIFY,&(_Module.m_NotifyIcon));
	}
}


void DateToString(DATE newVal,char *string)
{
	SYSTEMTIME st;
	char buff[256];
	
	char *days[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	char *months[13] = {"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	
	TIME_ZONE_INFORMATION tz;
	memset(&tz, 0, sizeof(TIME_ZONE_INFORMATION));
	GetTimeZoneInformation(&tz); 
	
	
	VariantTimeToSystemTime(newVal, &st);
	
	char buff1[128];
	//strcpy(buff1, "%s, %d %s %d 00:00:00 -0000");
	strcpy(buff1, "%s, %d %s %d 00:00:00");
	
	char *hourpos = buff1+13;
	if (st.wHour<10)
		hourpos++;
	wsprintf(hourpos, "%d", st.wHour);
	buff1[15] = ':';
	
	char *minpos = buff1+16;
	if (st.wMinute<10)
		minpos++;
	wsprintf(minpos, "%d", st.wMinute);
	buff1[18] = ':';
	
	char *secpos = buff1+19;
	if (st.wSecond<10)
		secpos++;
	wsprintf(secpos, "%d", st.wSecond);
	/*buff1[21] = ' ';
	
	  SYSTEMTIME st1, st2;
	  __int64 t1, t2;
	  GetSystemTime(&st1);
	  GetLocalTime(&st2);
	  SystemTimeToFileTime(&st1, (FILETIME *)&t1);
	  SystemTimeToFileTime(&st2, (FILETIME *)&t2);
	  
		int bs = (t1 - t2)/36000000000L;
		int bs1 = 0;
		if (bs<0)
		bs = (-bs);
		int bs2 = bs;
		while (bs2>0)
		{
		bs2 /= 10;
		bs1++;
		}
		if (t1<t2)
		buff1[22] = '+';
		
		  char *bz = buff1+25 - bs1;
	wsprintf(bz, "%d", bs);*/
	
	wsprintf(buff, buff1, days[st.wDayOfWeek], st.wDay, months[st.wMonth], st.wYear, st.wHour);
	
	strcpy(string,buff);
  
}