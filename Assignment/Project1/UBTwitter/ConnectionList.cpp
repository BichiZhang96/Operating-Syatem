#include "ConnectionList.h"

ConnectionList::ConnectionList()
{
	Head = NULL;
	Tail = Head;
}

ConnectionList::~ConnectionList()
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

LPCTSTR ConnectionList::PeekBuffer(HANDLE hIndex)
{
	NODE *tmp = Head;
	while(tmp != NULL)
	{
		if(tmp->hIndex == hIndex)
                   return tmp->pBuf;
		tmp = tmp->Next;
	} 
        return NULL;
}

void ConnectionList::Remove(HANDLE hIndex)
{
	NODE *tmp = Head;
	if (tmp != NULL)
	{
		do
		{
			if (tmp->hIndex == hIndex)
			{
				NODE *save = tmp;
				if (tmp->Previous != NULL) //is head?
				{
					if (tmp->Next != NULL) //is tail?
					{
						tmp->Next->Previous = tmp->Previous;
						tmp->Previous->Next = tmp->Next;
					}
					else //removing tail
					{
						tmp->Previous->Next = NULL;
						Tail = tmp->Previous;
					}
				}
				else //removing head
				{
					Head = tmp->Next;
					Head->Previous = NULL;
				}
				
				delete save;
				return;
			}
			if(tmp->Next !=NULL)
				tmp = tmp->Next;
		} while (tmp!=NULL);
	}
}

BOOL ConnectionList::RemoveTail()
{
	int page = -1;
	if (Tail != NULL)
	{
		NODE *tmp = Tail;
		if (Tail->Previous != NULL)
		{
			Tail = Tail->Previous;
			Tail->Next = NULL;
			delete tmp;
			return TRUE;
		}
	}
	return FALSE;
}

LPCTSTR ConnectionList::PeekTail()
{
	if (Tail != NULL)
		return Tail->pBuf;

	return NULL;
}

void ConnectionList::Add(HANDLE hIndex, LPCTSTR pBuf)
{
	NODE *tmp = new NODE;
	tmp->hIndex = hIndex;
	tmp->pBuf = pBuf;
	tmp->Previous = NULL;
	tmp->Next = Head;
	if (Head != NULL)
		Head->Previous = tmp;
	if (Tail == NULL)
		Tail = tmp;
	Head = tmp;
}