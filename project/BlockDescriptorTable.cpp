#include "BlockDescriptorTable.h"


BlockDescriptorTable::BlockDescriptorTable(void)
{
}


BlockDescriptorTable::~BlockDescriptorTable(void)
{
}

void BlockDescriptorTable::inputBlockDescriptorTableData()
{
	memcpy( blockBitmapBlockNum, "2", blockBitmapBlockNumLength - 1); 
	memcpy( inodeBitmapBlockNum, "3", inodeBitmapBlockNumLength - 1 ); 
	memcpy( firstInodeTableBlockNum, "4", firstInodeTableBlockNumLength - 1); 
	memcpy( unassignedBlockNum, "95", unassignedBlockNumLength - 1 ); 
	memcpy( unassignedInodeNum, "64", unassignedInodeNumLength - 1 ); 
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
	memcpy( unassignedInodeNum, buffer, sizeof( *buffer ) + 1 ); 

	//delete[] buffer;
}

void BlockDescriptorTable::setUnassignedBlockNum( int num )
{
	char *buffer = new char[unassignedBlockNumLength - 1];
	itoa( num, buffer );
	memcpy( unassignedBlockNum, buffer, sizeof( *buffer ) + 1 );

	//delete[] buffer;
}
 