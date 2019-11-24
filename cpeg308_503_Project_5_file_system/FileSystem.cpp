#include "pch.h"
#include "FileSystem.h"

#include <stdio.h>

struct Inode;

FileSystem::FileSystem()
{
	super_block.Super.Blocks = 0;
	memset(&super_block, 0, Disk::BLOCK_SIZE);
	memset(&free_block, 1, Disk::BLOCK_SIZE);   //default to free
	FIRST_DATA_BLOCK = 2;
	inode_blocks = NULL;
	inode_blocks_dirty = NULL;
	m_disk = NULL;
}

FileSystem::~FileSystem()
{
	//save all durty inodes
	int total_inode_blocks = super_block.Super.InodeBlocks;
	uint32_t offset;

	if (inode_blocks_dirty != NULL)
	{
		for (int i = 0; i < total_inode_blocks; i++)
		{
			if (inode_blocks_dirty[i])
			{
				m_disk->write(i + 1, inode_blocks[i].Data);
			}
		}
		delete inode_blocks_dirty;
	}

	if (inode_blocks != NULL)
		delete inode_blocks;
}

bool FileSystem::isDiskMounted()
{
	return((m_disk != NULL) && (m_disk->mounted()));
}

bool FileSystem::isValidInode(uint32_t inode)
{
    return isDiskMounted() && inode < super_block.Super.InodeBlocks;
}

void FileSystem::setBlockFreeStatus(uint32_t block_number, bool free)
{
	if (block_number >= FIRST_DATA_BLOCK && block_number < m_disk->size())
		free_block.Bitmap[block_number]=free;
}

bool FileSystem::getBlockFreeStatus(uint32_t block_number)
{
	if(block_number>= FIRST_DATA_BLOCK && block_number<m_disk->size())
		return free_block.Bitmap[block_number];
	else return false;
}

//returns next free data block
uint32_t FileSystem::getNextFreeBlock()
{
	int block_number = INVALID_DATA_BLOCK, length = m_disk->size();
	for (block_number = FIRST_DATA_BLOCK; block_number < length; block_number++)
		if (getBlockFreeStatus(block_number))
			return block_number;
	return block_number;
}

Inode* FileSystem::getINode(uint32_t inode_number){
	uint32_t block_number = inode_number / INODES_PER_BLOCK;
	uint32_t inode_offset = inode_number % INODES_PER_BLOCK;
	return getINode(block_number, inode_offset);
}

Inode* FileSystem::getINode(uint32_t block_number, uint32_t inode_offset) {
	return &inode_blocks[block_number].Inodes[inode_offset];
}

Inode* FileSystem::getNextFreeINode(uint32_t *current_inode)
{
	Inode *pInode = NULL;
	int i, j;
	int total_inode_blocks = super_block.Super.InodeBlocks;
	*current_inode  = INVALIDE_RETURN;
	for (i = 0; i < total_inode_blocks; i++)
	{
		for (j = 0; j < INODES_PER_BLOCK; j++)
		{
			pInode = getINode(i, j);
			if (pInode->Valid == 0)
			{
				*current_inode = (i*INODES_PER_BLOCK) + j;
				return pInode;
			}
		}
	}
	return NULL;
}

void FileSystem::debug(Disk *disk) {
	Block block;
	Inode *pInode = NULL;

	// Read Superblock
	disk->read(0, block.Data);

	printf("SuperBlock:\n");
	printf("    %u blocks\n", block.Super.Blocks);
	printf("    %u inode blocks\n", block.Super.InodeBlocks);
	printf("    %u inodes\n", block.Super.Inodes);
}

// Format file system ----------------------------------------------------------

bool FileSystem::format(Disk *disk) {
	if (disk->mounted())
	{
		printf("Unount the disk first.\n");
		return false;
	}

	Block null_block;
	memset(&null_block, 0, Disk::BLOCK_SIZE);
	
	//TODO
	// initialize superblock
	SuperBlock super_block;

	// Write superblock
	super_block.Blocks = 200;
	super_block.InodeBlocks = 20;
	super_block.Inodes = 2560;

	// Clear all other blocks
	for (int i = 1; i < 200; i++) {
		disk->write(i, null_block.Data);
	}
	return true;
}

// Mount file system -----------------------------------------------------------

