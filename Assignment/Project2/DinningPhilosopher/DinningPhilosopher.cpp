// DinningPhilosopher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "SharedData.h"
#include "Philosopher.h"

Philosopher* pPhilosopher[MAX_PHILOSOPHER];

void ReportStatus()
{
	for (int i = 0; i < MAX_PHILOSOPHER; i++)
		std::cout << pPhilosopher[i]->getStatus();
	std::cout << "\n";
}

int main()
{
	int id = 0, total=0;
	SharedData data;

	for (int i = 0; i < MAX_PHILOSOPHER; i++)
	{
		pPhilosopher[i] = new Philosopher(i, &data);
	}
	while (data.MoreRice())
	{
		ReportStatus();
		Sleep(100);
	}
	Sleep(100);
	//wait for all of them to finish
	for (int i = 0; i < MAX_PHILOSOPHER; i++)
		pPhilosopher[i]->isVacationing();
	
	ReportStatus();	
	for (int i = 0; i < MAX_PHILOSOPHER; i++)
		total = total + pPhilosopher[i]->getRiceConsume();
    std::cout << "\nTotal Rice Consume = " << total; 
}

