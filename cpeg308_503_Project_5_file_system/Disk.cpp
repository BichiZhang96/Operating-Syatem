#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_GLOBALS
#include "Disk.h"

#include <stdexcept>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

LPTSTR Disk::LPCSTRtoLPTSTR(LPCSTR data)
{
	TCHAR* tmp1 = new TCHAR[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, data, -1, tmp1, 64);
	return tmp1;
}

void Disk::open(const char *path, size_t nblocks) {

	FileDescriptor = CreateFile(LPCSTRtoLPTSTR(path),                // name of the write
		GENERIC_READ | GENERIC_WRITE,          // open for writing & reading
		FILE_SHARE_READ,									// share
		NULL,                   // default security
		OPEN_EXISTING,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (FileDescriptor == INVALID_HANDLE_VALUE)  //new file
	{
		FileDescriptor = CreateFile(LPCSTRtoLPTSTR(path),                // name of the write
			GENERIC_READ | GENERIC_WRITE,          // open for writing & reading
			FILE_SHARE_READ,						//
			NULL,                   // default security
			CREATE_NEW,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);

		LARGE_INTEGER file_size;
		file_size.LowPart = (int)nblocks * BLOCK_SIZE;
		file_size.HighPart = 0;
		bool status = SetFilePointerEx(FileDescriptor, file_size, 0, FILE_BEGIN);
		status = status && SetEndOfFile(FileDescriptor);
		if (!status)
		{
			char what[BUFSIZ];
			snprintf(what, BUFSIZ, "Unable to open %s: %s", path, strerror(errno));
			throw std::runtime_error(what);
		}
	}

	Blocks = nblocks;
	Reads = 0;
	Writes = 0;
}

Disk::~Disk() {
	if (FileDescriptor > 0) {
		printf("%lu disk block reads\n", Reads);
		printf("%lu disk block writes\n", Writes);
		CloseHandle(FileDescriptor);
		FileDescriptor = 0;
	}
}

void Disk::sanity_check(int blocknum, unsigned char *data) {
	char what[BUFSIZ];

	if (blocknum < 0) {
		snprintf(what, BUFSIZ, "blocknum (%d) is negative!", blocknum);
		throw std::invalid_argument(what);
	}

	if (blocknum >= (int)Blocks) {
		snprintf(what, BUFSIZ, "blocknum (%d) is too big!", blocknum);
		throw std::invalid_argument(what);
	}

	if (data == NULL) {
		snprintf(what, BUFSIZ, "null data pointer!");
		throw std::invalid_argument(what);
	}
}

int Disk::read(int blocknum, unsigned char *data) {
	sanity_check(blocknum, data);
    DWORD      lpNumberOfBytesRead =0;
	LARGE_INTEGER file_size;
	file_size.LowPart = blocknum * BLOCK_SIZE;
	file_size.HighPart = 0;
	if (SetFilePointerEx(FileDescriptor, file_size, 0, FILE_BEGIN))
	{
		
		BOOL status = ReadFile(FileDescriptor,
			(LPVOID)data,
			BLOCK_SIZE,
			&lpNumberOfBytesRead,
			NULL);
		if (!status || lpNumberOfBytesRead != BLOCK_SIZE) {
			char what[BUFSIZ];
			snprintf(what, BUFSIZ, "Unable to read %d: %s", blocknum, strerror(errno));
			throw std::runtime_error(what);
		}
	}

	Reads++;
	return lpNumberOfBytesRead;
}

int Disk::write(int blocknum, unsigned char *data) {
	sanity_check(blocknum, data);
    DWORD lpNumberOfBytesWritten =0;
	LARGE_INTEGER file_size;
	file_size.LowPart = blocknum * BLOCK_SIZE;
	file_size.HighPart = 0;
	if (SetFilePointerEx(FileDescriptor, file_size, 0, FILE_BEGIN))
	{
		BOOL status = WriteFile(FileDescriptor,
			(LPVOID)data,
			BLOCK_SIZE,
			&lpNumberOfBytesWritten,
			NULL);
		if (!status || lpNumberOfBytesWritten != BLOCK_SIZE) {
			char what[BUFSIZ];
			snprintf(what, BUFSIZ, "Unable to write %d: %s", blocknum, strerror(errno));
			throw std::runtime_error(what);
		}
	}

	Writes++;
	return lpNumberOfBytesWritten;
}

