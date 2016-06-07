#include "FileSystem.h"
#include "DirectoryManager.h"

FileSystem::FileSystem(void)
{
}


void FileSystem::initFS()
{
	/* blockList 초기화 */
	for ( int i = 0; i < NUM_BLOCKS * BLOCK_SIZE; i++ )
		blockList[i] = ' ';

	for ( int i = 2 * BLOCK_SIZE; i < 4 * BLOCK_SIZE; i++ ) 
		blockList[i] = '0';
}

void FileSystem::loadFS( char* FS_File )
{
	initFS();
	initBlocks(); // 클래스 블럭들 포인터 초기화

	int fd = open( FS_File, O_RDONLY );

	if ( fd == -1 ) // 파일 열기 실패
	{	
		/* 블럭의 데이터 값을 초기화 함수 불러준다. */
 		superBlock.inputSuperBlockData(); // SuperBlock 값 초기화
		blockDescriptorTable.inputBlockDescriptorTableData(); // BDT 값 초기화

		for ( int i = 0; i <= 5; i++ ) // BlockBitmap 값 초기화
			blockBitmap[i] = '1';

		/* 디렉토리 초기화 */
		DirectoryManager& dm = *DirectoryManager::getInstance();
		dm.makeDefaultDirectory();

		for (int i = 0; i < 7; i++) // InodeBitmap 값 초기화
			inodeBitmap[i] = '1';
			
	}

	else // 파일 열기 성공
	{
		read( fd, blockList, BLOCK_SIZE * NUM_BLOCKS ); // blockList[409600]에 FS_File을 채워 넣어준다
		close( fd );
	}
}

void FileSystem::saveFS( char* FS_File )
{
	int fd = open( FS_File, _O_WRONLY | O_CREAT );

	write( fd, blockList, BLOCK_SIZE * NUM_BLOCKS );

	close( fd );

}

void FileSystem::readFS(int dataBlockNum, char* buffer)
{
	string str = dataBlocks[dataBlockNum - 6].getDataBlockData();

	for (int i = 0; i < str.length(); i++)
		buffer[i] = str.at(i);
	buffer[str.length()] = '\0';
}

Inode FileSystem::readFS( int inodeNum )
{
	// return 아이노드정보
	if ( checkInodeBlockNum( inodeNum ) == 4 )
		return inodeBlock[0].getInodeData( inodeNum );

	else
		return inodeBlock[1].getInodeData( inodeNum - 32 );
}

/* 수정필요 */
//void FileSystem::readFS( int blockNum, char* buffer )
//{
//	try {
//      char* tmp = getBlock(blockNum);
//      buffer = tmp;
//   }
//   catch (char* msg) {
//      cerr << "error : " << msg << endl;
//   }  
//
//   // Inode 관련
//}
//

int FileSystem::writeFS( Inode inode )
{
	int dataBlockNum = searchUnassignedBlockNum(); // 데이터블럭 번호 할당
	int inodeNum = searchUnassignedInodeNum(); // 아이노드 번호 할당

	char *buffer = new char[2];
	itoa( dataBlockNum, buffer );

	int inodeBlockNum = checkInodeBlockNum( inodeNum ); // 아이노드 번호가 32이상인지 확인

	if ( inodeBlockNum == 4 ) // 아이노드 번호가 32이하 이면 블럭번호 4에 저장
	{
		inodeBlock[0].setMode( inodeNum, inode.mode );
		inodeBlock[0].setSize( inodeNum, inode.size );
		inodeBlock[0].setTime( inodeNum, inode.time );
		inodeBlock[0].setCtime( inodeNum, inode.ctime );
		inodeBlock[0].setMtime( inodeNum, inode.mtime );
		inodeBlock[0].setLinksCount( inodeNum, inode.linksCount );
		inodeBlock[0].setBlocks( inodeNum, inode.blocks );
		inodeBlock[0].setDataBlockList( inodeNum, buffer );
	}

	else if ( inodeBlockNum == 5 ) // 이상이면 블럭번호 5에 저장
	{
		inodeBlock[1].setMode( inodeNum - 32, inode.mode );
		inodeBlock[1].setSize( inodeNum - 32, inode.size );
		inodeBlock[1].setTime( inodeNum - 32, inode.time );
		inodeBlock[1].setCtime( inodeNum - 32, inode.ctime );
		inodeBlock[1].setMtime( inodeNum - 32, inode.mtime );
		inodeBlock[1].setLinksCount( inodeNum - 32, inode.linksCount );
		inodeBlock[1].setBlocks( inodeNum - 32, inode.blocks );
		inodeBlock[1].setDataBlockList( inodeNum - 32, buffer );
	}

	blockBitmap[dataBlockNum] = '1'; // 블럭 비트맵 갱신
	inodeBitmap[inodeNum] = '1'; // 아이노드 비트맵 갱신

	blockDescriptorTable.setUnassignedBlockNum( atoi( blockDescriptorTable.getUnassignedBlockNum() ) - 1 ); // unassignedBlockNum - 1 
	blockDescriptorTable.setUnassignedInodeNum( atoi( blockDescriptorTable.getUnassignedInodeNum() ) - 1 ); // unassignedInodeNum - 1

	delete[] buffer;

	return inodeNum;
}

