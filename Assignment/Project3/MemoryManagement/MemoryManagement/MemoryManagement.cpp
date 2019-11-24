// MemoryManagement.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Universit of Bridgeport
// CPEG 308/503 F2019 Project #3

#include "pch.h"
#include <iostream>

#define NOT_FOUND	     -1
#define EMPTY		     -1
#define NUMBER_OF_FRAMES 256      //for physical memory
#define NUMBER_OF_PAGES  256      //for logical memory

union Address_Type {
	int  iNum;
	unsigned char strNum[4];
};

FILE *vm_file_Ptr;
errno_t err;

void init_PhysicalMemory();
int read_Address(int* buffer, int length);
unsigned char* physical_memory[NUMBER_OF_FRAMES];
int page_table[NUMBER_OF_FRAMES];
bool load_LogicalToPhysical(int frame_index, int page_number);
int is_PageLoaded(int page_numbr);
void update_PageTable(int page_table_index, int page_number);
void printData(unsigned char opcode);
int getPageNumber(int address_param);
int getPageOffset(int address_param);
unsigned char fetchData(int frame_index, int offset);

int main()
{
	int previous_page = -1, page_jump=0, fault_count = 0, length;
	
	int addressPtr[128];
	unsigned char *tmpPtr = NULL;

	init_PhysicalMemory();
	length = read_Address(&addressPtr[0], 128);

	int frame_index;
	int page;
	int offset;
	unsigned char data;

	for (int PC = 0; PC < length; PC++)
	{
		page = getPageNumber(addressPtr[PC]);
		offset = getPageOffset(addressPtr[PC]);
		if (previous_page != page)
		{ 
			previous_page = page;
			page_jump++;
		}

		//START of TODO

		/* your code goes here */
		frame_index = is_PageLoaded(page);
		if (frame_index == NOT_FOUND)
		{
			frame_index = page;
			load_LogicalToPhysical(frame_index, page);
			update_PageTable(previous_page,page);
			fault_count++;
		}
		data = fetchData(frame_index, offset);

		//END of TODO

		printData(data);
	}

	std::cout << "\n# of Page Jump: " << page_jump << "\n";
	std::cout << "# of Page Fault: " << fault_count << "\n";
	return 0;
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
	//Address_Type address;     //Note: uncomment this if you pick this method to extract the page number
	page = (address_param & 0xffff) >> 8;
	//TODO

	//address.iNum = address_param;
	//page = address.strNum[1];
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
	//Address_Type address;   //Note: uncomment this if you pick this method to extract the offset
	offset = address_param & 0xff;
	//TODO: your code goes here

	return offset;
}

//This function determine if a frame number/index exist in the page_table array
//If it exist it returns it
//if not it returns NOT_FOUND
int is_PageLoaded(int page_numbr)
{
	int frame_index = NOT_FOUND;

	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
	{
		if (page_table[i] == page_numbr)
		{
			frame_index = page_numbr;
			break;
		}
	}

	//TODO: your code goes here

	return frame_index;
}

//This function reads 1 byte from physical memory and return its
unsigned char fetchData(int frame_index, int offset)
{
	unsigned char data;


	data = *(physical_memory[frame_index]+offset);
	//TODO: your code goes here
	
	return data;
}

/* YOU MAY NOT NEED TO MODIFY ANYTHING BELLOW THIS LINE*/

void printData(unsigned char opcode)
{
	std::cout << opcode;
}

void init_PhysicalMemory()
{
	for (int i = 0; i < NUMBER_OF_FRAMES; i++)
	{
		physical_memory[i] = NULL;
		page_table[i] = EMPTY;
	}
}

//This function loads data the specify page from logical memory (disk) and copy it to physical memory
//at the specify frame index.
bool load_LogicalToPhysical(int frame_index, int page_number)
{
	if (page_number < 0 || page_number>(NUMBER_OF_PAGES-1))
		return NULL;
	if (frame_index < 0 || frame_index>(NUMBER_OF_FRAMES - 1))
		return NULL;

	unsigned char* Ptr = (unsigned char*)malloc(NUMBER_OF_PAGES);

	err = fopen_s(&vm_file_Ptr, "..\\BACKING_STORE.bin", "rb");
	if (vm_file_Ptr != NULL)
	{
		int flag = fseek(vm_file_Ptr, page_number * NUMBER_OF_PAGES, 0);
		flag = fread(Ptr, 1, NUMBER_OF_PAGES, vm_file_Ptr);
		fclose(vm_file_Ptr);
		physical_memory[frame_index] = Ptr;
		return true;
	}

	return false;
}

void update_PageTable(int page_table_index, int page_number)
{
	if (page_number < 0 || page_number>(NUMBER_OF_PAGES - 1))
		return;
	if (page_table_index < 0 || page_table_index>(NUMBER_OF_FRAMES - 1))
		return;

	page_table[page_table_index] = page_number;
}

int read_Address(int *buffer, int length)
{
	int i, j, ret, count = 0;

	memset(buffer, 0, length);
	err = fopen_s(&vm_file_Ptr, "..\\addresses.txt", "r");
	if (vm_file_Ptr != NULL)
	{
		for (j = 0; j < length; j++)
		{
			ret = fscanf_s(vm_file_Ptr, "%d, ", &i);
			if (ret > 0)
			{
				buffer[j] = i;
				count++;
			}
			else break;
		}
		fclose(vm_file_Ptr);
	}
	return count;
}
