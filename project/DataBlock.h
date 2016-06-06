#pragma once
#include "CommonLibrary.h"

class DataBlock
{
public:
	char* data; // 파일 내용

	DataBlock(void); 
	~DataBlock(void); 

	void initDataBlock( char* blockPtr ); // ( FS_File이 있을때 동작 )
	void resetData();
	void setData( char* buffer );

	char* getDataBlockData();
	// BDT의 unassignedInodeNum 올려주기
};

