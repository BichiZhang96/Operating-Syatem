#pragma once
#include "SharedData.h"
enum eState {
	THINKING,
	HUNGRY,
	WAITING,
	EATING,
	VACATION
};

class Philosopher
{
private:
	int RiceConsume;
	eState v_State;
	int myID;
	SharedData *Food;
public:
	Philosopher(int id, SharedData* data);
	~Philosopher();
	static DWORD WINAPI thread_func(LPVOID lpParam);
	void Run();
	char* getStatus();
	char* StateToString();
	int getRiceConsume();
	bool isVacationing();
};

