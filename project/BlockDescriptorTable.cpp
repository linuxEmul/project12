#include "BlockDescriptorTable.h"


BlockDescriptorTable::BlockDescriptorTable(void)
{
}


BlockDescriptorTable::~BlockDescriptorTable(void)
{
}

void BlockDescriptorTable::inputBlockDescriptorTableData()
{
	strncpy( blockBitmapBlockNum, "2", blockBitmapBlockNumLength - 1); 
	strncpy( inodeBitmapBlockNum, "3", inodeBitmapBlockNumLength - 1 ); 
	strncpy( firstInodeTableBlockNum, "4", firstInodeTableBlockNumLength - 1); 
	strncpy( unassignedBlockNum, "95", unassignedBlockNumLength - 1 ); 
	strncpy( unassignedInodeNum, "57", unassignedInodeNumLength - 1 ); 
}

void BlockDescriptorTable::initBlockDescriptorTable( char* blockPtr )
{
	blockBitmapBlockNum = blockPtr;
	blockPtr += blockBitmapBlockNumLength - 1;

	inodeBitmapBlockNum = blockPtr;
	blockPtr += inodeBitmapBlockNumLength - 1;

	firstInodeTableBlockNum = blockPtr;
	blockPtr += firstInodeTableBlockNumLength - 1;

	unassignedBlockNum = blockPtr;
	blockPtr += unassignedBlockNumLength - 1;

	unassignedInodeNum = blockPtr;
}

char* BlockDescriptorTable::getBlockBitmapBlockNum()
{
	return getData( blockBitmapBlockNum, blockBitmapBlockNumLength );
}

char* BlockDescriptorTable::getInodeBitmapBlockNum()
{
	return getData( inodeBitmapBlockNum, inodeBitmapBlockNumLength );
}

char* BlockDescriptorTable::getFirstInodeTableBlockNum()
{
	return getData( firstInodeTableBlockNum, firstInodeTableBlockNumLength );
}

char* BlockDescriptorTable::getUnassignedBlockNum()
{
	return getData( unassignedBlockNum, unassignedBlockNumLength );
}

char* BlockDescriptorTable::getUnassignedInodeNum()
{
	return getData( unassignedInodeNum, unassignedInodeNumLength );
}

void BlockDescriptorTable::setUnassignedInodeNum( int num )
{
	char *buffer = new char[unassignedInodeNumLength - 1];
	itoa( num, buffer );
	strncpy( unassignedInodeNum, buffer, sizeof( *buffer ) + 1 ); 
}

void BlockDescriptorTable::setUnassignedBlockNum( int num )
{
	char *buffer = new char[unassignedBlockNumLength - 1];
	itoa( num, buffer );
	strncpy( unassignedBlockNum, buffer, sizeof( *buffer ) + 1 );
}
 