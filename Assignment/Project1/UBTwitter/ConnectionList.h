#pragma once
#include <windows.h>
class ConnectionList
{
private:
	struct NODE
	{
		NODE *Previous;
		HANDLE hIndex;
		LPCTSTR pBuf;
		NODE *Next;
	};

	NODE *Head;
	NODE* Tail;
public:
	ConnectionList();
	~ConnectionList();
	void Remove(HANDLE hIndex);
	void Add(HANDLE hIndex, LPCTSTR pBuf);
	LPCTSTR PeekBuffer(HANDLE hIndex);
	LPCTSTR PeekTail();
	BOOL RemoveTail();
};

