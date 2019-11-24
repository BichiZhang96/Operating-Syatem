#include "LinkedList.h"

MsgList::MsgList()
{
	Head = NULL;
	Tail = Head;
	count = 0;
}

MsgList::~MsgList()
{
	if (Head != NULL)
	{
		while (Head->Next != NULL)
		{
			Tail = Head;
			Head = Head->Next;
			delete Tail;
		}
		delete Head;
	}
}

BOOL MsgList::Get(MailBoxFormat* msg)
{
	if (Tail != NULL)
	{
		NODE *tmp = Tail;
		tmp->Msg->CopyTo(msg);
		count--;
		if (Tail->Previous != NULL)
		{
			Tail = Tail->Previous;
			Tail->Next = NULL;
			delete tmp;
		}
		return TRUE;
	}
	return FALSE;
}

void MsgList::Put(MailBoxFormat* msg)
{
	NODE *tmp = new NODE;
	tmp->Msg = new MailBoxMessage(msg);

	tmp->Previous = NULL;
	tmp->Next = Head;
	if(Head!=NULL)
		Head->Previous = tmp;
	if (Tail == NULL)
		Tail = tmp;
	Head = tmp;
	count++;
}

int MsgList::GetCount()
{
	return count;
}