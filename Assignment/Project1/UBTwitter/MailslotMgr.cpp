#include "Mailslotmgr.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <stringapiset.h>
#include <time.h>

#define MAXBOX 10
#define EMPTY  1
#define FULL   2
static HANDLE hMailBox[MAXBOX];		//handle to all active mailslot


static int    msg_id = 0;

LPTSTR pMailBoxName = TEXT("Global\\Twiter_Mailslot");				//format for share memmory name
//LPTSTR pMailBoxName = TEXT("\\\\.\\mailslot\\Twiter_Mailslot");			//format for mailbox

//Only one server per process
int    thisMailslot   = INVALID_MAILSLOT;   //valid server/owner id: Range is: 0 to 9

MailslotMrg::MailslotMrg()
{
	int i;
	pLocalReadBuffer = new MailBoxMessage();		//buffer for the server
	pLocalWriteBuffer = new MailBoxMessage();		//buffer for the client

	srand((unsigned)time(NULL));
	msg_id = rand();

	for (i = 0; i < MAXBOX; i++)
	{
		hMailBox[i] = NULL;                         //no active mailslot
	}

	LPCSTR unique_id =NULL;
	//Create the mailslot this process needs to monitor
	for (i = 0; i < MAXBOX; i++)
	{
		unique_id = GetUniqueIdentifierM(pMailBoxName, i, 0);
		//The mailbox already belong to someone else, so create a connection to it
		hMailBox[i] = myCreateFile(unique_id,
			GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

		if (hMailBox[i] == INVALID_HANDLE_VALUE || hMailBox[i] == NULL)
		{
			hMailBox[i] = myCreateMailslot(unique_id, pLocalReadBuffer->GetFormatSize(), MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);
			if (hMailBox[i] != INVALID_HANDLE_VALUE && hMailBox[i] != NULL)
			{
				thisMailslot = i;
				break;
			}
		}
		else {   
		}
	}

	if (thisMailslot != INVALID_MAILSLOT)
		SendBroadcastMsg("Hello UB!");
}

MailslotMrg::~MailslotMrg()
{
	if (hMailBox[thisMailslot] != NULL)
	{
		CloseHandle(hMailBox[thisMailslot]);
	}
	if (pLocalReadBuffer != NULL)
		delete pLocalReadBuffer;
	if (pLocalWriteBuffer != NULL)
		delete pLocalWriteBuffer;
}

int MailslotMrg::GetMailslotID() {
	return thisMailslot;
}

std::string MailslotMrg::ReadTweets()
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult = FALSE;
	std::string data = std::string("");

	fResult = myGetMailslotInfo(hMailBox[thisMailslot],		// mailslot handle
		(LPDWORD)NULL,				// no maximum message size
		&cbMessage,                   // size of next message         
		&cMessage,                    // number of messages         
		(LPDWORD)NULL);              // no read time-out 

	if (fResult && cMessage > 0)
	{ 
		fResult = myReadFile(hMailBox[thisMailslot], (LPVOID)pLocalReadBuffer->data, cbMessage, &cbRead, NULL);
		if (fResult) {
			data = pLocalReadBuffer->FormatMsgIn();

			//If we are receiving this message for the first time
			//establish a connection to this mail box
			if (pLocalReadBuffer->IsValidData(MAXBOX))
			{
				int from = pLocalReadBuffer->GetFrom();
				if ((hMailBox[from] == NULL))
				{
					LPCSTR unique_id = GetUniqueIdentifierM(pMailBoxName, from, 0);
					hMailBox[from] = myCreateFile(unique_id,
						GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				}
			}
		}
	}
	return data;
}

BOOL MailslotMrg::WriteTweets(int to, std::string Msg)
{
	BOOL fResult = FALSE;    DWORD cbWritten;
	LPCVOID lpszMessage = pLocalWriteBuffer->FormatMsgOut(thisMailslot, Msg, msg_id);

	if(hMailBox[to] != NULL)
		fResult = myWriteFile(hMailBox[to], lpszMessage, MailBoxMessage::GetFormatSize(), &cbWritten, (LPOVERLAPPED)NULL);

	msg_id++;
	return fResult;
}

LPTSTR MailslotMrg::GetUniqueIdentifierW(LPTSTR name, int id, int sub_id)
{
	TCHAR *tmp = new TCHAR[64];
	_stprintf(tmp, L"%s%02d%02d", name, id, sub_id);
	return tmp;
}

LPCSTR MailslotMrg::GetUniqueIdentifierM(LPTSTR name, int id, int sub_id)
{
	CHAR* tmp1 = new CHAR[64];
	TCHAR *tmp = GetUniqueIdentifierW(name, id, sub_id);
	::WideCharToMultiByte(CP_ACP, 0, tmp, -1, tmp1, 64, NULL, FALSE);
	return tmp1;
}

//Send message to all active mailbox (except yourself).
void MailslotMrg::SendBroadcastMsg(std::string Msg)
{
	for (int index = 0; index < MAXBOX; index++)
		if( (index != thisMailslot) && (hMailBox[index] != NULL) )
		{
			WriteTweets(index, Msg);
		}
}
