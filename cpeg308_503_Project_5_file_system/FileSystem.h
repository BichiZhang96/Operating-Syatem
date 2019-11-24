
#pragma once

#include "disk.h"

#include <stdint.h>

const static uint32_t INVALIDE_RETURN = 2147483648;
const static uint32_t POINTERS_PER_INODE = 5;
const static uint32_t POINTERS_PER_BLOCK = 1024;
const static uint32_t INODES_PER_BLOCK = 128;
struct SuperBlock {		    // Superblock structure
	uint32_t MagicNumber;	// File system magic number
	uint32_t Blocks;	    // Number of blocks in file system
	uint32_t InodeBlocks;	// Number of blocks reserved for inodes
	uint32_t Inodes;	    // Number of inodes in file system
};

struct Inode {
	uint32_t Valid;		// Whether or not inode is valid
	uint32_t Size;		// Size of file
	uint32_t Direct[POINTERS_PER_INODE]; // Direct pointers
	uint32_t Indirect;	// Indirect pointer
};

union Block {
	SuperBlock     Super;			               // Superblock
	Inode	       Inodes[INODES_PER_BLOCK];	   // Inode block
	uint32_t       Pointers[POINTERS_PER_BLOCK];   // Pointer block
	unsigned char  Data[Disk::BLOCK_SIZE];	       // Data block
	bool           Bitmap[Disk::BLOCK_SIZE];       // Free block Information
};

class FileSystem {
public:
	const static uint32_t MAGIC_NUMBER = 0xf0f03410;
	const static uint32_t INVALID_DATA_BLOCK = 0;
	FileSystem();
	~FileSystem();
private:
	Disk *m_disk;
	int FIRST_DATA_BLOCK;
	Block super_block;
	Block *inode_blocks;       //to hold all node blocks in memory
	bool *inode_blocks_dirty;  //which inode need saving
	Block free_block;

	void setBlockFreeStatus(uint32_t block_number, bool free);
	bool getBlockFreeStatus(uint32_t block_number);
	uint32_t  getNextFreeBlock();
	Inode* getNextFreeINode(uint32_t *current_inode);
	Inode* getINode(uint32_t inode_number);
	Inode* getINode(uint32_t block_number, uint32_t inode_offset);

	// helper functions

public:
	//already implemented
	size_t write(size_t inumber, unsigned char *data, size_t length, size_t offset);
	void updateSize(uint32_t Inode, uint32_t size);
	void debug(Disk *disk);
	bool mount(Disk *disk);
	size_t create();
	bool isDiskMounted();
	bool isValidInode(uint32_t inode);

	//TODO
	bool   format(Disk *disk);
	bool   remove(size_t inumber);
	size_t stat(size_t inumber);
	void   ls();
	size_t read(size_t inumber, unsigned char *data, size_t length, size_t offset);
};