int FileSystem::writeFS_Dir(Inode inode)
{
	int dataBlockNum = searchUnassignedBlockNum(); // 데이터블럭 번호 할당
	int inodeNum = searchUnassignedInodeNum(); // 아이노드 번호 할당

	char *buffer = new char[2];
	itoa(dataBlockNum, buffer);

	int inodeBlockNum = checkInodeBlockNum(inodeNum); // 아이노드 번호가 32이상인지 확인

	if (inodeBlockNum == 4) // 아이노드 번호가 32이하 이면 블럭번호 4에 저장
	{
		inodeBlock[0].setMode(inodeNum, inode.mode);
		inodeBlock[0].setSize(inodeNum, inode.size);
		inodeBlock[0].setTime(inodeNum, inode.time);
		inodeBlock[0].setCtime(inodeNum, inode.ctime);
		inodeBlock[0].setMtime(inodeNum, inode.mtime);
		inodeBlock[0].setLinksCount(inodeNum, inode.linksCount);
		inodeBlock[0].setBlocks(inodeNum, inode.blocks);
		inodeBlock[0].setDataBlockList(inodeNum, buffer);
	}

	else if (inodeBlockNum == 5) // 이상이면 블럭번호 5에 저장
	{
		inodeBlock[1].setMode(inodeNum - 32, inode.mode);
		inodeBlock[1].setSize(inodeNum - 32, inode.size);
		inodeBlock[1].setTime(inodeNum - 32, inode.time);
		inodeBlock[1].setCtime(inodeNum - 32, inode.ctime);
		inodeBlock[1].setMtime(inodeNum - 32, inode.mtime);
		inodeBlock[1].setLinksCount(inodeNum - 32, inode.linksCount);
		inodeBlock[1].setBlocks(inodeNum - 32, inode.blocks);
		inodeBlock[1].setDataBlockList(inodeNum - 32, buffer);
	}
	inodeBitmap[inodeNum] = '1'; // 아이노드 비트맵 갱신
	/*
	cout << endl;
	cout << "Assigned Inode Num : " << inodeNum << endl;
	cout << "<Before Inode Bitmap>" << endl;
	displayInodeBitmap();
	
	cout << "<After Inode Bitmap>" << endl;
	displayInodeBitmap();
	cout << "<After DataBlock Bitmap>" << endl;
	displayBlockBitmap();
	cout << "---------------------" << endl;
	*/

	blockDescriptorTable.setUnassignedBlockNum(atoi(blockDescriptorTable.getUnassignedBlockNum()) - 1); // unassignedBlockNum - 1 
	blockDescriptorTable.setUnassignedInodeNum(atoi(blockDescriptorTable.getUnassignedInodeNum()) - 1); // unassignedInodeNum - 1

	delete[] buffer;

	return inodeNum;
}

