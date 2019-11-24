#include "pch.h"
#include "SharedData.h"
#include <time.h>

SharedData::SharedData()
{ 
	//pound of rice
	Rice = 5000;
	for (int i = 0; i < MAX_PHILOSOPHER; i++)
		chopstic[i] = CreateSemaphore();
	pMutex = CreateSemaphore();
	srand(time(NULL));
}


SharedData::~SharedData()
{
	for (int i = 0; i < MAX_PHILOSOPHER; i++)
		if(chopstic[i] != NULL )
			CloseHandle(chopstic[i]);

	if (pMutex != NULL)
		CloseHandle(pMutex);
}

//This function implements the critical section that protects the share variable Rice
int SharedData::Eat(int who, int* state, int new_state)
{
	int pounf_of_rice = 0;

	//////////////////////////////////////////////////////////
	//Start of Critical Section
	if (who % 2 == 1)
	{
		getLeftChopstic(who);
		getRightChopstic(who);
	
	
		*state = new_state;
		if (Rice > 0) {
			Rice--;
			pounf_of_rice++;
		}
	
	//Two philosophers are allowed to run this section concurentle or in parallel
		ConsumeRice();
	
	//End of Critical Section
	//////////////////////////////////////////////////////////
		putLeftChopstic(who);
		putRightChopstic(who);
	}
	else
	{
		getRightChopstic(who);
		getLeftChopstic(who);
		*state = new_state;
		if (Rice > 0) {
			Rice--;
			pounf_of_rice++;
		}

		//Two philosophers are allowed to run this section concurentle or in parallel
		ConsumeRice();

		//End of Critical Section
		//////////////////////////////////////////////////////////
		putRightChopstic(who);
		putLeftChopstic(who);
	}
		

	return pounf_of_rice;
}

//This function creates a nameless semaphore.
//TODO: uncomment the code bellow and replace ? with appropriate number(s)
HANDLE SharedData::CreateSemaphore()
{
	HANDLE hSemaphore = NULL;
		hSemaphore = CreateSemaphoreA(
							NULL,         // use paging file 
							1,            // initial count value
							1,            // maximum count                                  
							NULL);        // name of mapping object
	return hSemaphore;
}

bool SharedData::MoreRice()
{
	return (Rice > 0);
}

void SharedData::wait(HANDLE pParam)
{
	DWORD status;
	if (pParam != NULL)
		status = WaitForSingleObject(pParam, INFINITE);
	Sleep(5);
}

void SharedData::signal(HANDLE pParam)
{
	if (pParam != NULL)
		ReleaseSemaphore(pParam, 1, NULL);
}

void SharedData::getLeftChopstic(int who)
{
	getChopstic(who);
}


void SharedData::getRightChopstic(int who)
{
	getChopstic((who+1) % MAX_PHILOSOPHER);
}

void SharedData::getChopstic(int who)
{
	wait(chopstic[who]);
}

void SharedData::putLeftChopstic(int who)
{
	putChopstic(who);
}


void SharedData::putRightChopstic(int who)
{
	putChopstic((who + 1) % MAX_PHILOSOPHER);
}

void SharedData::putChopstic(int who)
{
	signal(chopstic[who]);
}

void SharedData::ConsumeRice()
{
	Sleep(5);
}
