//#include "stdafx.h"
#include "pch.h"
#include "LinkedList.h"

#define NULL 0

LinkedList::LinkedList()
{
	Head = NULL;
	Tail = Head;
}

LinkedList::~LinkedList()
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

//This function is to help debuging
void LinkedList::List()
{
	NODE *tmp = Head;
	
	while(tmp != NULL)
	{
		tmp = tmp->Next;
	} 
}

int LinkedList::RemoveTail()
{
	int page = -1;
	if (Tail != NULL)
	{
		NODE *tmp = Tail;
		page = Tail->page;
		if (Tail->Previous != NULL)
		{
			Tail = Tail->Previous;
			Tail->Next = NULL;
			delete tmp;
		}
	}
	return page;
}

void LinkedList::MoveToHead(int page)
{
	if (Head!=NULL && Head->page!=page) //if it's already on top do nothing
	{
		Remove(page);
		AddToHead(page);
	}
}

void LinkedList::Remove(int page)
{
	NODE *tmp = Head;
	if (tmp != NULL)
	{
		do
		{
			if (tmp->page == page)
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

void LinkedList::AddToHead(int page)
{
	NODE *tmp = new NODE;
	tmp->page = page;
	tmp->Previous = NULL;
	tmp->Next = Head;
	if(Head!=NULL)
		Head->Previous = tmp;
	if (Tail == NULL)
		Tail = tmp;
	Head = tmp;
}