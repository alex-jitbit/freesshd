#ifndef __LOG_H
#define __LOG_H

enum conn_state
{
	success = 0,
	connecting = success + 1,
	bad_pass = connecting + 1,
	bad_key = bad_pass + 1,
	host_restriction = bad_key + 1,
	user_restriction = host_restriction + 1,
	disconnect = user_restriction + 1,
	user_unknown = disconnect +1
};

enum service_type
{
	shell = 0,
	sftp = 1,
	tunnel = 2
};

void WriteLog(char text[1024]);
void WriteConnectionLog(CSSHUser *User, int service, enum conn_state state, int auth_method = 0);
void WriteServiceLog(CSSHUser *User, enum service_type service, int state);
void WriteUserLog(CSSHUser *User, char *text);

void UpdateNotifyIcon();

void DateToString(DATE newVal,char *string);

#endif