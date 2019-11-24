// MemoryManagement.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Universit of Bridgeport
// CPEG 308/503 F2019 Project #4

#include "pch.h"
#include <iostream>
#include <string>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LinkedList.h"

#define NOT_FOUND			-1
#define EMPTY				-1
#define NUMBER_OF_FRAMES	3        //for physical memory
#define FRAME_SIZE			256      //for pgysical memory
#define NUMBER_OF_PAGES		256      //for logical memory
#define PAGE_SIZE			256      //for logical memory
#define PAGE_NUMBER_INDEX	0
#define FRAME_NUMBER_INDEX	1

LinkedList *history = NULL;

enum ALGORITHM
{
	FIFO,
	LRU,
	OPTIMUM
};

union Address_Type {
	int  iNum;
	unsigned char strNum[4];
};

FILE *vm_file_Ptr;
errno_t err;
unsigned char* physical_memory[NUMBER_OF_FRAMES];
int TLB[NUMBER_OF_FRAMES][2];

void init_PhysicalMemory();
int read_Address(int* buffer, int length);
bool load_LogicalToPhysical(int page_number, int frame_index);
int is_PageLoaded(int page_numbr);
void update_TLB(int page_number, int frame_index);
void printData(unsigned char opcode);
int getPageNumber(int address_param);
int getPageOffset(int address_param);
unsigned char fetchData(int frame_index, int offset);
int findEmptyFrame();
int swapPage_FIFO(int frame_index_param, int page);
int swapPage_LRU(int frame_index_param, int page);
int swapPage_Optimal(int frame_index, int *addressList, int current, int addressLength);
int Run(int* address_list, int length, ALGORITHM alg);
int getPageNumber_from_TLB(int index);

int main()
{
	int previous_page = -1, page_jump = 0, length;
	int FIFO_fault_count = 0, LRU_fault_count = 0, OPTM_fault_count = 0;
	int addressPtr[128];
	history = new LinkedList();

	length = read_Address(&addressPtr[0], 128);

	std::cout << "\nFIFO Algorithm\n";
	FIFO_fault_count = Run(addressPtr, length, FIFO);
	delete history;
	history = new LinkedList();

	std::cout << "\n\nLRU Algorithm\n";
	LRU_fault_count = Run(addressPtr, length, LRU);

	std::cout << "\n\nOptimal Algorithm\n";
	OPTM_fault_count = Run(addressPtr, length, OPTIMUM);

	std::cout << "\n\nFIFO: # of Page Fault: " << FIFO_fault_count << "\n";
	std::cout << " LRU: # of Page Fault: " << LRU_fault_count << "\n";
	std::cout << "OPIM: # of Page Fault: " << OPTM_fault_count << "\n";
	return 0;
}

void printData(unsigned char opcode)
{
	std::cout << opcode;
}

void init_PhysicalMemory()
{
	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
	{
		physical_memory[i] = NULL;
		TLB[i][0] = EMPTY;
		TLB[i][1] = EMPTY;
	}
}

//This function loads data the specify page from logical memory (disk) and copy it to physical memory
//at the specify frame index.
bool load_LogicalToPhysical(int page_number, int frame_index)
{
	if (page_number < 0 || page_number>(NUMBER_OF_PAGES - 1))
		return NULL;
	if (frame_index < 0 || frame_index>(NUMBER_OF_FRAMES - 1))
		return NULL;

	unsigned char* Ptr = (unsigned char*)malloc(PAGE_SIZE);

	err = fopen_s(&vm_file_Ptr, "BACKING_STORE.bin", "rb");
	if (vm_file_Ptr != NULL)
	{
		int flag = fseek(vm_file_Ptr, page_number * PAGE_SIZE, 0);
		flag = fread(Ptr, 1, PAGE_SIZE, vm_file_Ptr);
		fclose(vm_file_Ptr);
		physical_memory[frame_index] = Ptr;
		return true;
	}

	return false;
}

