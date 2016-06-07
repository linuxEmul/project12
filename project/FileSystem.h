#pragma once

#include "SuperBlock.h"
#include "BlockDescriptorTable.h"
#include "InodeBlock.h"
#include "DataBlock.h"

class FileSystem
{
private:

	static FileSystem* instance;
	
	FileSystem(void);
public:
	static bool allowDelete;
	~FileSystem()
	{
		if (allowDelete)
		{
			if (instance)
				delete instance;
		}
	}

	static FileSystem* getInstance() {
		if (!instance) {
			instance = new FileSystem();
		}
		return instance;
	}
	char blockList[ NUM_BLOCKS * BLOCK_SIZE ]; // 409600

	/* Block Class�� */
	SuperBlock superBlock; // ����ȣ 0
	BlockDescriptorTable blockDescriptorTable; // ����ȣ 1
	char *blockBitmap; // ����ȣ 2 - 100��
	char *inodeBitmap; // ����ȣ 3 - 64��
	InodeBlock inodeBlock[2]; // ����ȣ 4 ����ȣ 5
	DataBlock dataBlocks[94]; // ����ȣ 6 ~

	/* �ʿ� ������ */
	~FileSystem(void);

	void initFS();
	void loadFS( char* FS_File ); 
	void saveFS( char* FS_File );

	/* readFS �����ε� */
	void readFS( int dataBlockNum, char* buffer );
	Inode readFS( int inodeNum );
	
	/* writeFS �����ε� */
	int writeFS( Inode inode );  // ���� Create �� ȣ��
	void writeFS( int inodeNum ); // ���� Unlink(delete) �� ȣ��
	int writeFS( char* blockData );
	int writeFS_Dir(Inode inode);

	int updateInode_readFile( int inodeNum, Inode inode ); // updateInode ���
	int updateInode_writeFile( int inodeNum, Inode inode ); // updateInode ���

	/* ��Ÿ �ʿ� �Լ��� */
	char* getBlockList( int blockNum ); // blockList �� ��ȣ�� ù��° �ּ� ��ȯ
	void initBlocks(); // �� �� Ŭ������ blockList[409600] ������ ����

	void resetDataBlock( int* indexList, int blocks ); // �Ҵ�� datablock �ʱ�ȭ
	void resetInodeBlock( int inodeNum ); // �Ҵ�� inode���� �ʱ�ȭ

	int getAssignedInodeNum(); // �Ҵ�� ���̳��� ��ȯ
	int* getAssignedInodeIndex(); // �Ҵ�� ���̳�� ��ȣ�� �迭�� ��ȯ

	void displayBlockBitmap(); // blockbitmap ���
	void displayInodeBitmap(); // inodebitmap ���

	int searchUnassignedBlockNum(); // �Ҵ�������� ����ȣ ã��
	int searchUnassignedInodeNum(); // �Ҵ�������� ���̳���ȣ ã��

	int checkInodeBlockNum( int inodeNum );

	void togleBlockBitmap( int blockNum );
	void togleInodeBitmap( int inodeNum );


	void test(){}
};