void FileSystem::writeFS( int inodeNum )
{
	int blocks;
	if ( checkInodeBlockNum( inodeNum ) == 4 )
		blocks = atoi ( inodeBlock[0].getBlocks( inodeNum ) ) / 3;

	else
		blocks = atoi ( inodeBlock[1].getBlocks( inodeNum - 32 ) ) / 3;

	int* dataIndex = new int [ blocks ];

	if ( checkInodeBlockNum( inodeNum ) == 4 )
		translateCharArrToIntArr( inodeBlock[0].getDataBlockList( inodeNum ), dataIndex, blocks );

	else
		translateCharArrToIntArr( inodeBlock[1].getDataBlockList( inodeNum ), dataIndex, blocks );

	resetDataBlock( dataIndex, blocks );
	resetInodeBlock( inodeNum );

	delete[] dataIndex;
}

int FileSystem::writeFS( char* blockData )
{
	int blockNum = searchUnassignedBlockNum();
	blockBitmap[ blockNum ] = '1';

	dataBlocks[ blockNum - 6 ].setData( blockData );

	return blockNum;
}

int FileSystem::updateInode_readFile( int inodeNum, Inode inode )
{
	int inodeBlockNum = checkInodeBlockNum( inodeNum );

	if ( inodeBlockNum == 4 ) // 아이노드 번호가 32이하 이면 블럭번호 4에 저장
	{
		inodeBlock[0].setMode( inodeNum, inode.mode );
		inodeBlock[0].setSize( inodeNum, inode.size );
		inodeBlock[0].setTime( inodeNum, inode.time );
		inodeBlock[0].setCtime( inodeNum, inode.ctime );
		inodeBlock[0].setMtime( inodeNum, inode.mtime );
		inodeBlock[0].setLinksCount( inodeNum, inode.linksCount );
	}

	else if ( inodeBlockNum == 5 ) // 이상이면 블럭번호 5에 저장
	{
		inodeBlock[1].setMode( inodeNum - 32, inode.mode );
		inodeBlock[1].setSize( inodeNum - 32, inode.size );
		inodeBlock[1].setTime( inodeNum - 32, inode.time );
		inodeBlock[1].setCtime( inodeNum - 32, inode.ctime );
		inodeBlock[1].setMtime( inodeNum - 32, inode.mtime );
		inodeBlock[1].setLinksCount( inodeNum - 32, inode.linksCount );
	}

	return 0;
}

int FileSystem::updateInode_writeFile( int inodeNum, Inode inode )
{
	int inodeBlockNum = checkInodeBlockNum( inodeNum ); // 아이노드 번호가 32이상인지 확인

	if ( inodeBlockNum == 4 ) // 아이노드 번호가 32이하 이면 블럭번호 4에 저장
	{
		inodeBlock[0].setMode( inodeNum, inode.mode );
		inodeBlock[0].setSize( inodeNum, inode.size );
		inodeBlock[0].setTime( inodeNum, inode.time );
		inodeBlock[0].setCtime( inodeNum, inode.ctime );
		inodeBlock[0].setMtime( inodeNum, inode.mtime );
		inodeBlock[0].setLinksCount( inodeNum, inode.linksCount );
		inodeBlock[0].setBlocks( inodeNum, inode.blocks );
		inodeBlock[0].setDataBlockList( inodeNum, inode.dataBlockList );
	}

	else if ( inodeBlockNum == 5 ) // 이상이면 블럭번호 5에 저장
	{
		inodeBlock[1].setMode( inodeNum - 32, inode.mode );
		inodeBlock[1].setSize( inodeNum - 32, inode.size );
		inodeBlock[1].setTime( inodeNum - 32, inode.time );
		inodeBlock[1].setCtime( inodeNum - 32, inode.ctime );
		inodeBlock[1].setMtime( inodeNum - 32, inode.mtime );
		inodeBlock[1].setLinksCount( inodeNum - 32, inode.linksCount );
		inodeBlock[1].setBlocks( inodeNum - 32, inode.blocks );
		inodeBlock[1].setDataBlockList( inodeNum - 32, inode.dataBlockList );
	}

	return 0;
}