bool FileSystem::mount(Disk *disk) {
	// Read superblock
	disk->read(0, super_block.Data);

	// Set device and mount
	disk->mount();
    m_disk = disk;

	FIRST_DATA_BLOCK = super_block.Super.InodeBlocks + 1;

	inode_blocks = new Block[super_block.Super.InodeBlocks];

	//create a durty block list and set them to false
	inode_blocks_dirty = new bool[super_block.Super.InodeBlocks];
	memset(inode_blocks_dirty, 0, super_block.Super.InodeBlocks);

	// Copy metadata
	// Allocate free block bitmap
	// Read all Inode blocks and hold them in memory
	// build the free block list
	int i, j, k;
	int total_inodes = super_block.Super.InodeBlocks;
	for (i = 0; i < total_inodes; i++)
	{
		//load inode into emory
		disk->read(i+1, inode_blocks[i].Data);
		for (j = 0; j < INODES_PER_BLOCK; j++)
			if (inode_blocks[i].Inodes[j].Valid > 0)
			{
				for (k = 0; k < POINTERS_PER_INODE; k++)
				{
					//create bitmap to indicate free data blocks
					if (inode_blocks[i].Inodes->Direct[k] > 0);
						setBlockFreeStatus(inode_blocks[i].Inodes[j].Direct[k], false);
				}
			}
	}
	return true;
}

// Create inode ----------------------------------------------------------------

size_t FileSystem::create() {
	// Locate free inode in inode table
	Inode *pInode = NULL;
	uint32_t inumber = INVALIDE_RETURN;
	pInode = getNextFreeINode(&inumber);
	// Record inode if found
	if (pInode!=NULL)
	{
		pInode->Valid = 1;
		inode_blocks_dirty[int(inumber/INODES_PER_BLOCK)] = true;
	}
	return inumber;
}

// Remove inode ----------------------------------------------------------------

bool FileSystem::remove(size_t inumber) {
	// Load inode information
	Inode *pInode = getINode(inumber);
	// Free direct blocks
	// Clear inode in inode table
	for (int i = 0; i < POINTERS_PER_INODE; i++) {
		setBlockFreeStatus(pInode->Direct[i], true);
		pInode->Size = 0;
		pInode->Direct[i] = 0;
		pInode->Valid = 0;
	}
	// Free indirect blocks
	return true;
}

// Inode stat ------------------------------------------------------------------

size_t FileSystem::stat(size_t inumber) {
	// Load inode information
	Inode *pInode = getINode(inumber);
	int node_size = pInode->Size;
	return node_size;
}

void FileSystem::ls() {
	// load master block
	// loop through inode information
	uint32_t inode_number=0, size = 0;

	//loop through all inodes
	for (inode_number = 0; inode_number < 20; inode_number++) {
		Inode *pInode = getINode(inode_number);
		size = pInode->Size;
		printf("iNode %u Contains a file of size %u\n", inode_number, size);
	}
}

// Read from inode -------------------------------------------------------------
// returns the number of bytes read
size_t FileSystem::read(size_t inumber, unsigned char *data, size_t length, size_t offset) {
	// calculate the direct_pointer_index based on the offset
	int total_read = 0;
	int direct_pointer_index = int(offset / Disk::BLOCK_SIZE);

	// get the data block number
	Inode *pInode = getINode(inumber);
	int data_block_number = pInode->Direct[direct_pointer_index];

	// read the data block
	if (data_block_number >0 && direct_pointer_index < POINTERS_PER_INODE){
		total_read = m_disk->read(data_block_number, data);
		setBlockFreeStatus(data_block_number, false);
	}
	return total_read;
}

// Write to inode --------------------------------------------------------------
// Limit file size to only 5 data blocks (POINTERS_PER_INODE)
size_t FileSystem::write(size_t inumber, unsigned char *data, size_t length, size_t offset) {
	int total_write = 0;
	int direct_pointer_index = int(offset / Disk::BLOCK_SIZE);
	int data_block_number = getNextFreeBlock();
	if (data_block_number > 0 && direct_pointer_index < POINTERS_PER_INODE) {
		Inode *pInode = getINode(inumber);
		pInode->Direct[direct_pointer_index] = data_block_number;
		total_write = m_disk->write(data_block_number, data);
		setBlockFreeStatus(data_block_number, false);
	}
	return total_write;
}

void FileSystem::updateSize(uint32_t node_number, uint32_t size)
{
	Inode *pInode = getINode(node_number);
	pInode->Size = size;
}