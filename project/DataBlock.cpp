#include "DataBlock.h"


DataBlock::DataBlock(void)
{
}


DataBlock::~DataBlock(void)
{
}

void DataBlock::initDataBlock( char* blockPtr )
{
	data = blockPtr;
}

void DataBlock::resetData()
{
	for ( int i = 0; i < BLOCK_SIZE; i++ )
		data[i] = ' ' ;
}

char* DataBlock::getDataBlockData()
{
	return getData( data, BLOCK_SIZE );
}

void DataBlock::setData( char* buffer )
{
	strncpy( data, buffer, BLOCK_SIZE ); 
}