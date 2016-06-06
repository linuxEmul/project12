#include "InodeBlock.h"

InodeBlock::InodeBlock(void)
{
}


InodeBlock::~InodeBlock(void)
{
}

void InodeBlock::initInodeBlock( char* blockPtr )
{
	for ( int index = 0; index < 32; index++ )
	{
		blockPtr = initInode( blockPtr, index );
	}
}

char* InodeBlock::initInode( char* blockPtr, int index )
{
	inodeBlocks[index].mode = blockPtr;
	blockPtr += modeLength - 1;

	inodeBlocks[index].size = blockPtr;
	blockPtr += sizeLength - 1;

	inodeBlocks[index].time = blockPtr;
	blockPtr += timeLength - 1;

	inodeBlocks[index].ctime = blockPtr;
	blockPtr += ctimeLength - 1;

	inodeBlocks[index].mtime = blockPtr;
	blockPtr += mtimeLength - 1;

	inodeBlocks[index].linksCount = blockPtr;
	blockPtr += linksCountLength - 1;

	inodeBlocks[index].blocks = blockPtr;
	blockPtr += blocksLength - 1;

	inodeBlocks[index].dataBlockList = blockPtr;
	blockPtr += dataBlockList - 1;

	return blockPtr;
}

Inode InodeBlock::getInodeData( int inode )
{
	return inodeBlocks[inode];
}

char* InodeBlock::getMode( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].mode, modeLength );
}

char* InodeBlock::getSize( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].size, sizeLength );
}

char* InodeBlock::getTime( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].time, timeLength );
}

char* InodeBlock::getCtime( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].ctime, ctimeLength );
}

char* InodeBlock::getMtime( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].mtime, mtimeLength );
}

char* InodeBlock::getLinksCount( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].linksCount, linksCountLength );
}

char* InodeBlock::getBlocks( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].blocks, blocksLength );
}

char* InodeBlock::getDataBlockList( int inodeNum )
{
	return getData( inodeBlocks[inodeNum].dataBlockList, dataBlockList );
}

void InodeBlock::setMode( int inodeNum, char* mode )
{
	strncpy( inodeBlocks[inodeNum].mode, mode, modeLength ); 
}

void InodeBlock::setSize( int inodeNum, char* size )
{
	strncpy( inodeBlocks[inodeNum].size, size, sizeLength ); 
}

void InodeBlock::setTime( int inodeNum, char* time )
{
	strncpy( inodeBlocks[inodeNum].time, time, timeLength ); 
}

void InodeBlock::setCtime( int inodeNum, char* ctime )
{
	strncpy( inodeBlocks[inodeNum].ctime, ctime, ctimeLength ); 
}

void InodeBlock::setMtime( int inodeNum, char* mtime )
{
	strncpy( inodeBlocks[inodeNum].mtime, mtime, mtimeLength ); 
}

void InodeBlock::setLinksCount( int inodeNum, char* linksCount )
{
	strncpy( inodeBlocks[inodeNum].linksCount, linksCount, linksCountLength ); 
}

void InodeBlock::setBlocks( int inodeNum, char* blocks )
{
	strncpy( inodeBlocks[inodeNum].blocks, blocks, blocksLength ); 
}

void InodeBlock::setDataBlockList( int inodeNum, char* dataBlockList )
{
	int num = atoi( inodeBlocks[inodeNum].blocks );

	for ( int i = 0; i < 72; i++ )
		inodeBlocks[inodeNum].dataBlockList[i] = ' ';

	strncpy( inodeBlocks[inodeNum].dataBlockList, dataBlockList, 72);
}

void InodeBlock::resetinodeBlock( int inodeNum )
{
	for ( int i = 0; i < modeLength - 1 ; i++ )
		inodeBlocks[inodeNum].mode[i] = ' ' ;

	for ( int i = 0; i < sizeLength - 1 ; i++ )
		inodeBlocks[inodeNum].size[i] = ' ' ;

	for ( int i = 0; i < timeLength - 1 ; i++ )
		inodeBlocks[inodeNum].time[i] = ' ' ;

	for ( int i = 0; i < ctimeLength - 1 ; i++ )
		inodeBlocks[inodeNum].ctime[i] = ' ' ;

	for ( int i = 0; i < mtimeLength - 1 ; i++ )
		inodeBlocks[inodeNum].mtime[i] = ' ' ;

	for ( int i = 0; i < linksCountLength - 1 ; i++ )
		inodeBlocks[inodeNum].linksCount[i] = ' ' ;

	for ( int i = 0; i < blocksLength - 1 ; i++ )
		inodeBlocks[inodeNum].blocks[i] = ' ' ;

	for ( int i = 0; i < dataBlockList; i++ )
		inodeBlocks[inodeNum].dataBlockList[i] = ' ' ;
}