char* FileSystem::getBlockList( int blockNum )
{
	return &blockList[ blockNum * BLOCK_SIZE ];
}

void FileSystem::initBlocks() // 각 블럭 클래스랑 blockList[409600] 포인터 연결
{
	superBlock.initSuperBlock( getBlockList(0) ); // 슈퍼 블럭 초기화
	blockDescriptorTable.initBlockDescriptorTable( getBlockList(1) ); // BDT 초기화
	
	blockBitmap = getBlockList(2);
	inodeBitmap = getBlockList(3);

	inodeBlock[0].initInodeBlock( getBlockList(4) ); // 아이노드 블럭1 초기화
	inodeBlock[1].initInodeBlock( getBlockList(5) ); // 아이노드 블럭2 초기화

	for ( int i = 0; i < 94; i++ ) // 데이터블럭 초기화
		dataBlocks[i].initDataBlock( getBlockList( i + 6 ) );
}

void FileSystem::resetDataBlock( int* indexList, int blocks )
{
	for ( int i = 0; i < blocks; i++ )
	{
		blockBitmap[ indexList[i] ] = '0';
		dataBlocks[ indexList[i] - 6 ].resetData();
	}

	char* num = blockDescriptorTable.getUnassignedBlockNum();
	blockDescriptorTable.setUnassignedBlockNum( atoi(num) + blocks );
}

void FileSystem::resetInodeBlock( int inodeNum )
{
	inodeBitmap[ inodeNum ] = '0';

	if ( checkInodeBlockNum( inodeNum ) == 4 )
		inodeBlock[0].resetinodeBlock( inodeNum );

	else
		inodeBlock[1].resetinodeBlock( inodeNum - 32 );

	char* num = blockDescriptorTable.getUnassignedInodeNum();
	blockDescriptorTable.setUnassignedInodeNum( atoi(num) + 1 );
}

int FileSystem::getAssignedInodeNum()
{
	return ( 64 - atoi( blockDescriptorTable.getUnassignedInodeNum() ) );
}

void FileSystem::displayBlockBitmap()
{
	for ( int i = 0; i < 100; i++ )
	{
		cout << blockBitmap[i] ;

		if ( i != 0 && i % 10 == 0 )
			cout << endl;
	}
}

void FileSystem::displayInodeBitmap()
{
	for ( int i = 0; i < 64; i++ )
	{
		cout << inodeBitmap[i] ;

		if ( i != 0 && ( i % 10 == 0 || i == 63 ) )
			cout << endl;
	}
}

int FileSystem::searchUnassignedBlockNum()
{
	for ( int i = 0; i < NUM_BLOCKS; i++ )
	{
		if ( blockBitmap[i] == '0' )
			return i;
	}
}

int FileSystem::searchUnassignedInodeNum()
{
	for ( int i = 0; i < 64; i++ )
	{
		if ( inodeBitmap[i] == '0' )
			return i;
	}
}

int FileSystem::checkInodeBlockNum( int inodeNum )
{
	if ( inodeNum < 32 )
		return 4;

	else 
		return 5;
}

int* FileSystem::getAssignedInodeIndex()
{
	int* inodeIndexList = new int [ getAssignedInodeNum() ];

	int j = 0;
	for ( int i = 0; i < 64; i++ )
	{
		if ( inodeBitmap[i] == '1' )
			inodeIndexList[ j++ ] = i;
	}

	return inodeIndexList;

	delete[] inodeIndexList;
}


void FileSystem::togleBlockBitmap( int blockNum )
{
	if ( blockBitmap[ blockNum ] == '0' )
		blockBitmap[ blockNum ] = '1';

	else
		blockBitmap[ blockNum ] = '0';
}

void FileSystem::togleInodeBitmap( int inodeNum )
{
	if ( inodeBitmap[ inodeNum ] == '0' )
		inodeBitmap[ inodeNum ] = '1';

	else
		inodeBitmap[ inodeNum ] = '0';
}
FileSystem* FileSystem::instance = NULL;
bool FileSystem::allowDelete = false;