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

	void inputSuperBlockData();// ( FS_File�� ������ ���� ) ������ �ʱ�ȭ �ȰͿ� ������ ����
	void initSuperBlock( char* blockPtr ); // ( FS_File�� ������ ���� )

	char* getMagicNum();
	char* getBlockSize();
	char* getBlockPerGroup();
	char* getInodesNum();
	char* getInodeSize();
	char* getFinishTime();
	char* getFirstDataBlockNum();
	char* getRootInodeNum();

};