void update_TLB(int page_number, int frame_index)
{
	if (frame_index < 0 || frame_index > (NUMBER_OF_FRAMES - 1))
		return;

	TLB[frame_index][PAGE_NUMBER_INDEX] = page_number;
	TLB[frame_index][FRAME_NUMBER_INDEX] = frame_index;

	std::cout << "[";
	for(int i=0; i< NUMBER_OF_FRAMES; i++)
		if( i < (NUMBER_OF_FRAMES-1) )
			std::cout << TLB[i][PAGE_NUMBER_INDEX] << " ";
		else 
			std::cout << TLB[i][PAGE_NUMBER_INDEX] << "]";
}

int read_Address(int *buffer, int length)
{
	int i, j, ret, count = 0;

	std::cout << "reference string\n";

	memset(buffer, 0, length);
	err = fopen_s(&vm_file_Ptr, "addresses.txt", "r");
	if (vm_file_Ptr != NULL)
	{
		for (j = 0; j < length; j++)
		{
			ret = fscanf_s(vm_file_Ptr, "%d, ", &i);
			if (ret > 0)
			{
				buffer[j] = i;
				std::cout << getPageNumber(i) << " ";
				count++;
			}
			else break;
		}
		fclose(vm_file_Ptr);
	}

	std::cout << "\n";

	return count;
}

//This function takes a 32 bits number and return the corresponding page number.
//The format is "XXPO" where each of those letter are 8bits
//O is the least significant byte which represent the offset number
//P represent the page number.
//This can be implemented using the methods:
//1) using structure type "Address_Type"
//2) using bit manipulation, mask and shift (note: & is logical and, | is logical or, << is shift left, >> is shift right
// 0xFFFF is the mask to eliminate the most significant 2 bytes
int getPageNumber(int address_param)
{
	int page = 0;
	Address_Type address;
	address.iNum = address_param;
	page = address.strNum[1];
	return page;
}

//This function takes a 32 bits number and return the corresponding offset number.
//The format is "XXPO" where each of those letter are 8bits
//O is the least significant byte which represent the offset number
//P represent the page number.
//This can be implemented using the methods:
//1) using structure type "Address_Type"
//2) using bit manipulation, mask and shift (note: & is logical and, | is logical or, << is shift left, >> is shift right
// 0xFFFF is the mask to eliminate the most significant 2 bytes
int getPageOffset(int address_param)
{
	int offset = 0;
	Address_Type address;
	address.iNum = address_param;
	offset = address.strNum[0];
	return offset;
}

//This function determine if a page is in memory by checking the TLB
//If it exist it returns the frame index
//if not it returns NOT_FOUND
int is_PageLoaded(int page_numbr)
{
	int frame_index = NOT_FOUND;

	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
		if(TLB[i][PAGE_NUMBER_INDEX] == page_numbr)
		{
			frame_index = TLB[i][FRAME_NUMBER_INDEX];
			i = NUMBER_OF_FRAMES;                       //to stop the loop
	        }

	return frame_index;
}

//This function returns reads 1 byte from physical memory and return its
unsigned char fetchData(int frame_index, int offset)
{
	unsigned char data;
	data = physical_memory[frame_index][offset];
	return data;
}

//This function return the the index of the first empty frame in the TLB
int findEmptyFrame()
{
	int frame_index = NOT_FOUND;
	
	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
		if (TLB[i][PAGE_NUMBER_INDEX] == NOT_FOUND)
		{
			frame_index = i;
			i = NUMBER_OF_FRAMES;    //to stop the loop
		}
	return frame_index;
}

int getPageNumber_from_TLB(int index)
{
	return TLB[index][PAGE_NUMBER_INDEX];
}

