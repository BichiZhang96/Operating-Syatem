#pragma once
#include <string>
#include <windows.h>
#define BUFFER_LENGTH 96

struct DataFormat
{
	wchar_t Buffer[BUFFER_LENGTH];
	int ID;
	int Index;
	int NextSize;
	int MessageCount;
};

struct MailBoxFormat
{
	int FROM;
	DataFormat pData;
};

class MailBoxMessage
{
public:
	MailBoxMessage();
	MailBoxMessage(MailBoxFormat* msg);
	~MailBoxMessage();
	LPCVOID FormatMsgOut(int from, std::string Msg, int msg_id);
	std::string FormatMsgIn(); // LPVOID data_in);
	static int GetFormatSize();
	int GetMaxMessageSize();
	int MailBoxMessage::GetFrom();
	BOOL IsValidData(int max);
	BOOL IsValidData();
	BOOL CopyTo(MailBoxFormat *msg);
	BOOL CopyFrom(MailBoxFormat* msg);
	MailBoxFormat * data;
private:
	void AllocateMsg();
};

