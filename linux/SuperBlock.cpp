#include "SuperBlock.h"

SuperBlock::SuperBlock(void)
{
}

SuperBlock::~SuperBlock(void)
{
}

void SuperBlock::inputSuperBlockData()
{
	memcpy(magicNum, "20141296", magicNumLength - 1); 
	memcpy(blockSize, "4096", blockSizeLength - 1 ); 
	memcpy(blockPerGroup, "100", blockPerGroupLength - 1); 
	memcpy(inodesNum, "64", inodesNumLength - 1 ); 
	memcpy(inodeSize, "128", inodeSizeLength - 1 ); 
	memcpy(finishTime, "201606040900", finishTimeLength - 1 ); 
	memcpy(firstDataBlockNum, "6", firstDataBlockNumLength - 1 ); 
	memcpy(rootInodeNum, "0", rootInodeNumLength - 1 ); 
}

void SuperBlock::initSuperBlock( char* blockPtr )
{
	magicNum = blockPtr;
	blockPtr += magicNumLength - 1 ;

	blockSize = blockPtr;
	blockPtr += blockSizeLength - 1;

	blockPerGroup = blockPtr;
	blockPtr += blockPerGroupLength - 1;

	inodesNum = blockPtr;
	blockPtr += inodesNumLength - 1;

	inodeSize = blockPtr;
	blockPtr += inodeSizeLength - 1;

	finishTime = blockPtr; // �ð� : 2016�� 6�� 4�� 9�� 00��
	blockPtr += finishTimeLength - 1;

	firstDataBlockNum = blockPtr;
	blockPtr += firstDataBlockNumLength - 1;
	
	rootInodeNum = blockPtr;
}

char* SuperBlock::getMagicNum() 
{
	return getData( magicNum, magicNumLength );
}

char* SuperBlock::getBlockSize()
{
	return getData( blockSize, blockSizeLength );
}

char* SuperBlock::getBlockPerGroup()
{
	return getData( blockPerGroup, blockPerGroupLength );
}

char* SuperBlock::getInodesNum()
{
	return getData( inodesNum, inodesNumLength );
}

char* SuperBlock::getInodeSize()
{
	return getData( inodeSize, inodeSizeLength );
}

char* SuperBlock::getFinishTime()
{
	return getData( finishTime, finishTimeLength );
}

char* SuperBlock::getFirstDataBlockNum()
{
	return getData( firstDataBlockNum, firstDataBlockNumLength );
}

char* SuperBlock::getRootInodeNum()
{
	return getData( rootInodeNum, rootInodeNumLength );
}