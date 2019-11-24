#include "SharedMemoryIO.h"
#include "MailBoxMessage.h"
#include "ConnectionList.h"
#include "LinkedList.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

int BUF_SIZE = 256;
#define NO_DATA 999

ConnectionList *pConnectionList = NULL;
MsgList        *pMsgList = NULL;
HANDLE         serverHandle = NULL;

HANDLE pThread = NULL;
DWORD thread_id = 0;
BOOL  running_flag = TRUE;

MailBoxMessage * pTempReadBuffer;

int            MaxMessageSize = 0;

SharedMemoryIO::SharedMemoryIO()
{
	pConnectionList = new ConnectionList();
	pMsgList = new MsgList();
	pTempReadBuffer = new MailBoxMessage();
}

SharedMemoryIO::~SharedMemoryIO()
{
	running_flag = FALSE;
	//unmap all views
	BOOL flag = TRUE;
	LPCTSTR pBuf = pConnectionList->PeekTail();
	while (flag && pBuf != NULL) {
		UnmapViewOfFile(pBuf);
		flag = pConnectionList->RemoveTail();
		pBuf = pConnectionList->PeekTail();
	}

	if (pThread != NULL)
		CloseHandle(pThread);

	delete pMsgList;
	delete pConnectionList;
}

//LPCVOID
BOOL SharedMemoryIO::myWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	//return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	//
	LPCTSTR pBuf = pConnectionList->PeekBuffer(hFile);
	if (pBuf != NULL)
	{
		Write((PVOID)pBuf, (PVOID)lpBuffer, nNumberOfBytesToWrite);
		return TRUE;
	}
	return FALSE;
}

//Open a connection with an existing mailbox
HANDLE SharedMemoryIO::myCreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	//return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	HANDLE		hMapFile = NULL;
	LPCTSTR     pBuf = NULL;
	LPTSTR unique_id = LPCSTRtoLPTSTR(lpFileName);
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,   // read/write access
				FALSE,                 // do not inherit the name
				unique_id);            // name of mapping object
	if (hMapFile != NULL)
	{
		pBuf = GetBuffer(hMapFile);
		if (pBuf != NULL)
			pConnectionList->Add(hMapFile, pBuf);
	}
	
	return hMapFile;
}

BOOL SharedMemoryIO::myGetMailslotInfo(HANDLE  hMailslot, LPDWORD lpMaxMessageSize, LPDWORD lpNextSize, LPDWORD lpMessageCount, LPDWORD lpReadTimeout)
{
	//return GetMailslotInfo(hMailslot, lpMaxMessageSize, lpNextSize, lpMessageCount, lpReadTimeout);
	*lpMessageCount = pMsgList->GetCount();
	return (lpMessageCount > 0);
}

HANDLE SharedMemoryIO::myCreateMailslot(LPCSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	//HANDLE hSlot = CreateMailslotA(lpName, nMaxMessageSize, lReadTimeout, lpSecurityAttributes);

	LPTSTR unique_id = LPCSTRtoLPTSTR(lpName);
	serverHandle = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file 
		lpSecurityAttributes,    //security
		PAGE_READWRITE,          // read/write access                 
		0,                       // maximum object size (high-order DWORD)                 
		nMaxMessageSize,         // maximum object size (low-order DWORD)                 
		unique_id);              // name of mapping object

	if (serverHandle != NULL)
	{
		serverBuf = GetBuffer(serverHandle);
		if (serverBuf != NULL)
		{
			MaxMessageSize = nMaxMessageSize;
			EmptyMailbox(serverBuf);
			pConnectionList->Add(serverHandle, serverBuf);
			pThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_func, (LPVOID)this, 0, &thread_id);
		}
	}
	return serverHandle;
}

BOOL SharedMemoryIO::myReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	//return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	BOOL return_val;
	if (pMsgList->GetCount() > 0)
	{
		return_val = pMsgList->Get((MailBoxFormat*)lpBuffer);
		return TRUE;
	}
	return FALSE;
}

LPCSTR SharedMemoryIO::LPTSTRtoLPCSTR(LPTSTR data)
{
	CHAR* tmp1 = new CHAR[64];
	::WideCharToMultiByte(CP_ACP, 0, data, -1, tmp1, 64, NULL, FALSE);
	return tmp1;
}

LPTSTR SharedMemoryIO::LPCSTRtoLPTSTR(LPCSTR data)
{
	TCHAR* tmp1 = new TCHAR[64];
	::MultiByteToWideChar(CP_ACP, 0, data, -1, tmp1, 64);
	return tmp1;
}

//This function return a pointer to the shared memory address, by calling mapviewofile
LPCTSTR SharedMemoryIO::GetBuffer(HANDLE hMapFile)
{
	LPCTSTR pBuf=(LPTSTR)MapViewOfFile(hMapFile,	        // handle to map object
									FILE_MAP_ALL_ACCESS,	// read/write permission   
									0, 0, BUF_SIZE);
	return pBuf;
}

void SharedMemoryIO::Write(PVOID dest, PVOID source, int size)
{
	CopyMemory((PVOID)dest, source, size);
}

void SharedMemoryIO::Read(PVOID dest, PVOID source, int size)
{
	CopyMemory((PVOID)dest, source, size);
}

void SharedMemoryIO::EmptyMailbox(LPCTSTR pBuf)
{
	int data = NO_DATA;

	if (pBuf != NULL)
		Write((LPTSTR)pBuf, (LPTSTR)&data, sizeof(int));
}

void SharedMemoryIO::MonitorIncomingData(LPCTSTR shared_memory_to_monitor)
{
	int data;
	LPTSTR local_serverBuf = (LPTSTR)shared_memory_to_monitor;
	if (shared_memory_to_monitor != NULL)
	{
		Read(&data, local_serverBuf, sizeof(int));
		if (data != NO_DATA)
		{
			Read((PVOID)pTempReadBuffer->data, local_serverBuf, MaxMessageSize);
			if (pTempReadBuffer->IsValidData())
			{ 
				pMsgList->Put(pTempReadBuffer->data);
				EmptyMailbox(local_serverBuf);
			}
		}
	}
}

//This funtion is the main server thread. Call this function as follows:
//pThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_func, (LPVOID)this, 0, &thread_id);
DWORD WINAPI SharedMemoryIO::thread_func(LPVOID lpParam) {
	while (running_flag)
	{
		((SharedMemoryIO*)lpParam)->MonitorIncomingData(((SharedMemoryIO*)lpParam)->serverBuf);
		Sleep(1);
	}
	return 0;
}