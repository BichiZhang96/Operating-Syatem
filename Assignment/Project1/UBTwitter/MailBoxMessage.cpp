#include "MailBoxMessage.h"

#include <tchar.h> 
#include <stdio.h> 
#include <strsafe.h>

#include <stringapiset.h>

MailBoxMessage::MailBoxMessage()
{
	AllocateMsg();
}

MailBoxMessage::MailBoxMessage(MailBoxFormat* msg)
{
	AllocateMsg();
	CopyFrom(msg);
}

BOOL MailBoxMessage::CopyTo(MailBoxFormat* msg)
{
	msg->FROM = this->data->FROM;
	msg->pData.ID = this->data->pData.ID;
	msg->pData.Index = this->data->pData.Index;
	msg->pData.NextSize = this->data->pData.NextSize;
	msg->pData.MessageCount = this->data->pData.MessageCount;
	CopyMemory(msg->pData.Buffer, this->data->pData.Buffer, GetFormatSize());
	return TRUE;
}

BOOL MailBoxMessage::CopyFrom(MailBoxFormat* msg)
{
	this->data->FROM = msg->FROM;
	this->data->pData.ID = msg->pData.ID;
	this->data->pData.Index = msg->pData.Index;
	this->data->pData.NextSize = msg->pData.NextSize;
	this->data->pData.MessageCount = msg->pData.MessageCount;
	CopyMemory(this->data->pData.Buffer, msg->pData.Buffer, GetFormatSize());
	return TRUE;
}
MailBoxMessage::~MailBoxMessage()
{
	if (data != NULL)
		delete data;
}

void MailBoxMessage::AllocateMsg()
{
	data = (MailBoxFormat*)malloc(sizeof(MailBoxFormat));
}

int MailBoxMessage::GetFormatSize()
{
	return sizeof(MailBoxFormat);
}

int MailBoxMessage::GetMaxMessageSize()
{
	return BUFFER_LENGTH;
}

int MailBoxMessage::GetFrom()
{
	return this->data->FROM;
}

BOOL MailBoxMessage::IsValidData(int max)
{
	return (this->data->FROM >= 0 && this->data->FROM < max);
}

BOOL MailBoxMessage::IsValidData()
{
	return (this->data->FROM >= 0);
}

LPCVOID MailBoxMessage::FormatMsgOut(int from, std::string Msg, int msg_id)
{
	int msg_size = (int)Msg.length();
	if (msg_size >= GetMaxMessageSize())
		msg_size = GetMaxMessageSize()-1;

	this->data->FROM = from;
	this->data->pData.ID = msg_id;
	this->data->pData.Index = 0;
	this->data->pData.NextSize = msg_size;
	this->data->pData.MessageCount = 1;
	::MultiByteToWideChar(CP_ACP, 0, Msg.c_str(), -1, data->pData.Buffer, msg_size);
	data->pData.Buffer[msg_size] = NULL;
	return this->data;
}

std::string MailBoxMessage::FormatMsgIn() //LPVOID data_in)
{
	TCHAR tmp[512];
	CHAR return_data[512];
	//CopyMemory((PVOID)this->data, data_in, GetFormatSize());
	_stprintf(tmp, L"Msg ID: %d From %d to Me => %s\0", this->data->pData.ID, this->data->FROM, this->data->pData.Buffer);
	::WideCharToMultiByte(CP_ACP, 0, tmp, -1, return_data, 512, NULL, FALSE);
	return std::string(return_data);
}