#pragma once
#include "MailBoxMessage.h"

class MsgList
{
private:
	struct NODE
	{
		NODE *Previous;
		MailBoxMessage* Msg;
		NODE *Next;
	};
	NODE *Head;
	NODE* Tail;
	int count;

public:
	MsgList();
	~MsgList();
	BOOL Get(MailBoxFormat* msg);
	void Put(MailBoxFormat* msg);
	int GetCount();
};