int swapPage_FIFO(int frame_index_param, int page)
{
	int frame_index = frame_index_param;
	int swap_page;

	if (frame_index == NOT_FOUND) {
		frame_index = findEmptyFrame();

		if (frame_index == NOT_FOUND)                 //need to swap
		{
			swap_page = history->RemoveTail();
			frame_index = is_PageLoaded(swap_page);   //get the frame index for the page to be removed
		}
		history->AddToHead(page);
	}

	return frame_index;
}

int swapPage_LRU(int frame_index_param, int page)
{
	int frame_index = frame_index_param;
	int swap_page;

	if (frame_index == NOT_FOUND)
	{
		frame_index = findEmptyFrame();

		int swap_page;
		if (frame_index == NOT_FOUND)
		{
			swap_page = history->RemoveTail();
			frame_index = is_PageLoaded(swap_page);
		}
		history->AddToHead(page);
	}
	else
		history->MoveToHead(page);

	return frame_index;
}

//This function decide which page to swap out of memory using the "Optimum" replacement algorithm
//findEmptyFrame()
//getPageNumber_from_TLB
//getPageNumber
int swapPage_Optimal(int frame_index_param, int *addressList, int current, int addressLength)
{
	int location = 0;
	int frame_index = frame_index_param;
	int addressIndex = current;
	int page;
	int TLBpage;
	int PC, TLB_index;

	if (frame_index_param == NOT_FOUND)
	{
		frame_index = findEmptyFrame();

		if (frame_index == NOT_FOUND)
			{
            //TODO
            //Either the frame is already in TLB table or an empty frame was found, then no need for a swap, so return
 	    //we need to pick a page from the TLB to remove or swap out. 
            //Starting from the current position in the "reference string" (in our case is the array "addressList"), We have to pick 
            //the page number that is furthest away. Remember the number in the addressList are 32bits, so you have to extract the page number.

            /* YOUR CODE GOES HERE */
			//1.start from begining of the reference string to the end
			//2.using the page number, ask if it is in memory
			//3.if in memory, go to step 1
			//4.if not in memory
			//	a. record the page fault
			//	b. get a free ,go to step 1
			// saved_R_S_index=0, saved_TLB_index=0  location=0
			//  c. loop through the TLB to find the page number that is used the farthest in the reference list
			//     record the index position in the reference list
			//     record the index position in the TLB table
			//     if not found, use it, return that TLB index
			//     if saved index < new index
			//        save new index
			//  return saved index
			//go to step 1
				PC = current;
				for (int i = 0; i < NUMBER_OF_FRAMES; i++)
				{
					addressIndex = 0;
					for (int j = PC+1; j < addressLength; j++)
					{
						page = getPageNumber(addressList[j]);
						if (TLB[i][PAGE_NUMBER_INDEX] == page)
						{
							if (j > addressIndex && addressIndex != 0)
							{
							}
							else 
							{
								addressIndex = j;
							}					
						}
					}
					if (location < addressIndex)
					{
						location = addressIndex;
						frame_index = i;
					}
					if (addressIndex == 0)
					{
						frame_index=i;
						break;
					}
				}
			}
		return frame_index;
	}
}

int Run(int* address_list, int length, ALGORITHM alg)
{
	int frame_index, fault_count = 0;
	int page;
	int offset;

	init_PhysicalMemory();
	for (int PC = 0; PC < length; PC++)
	{
		page = getPageNumber(address_list[PC]);
		offset = getPageOffset(address_list[PC]);

		frame_index = is_PageLoaded(page);
		if (frame_index == NOT_FOUND)
			fault_count++;

		switch (alg) {
		case FIFO:
			frame_index = swapPage_FIFO(frame_index, page);
			break;
		case OPTIMUM:
			frame_index = swapPage_Optimal(frame_index, address_list, PC, length);
			break;
		default:
			frame_index = swapPage_LRU(frame_index, page);
			break;
		}

		if (load_LogicalToPhysical(page, frame_index))
			update_TLB(page, frame_index);
            //no need to print the data
	}

	return fault_count;
}
