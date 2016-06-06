#pragma once
#include "CommonLibrary.h"

typedef struct inode // 128바이트
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

	// ( FS_File이 없을때 동작 )
	void initInodeBlock( char* blockPtr ); // ( FS_File이 있을때 동작 )
	char* initInode( char* blockPtr, int index );

	Inode getInodeData( int inode ); // dataBlockList의 첫번째 인덱스 넘겨주기
	// [ Func1(파일 create 시에) ] file쪽에서 Inode 구조체를 반환해 주면 알맞은 배열 inode에 넣어주고 넣어준 아이노드번호를 반환한다.
	// BDT에 unassignedInodeNum를 올려준다.

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

