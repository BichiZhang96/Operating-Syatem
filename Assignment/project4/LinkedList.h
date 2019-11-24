#pragma once
class LinkedList
{
private:
	struct NODE
	{
		NODE *Previous;
		int page;
		NODE *Next;
	};

	NODE *Head;
	NODE* Tail;
public:
	LinkedList();
	~LinkedList();
	int RemoveTail();
	void Remove(int page);
	void AddToHead(int page);
	void MoveToHead(int page);
	void List();
};

