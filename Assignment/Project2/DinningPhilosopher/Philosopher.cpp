#include "pch.h"
#include "Philosopher.h"

#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>

HANDLE pThread = NULL;
DWORD thread_id = 0;

DWORD WINAPI Philosopher::thread_func(LPVOID lpParam) {
	((Philosopher*)lpParam)->Run();
	return 0;
}

Philosopher::Philosopher(int id, SharedData* data)
{
	v_State = VACATION;
	myID = id;
	RiceConsume = 0;
	Food = data;
	srand(time(NULL));
	pThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_func, (LPVOID)this, 0, &thread_id);
}


Philosopher::~Philosopher()
{ 
	CloseHandle(pThread);
}

void Philosopher::Run()
{
	v_State = EATING;

	while (Food->MoreRice())
	{
		if (v_State == THINKING)
		{
			v_State = HUNGRY;
		}
		else if (v_State == HUNGRY)
		{
			v_State = WAITING;
			RiceConsume = RiceConsume + Food->Eat(myID, (int*)&v_State, (int)EATING);
		}
	    if (v_State == EATING)
		{
			v_State = THINKING;
		}
		
		Sleep(rand() % 10);
	}
	v_State = VACATION;
}

bool Philosopher::isVacationing()
{
	WaitForSingleObject(pThread, INFINITE);
	return (v_State == VACATION);
}

char* Philosopher::getStatus()
{
	char tmp[64];
	int len = sprintf_s(tmp, "%d %d %s |", myID, RiceConsume, StateToString());
	char* buff = new char[64];
	strcpy_s(buff, 64, (char*)&tmp[0]);
	return (buff);
}

char* Philosopher::StateToString()
{
	if (v_State == THINKING)
		return(("THINKING"));
	if (v_State == HUNGRY)
		return(("HUNGRY"));
	if (v_State == EATING)
		return(("EATING"));
	if (v_State == WAITING)
		return(("WAITING"));
	if (v_State == VACATION)
		return(("VACATION"));
	return nullptr;
}

int Philosopher::getRiceConsume()
{
	return RiceConsume;
}