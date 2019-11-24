#pragma once
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
//This class implement the shared memory IO
class SharedMemoryIO
{
public:
	SharedMemoryIO();
	~SharedMemoryIO();
	HANDLE myCreateMailslot(LPCSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	BOOL myGetMailslotInfo(HANDLE  hMailslot, LPDWORD lpMaxMessageSize, LPDWORD lpNextSize, LPDWORD lpMessageCount, LPDWORD lpReadTimeout);
	HANDLE myCreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	BOOL myWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
	BOOL myReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

	void MonitorIncomingData(LPCTSTR shared_memory_to_monitor);
	static DWORD WINAPI thread_func(LPVOID lpParam);
	LPCTSTR        serverBuf;
private:
	LPCTSTR GetBuffer(HANDLE hMapFile);
	LPCSTR LPTSTRtoLPCSTR(LPTSTR data);
	LPTSTR LPCSTRtoLPTSTR(LPCSTR data);
	void EmptyMailbox(LPCTSTR pBuf);
	void Write(PVOID dest, PVOID source, int size);
	void Read(PVOID dest, PVOID source, int size);
};

