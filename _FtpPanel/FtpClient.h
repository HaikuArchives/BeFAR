/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#ifndef _FtpClient_h
#define _FtpClient_h

using namespace std;

#include <Application.h>
#include <NetEndpoint.h>
#include <string>

#define CONNECT_COMPLETE 'conc'
#define PUT_COMPLETE 'putc'
#define GET_COMPLETE 'getc'
#define MOVE_COMPLETE 'movc'
#define MAKE_COMPLETE 'makc'
#define CD_COMPLETE 'cdco'
#define	PWD_COMPLETE 'pwdc'
#define LS_COMPLETE 'lscm'
#define AMOUNT_SENT 'amst'

class FtpClient
{
public:
	FtpClient(BHandler *handler = NULL);
	~FtpClient();

	enum ftp_mode
	{
		binary_mode,
		ascii_mode
	};

	bool connect(const string &server, const string &login, const string &passwd);
	bool putFile(const string &local, const string &remote, ftp_mode mode = binary_mode);
	bool getFile(const string &remote, const string &local, ftp_mode mode = binary_mode);
	bool moveFile(const string &oldpath, const string &newpath);
	bool makeDir(const string &newdir);
	bool cd(const string &dir);
	bool pwd(string &dir);
	bool ls(BString &listing);
	void setPassive(bool on);

public:
	enum {
		ftp_complete = 1UL,
		ftp_connected = 2,
		ftp_passive = 4
	};

	unsigned long m_state;
	bool p_testState(unsigned long state);
	void p_setState(unsigned long state);
	void p_clearState(unsigned long state);

	bool p_sendRequest(const string &cmd);
	bool p_getReply(string &outstr, int &outcode, int &codetype);
	bool p_getReplyLine(string &line);
	bool p_openDataConnection();
	bool p_acceptDataConnection();

	BNetEndpoint 	*m_control;
	BNetEndpoint 	*m_data;
	BHandler 		*mHandler;
	BLooper			*mLooper;

};

#endif /* _FtpClient_h */ 