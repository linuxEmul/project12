#pragma once
#include "CommonLibrary.h"

class DataBlock
{
public:
	char* data; // ���� ����

	DataBlock(void); 
	~DataBlock(void); 

	void initDataBlock( char* blockPtr ); // ( FS_File�� ������ ���� )
	void resetData();
	void setData( char* buffer );

	char* getDataBlockData();
	// BDT�� unassignedInodeNum �÷��ֱ�
};

