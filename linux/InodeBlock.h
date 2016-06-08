#pragma once
#include "CommonLibrary.h"

typedef struct inode // 128����Ʈ
{
	char* mode; 
	char* size; 
	char* time; 
	char* ctime; 
	char* mtime; 
	char* linksCount; 
	char* blocks;
	char* dataBlockList; 

} Inode;

class InodeBlock
{
private:
	Inode inodeBlocks[32];

public:
	InodeBlock(void);
	~InodeBlock(void); // delete

	// ( FS_File�� ������ ���� )
	void initInodeBlock( char* blockPtr ); // ( FS_File�� ������ ���� )
	char* initInode( char* blockPtr, int index );

	Inode getInodeData( int inode ); // dataBlockList�� ù��° �ε��� �Ѱ��ֱ�
	// [ Func1(���� create �ÿ�) ] file�ʿ��� Inode ����ü�� ��ȯ�� �ָ� �˸��� �迭 inode�� �־��ְ� �־��� ���̳���ȣ�� ��ȯ�Ѵ�.
	// BDT�� unassignedInodeNum�� �÷��ش�.

	char* getMode( int inodeNum );
	char* getSize( int inodeNum );
	char* getTime( int inodeNum );
	char* getCtime( int inodeNum );
	char* getMtime( int inodeNum );
	char* getLinksCount( int inodeNum );
	char* getBlocks( int inodeNum );
	char* getDataBlockList( int inodeNum );

	void setMode( int inodeNum, char* mode );
	void setSize( int inodeNum, char* size );
	void setTime( int inodeNum, char* time );
	void setCtime( int inodeNum, char* ctime );
	void setMtime( int inodeNum, char* mtime );
	void setLinksCount( int inodeNum, char* linksCount );
	void setBlocks( int inodeNum, char* blocks );
	void setDataBlockList( int inodeNum, char* dataBlockList );

	void resetinodeBlock( int inodeNum );
};

