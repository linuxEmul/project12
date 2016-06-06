#pragma once
#include "CommonLibrary.h"

class BlockDescriptorTable
{
private:
	char* blockBitmapBlockNum;
	char* inodeBitmapBlockNum;
	char* firstInodeTableBlockNum;
	char* unassignedBlockNum;
	char* unassignedInodeNum;

public:
	BlockDescriptorTable(void);
	~BlockDescriptorTable(void);

	void inputBlockDescriptorTableData(); // ( FS_File이 없을때 동작 )
	void initBlockDescriptorTable( char* blockPtr ); // ( FS_File이 있을때 동작 )

	char* getBlockBitmapBlockNum();
	char* getInodeBitmapBlockNum();
	char* getFirstInodeTableBlockNum();
	char* getUnassignedBlockNum();
	char* getUnassignedInodeNum();

	void setUnassignedBlockNum( int num );
	void setUnassignedInodeNum( int num );
};

