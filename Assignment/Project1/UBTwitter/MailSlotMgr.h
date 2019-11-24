#pragma once
#include "MailBoxMessage.h"
#include "SharedMemoryIO.h"
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

#define INVALID_MAILSLOT -1

class MailslotMrg : SharedMemoryIO
{
public:
	MailslotMrg();
	~MailslotMrg();
	int GetMailslotID();
	std::string ReadTweets();
	BOOL WriteTweets(int to, std::string Msg);
	
	//MailBoxFormat* ReadMailslot(int id);
	//BOOL WriteMailslot(int from, int to, int msg_id, std::string lpszMessage); //LPTSTR
	//BOOL WriteMailslot(int from, int to, MailBoxFormat *data);
	void EmptyMailbox();
	//void ReleaseSemaphores();
private:
	//
	LPTSTR GetUniqueIdentifierW(LPTSTR name, int id, int sub_id);
	LPCSTR GetUniqueIdentifierM(LPTSTR name, int id, int sub_id);
	//
	void SendBroadcastMsg(std::string Msg);
	MailBoxMessage * pLocalReadBuffer;
	MailBoxMessage * pLocalWriteBuffer;
};

