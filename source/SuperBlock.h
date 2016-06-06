#pragma once
#include "CommonLibrary.h"

class SuperBlock
{
private:
	char* magicNum;
	char* blockSize;
	char* blockPerGroup;
	char* inodesNum;
	char* inodeSize;
	char* finishTime;
	char* firstDataBlockNum;
	char* rootInodeNum;

public:
	SuperBlock(void);
	~SuperBlock(void); 

	void inputSuperBlockData();// ( FS_File이 없을때 동작 ) 포인터 초기화 된것에 데이터 쓰기
	void initSuperBlock( char* blockPtr ); // ( FS_File이 있을때 동작 )

	char* getMagicNum();
	char* getBlockSize();
	char* getBlockPerGroup();
	char* getInodesNum();
	char* getInodeSize();
	char* getFinishTime();
	char* getFirstDataBlockNum();
	char* getRootInodeNum();

};
