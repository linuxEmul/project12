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
	
	static FileSystem* getInstance() {
		if (!instance) {
			instance = new FileSystem();
		}
		return instance;
	}
	char blockList[ NUM_BLOCKS * BLOCK_SIZE ]; // 409600

	/* Block Class들 */
	SuperBlock superBlock; // 블럭번호 0
	BlockDescriptorTable blockDescriptorTable; // 블럭번호 1
	char *blockBitmap; // 블럭번호 2 - 100개
	char *inodeBitmap; // 블럭번호 3 - 64개
	InodeBlock inodeBlock[2]; // 블럭번호 4 블럭번호 5
	DataBlock dataBlocks[94]; // 블럭번호 6 ~

	/* 필요 변수들 */
	~FileSystem(void);

	void initFS();
	void loadFS( char* FS_File ); 
	void saveFS( char* FS_File );

	/* readFS 오버로딩 */
	void readFS( int dataBlockNum, char* buffer );
	Inode readFS( int inodeNum );
	
	/* writeFS 오버로딩 */
	int writeFS( Inode inode );  // 파일 Create 시 호출
	void writeFS( int inodeNum ); // 파일 Unlink(delete) 시 호출
	int writeFS( char* blockData );

	int updateInode_readFile( int inodeNum, Inode inode ); // updateInode 기능
	int updateInode_writeFile( int inodeNum, Inode inode ); // updateInode 기능

	/* 기타 필요 함수들 */
	char* getBlockList( int blockNum ); // blockList 블럭 번호의 첫번째 주소 반환
	void initBlocks(); // 각 블럭 클래스랑 blockList[409600] 포인터 연결

	void resetDataBlock( int* indexList, int blocks ); // 할당된 datablock 초기화
	void resetInodeBlock( int inodeNum ); // 할당된 inode정보 초기화

	int getAssignedInodeNum(); // 할당된 아이노드수 반환
	int* getAssignedInodeIndex(); // 할당된 아이노드 번호들 배열로 반환

	void displayBlockBitmap(); // blockbitmap 출력
	void displayInodeBitmap(); // inodebitmap 출력

	int searchUnassignedBlockNum(); // 할당되지않은 블럭번호 찾기
	int searchUnassignedInodeNum(); // 할당되지않은 아이노드번호 찾기

	int checkInodeBlockNum( int inodeNum );

	void togleBlockBitmap( int blockNum );
	void togleInodeBitmap( int inodeNum );

};