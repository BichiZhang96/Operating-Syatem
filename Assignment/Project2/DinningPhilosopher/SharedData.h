#pragma once
#include <windows.h>

#define MAX_PHILOSOPHER 5
class SharedData
{
private:
	int Rice;
	HANDLE chopstic[MAX_PHILOSOPHER];
	HANDLE pMutex;
	HANDLE CreateSemaphore();
public:
	SharedData();
	~SharedData();
	int Eat(int who, int* state, int new_state);
	bool MoreRice();
private:
	void wait(HANDLE pParam);
	void signal(HANDLE pParam);
	void getLeftChopstic(int who);
	void getRightChopstic(int who);
	void getChopstic(int who);
	void putLeftChopstic(int who);
	void putRightChopstic(int who);
	void putChopstic(int who);
	void ConsumeRice();
};

