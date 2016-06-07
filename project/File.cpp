#include "FIle.h"


File::File() { }

File::~File() { }


void File::createFile(char* filename, Directory& dir)//, DirectoryManager& dm )
{
	FileSystem& fs = *FileSystem::getInstance();

	Inode inode;

	char mode[5] = "f666";

	char time[13];
	getCurrentTime(time);

	char linkCount[2] = "1";
	char blocks[2] = "1";

	char size[2] = "0"; // getSizeOfDataBlock( int idx );
						// Inode Table에 정보 써주기 ( char* mode ,int size, struct tm lastTime, struct tm creatTime, struct tm modifyTime,  int links_count, int blocks, int dataIdx) 
						// Inode Table에서는 block[]에 dataIdx 저장함

	inode.mode = mode;
	inode.size = size;

	// 파일생성시에는 모두 같으므로 매개변수에 동일한 타임 객체를 넘김
	inode.time = time;      // 파일 마지막 접근 시간 설정
	inode.ctime = time;      // 파일 생성 시간 설정
	inode.mtime = time;      // 파일이 마지막으로 수정된 시간 설정
	inode.linksCount = linkCount;
	inode.blocks = blocks;

	int inodeNo = fs.writeFS(inode);
	/*
	(FS)   BlockDescriptor Table의 미 할당된 블록 수, 미 할당된 inode 수 갱신
	inode를 writeFS를 통해 FS로 보내서 파일 생성 정보들을 FS에 저장해주고,  inodeNo를 받아온다.
	*/

	Entry fileEntry;
	strcpy( fileEntry.name,  filename );
	fileEntry.inodeNum = inodeNo;
	
	// directory.addDir( inodeNo, filename ); // 파일명과 파일InodeNo를 디렉토리에 보냄
	dir.addDirectory(fileEntry, inodeNo);
}   // createFile

int File::openFile( Entry file )//, FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	PathManager& pm = *PathManager::getInstance();

	int fd = 0;

	Inode inode = fs.readFS( file.inodeNum );
	/* (FS)   InodeBlock에서 inodeNo번째 읽어 옴 */

	if ( tm.isExistInInodeTable( file.inodeNum ) )
	{
		cout << endl << "open되어있는 파일임" << endl;
		return fd;// fd = 0 -> 이미 오픈되어있는 경우
	}

	fd = tm.fileOpenEvent( file.inodeNum , inode );
	/*
	InodeTable에 읽어온 inode정보 저장->시스템파일테이블->파일디스크립터 설정됨

	InodeBlocks에서 파일의 아이노드 정보를 읽어 아이노드 테이블의 인덱스 n에 저장
	시스템 파일 테이블의 인덱스 k에 관련 정보를 저장
	inode번호에 인덱스n을 저장하고 파일 포인터는 0으로 초기화
	파일 디스크립터 테이블의 인덱스 j 에 시스템 파일 테이블의 인덱스 k 저장
	인덱스 j 리턴
	*/

	return fd;      //파일 디스크립터 반환
} // openFile

void File::readFile(int fd, char* buffer, int size)//,TableManager& tm , FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	Inode inode = inodeE->inode;

	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	int filePointer = ((SFTElement*)tm.getElement(SFT, fd))->file_pointer;// (FT)   getFilePoint( fd ) fd가 가르키는 시스템파일테이블에서 파일포인터를 받아온다

																		  // 하나의 data에다 각 데이터블럭에 분산됬던 것들을 합침
	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	int fileSize = data.length();
	if (size == 0) // 파일내용 전체 읽을 때 사용
		size = fileSize;
	else if (fileSize < size) // 파일크기가 사용자가 보낸 size보다 큰 경우, 현재 파일에 쓰여진 내용만큼으로 사이즈 변경
		size = fileSize;

	buffer = new char[size];

	std::strcpy(buffer, data.substr(filePointer, size).c_str());
	tm.updateFilePointer(fd, filePointer + size);

	getCurrentTime(inode.time);

	// (FT)   Inode Block 의 time(파일 마지막 접근시간) 갱신
	tm.updateInode(fd, inode);
	// (FS)   inode 갱신
	fs.updateInode_readFile(inodeE->inode_number, inode);
}//readFile

 // 기존에 있던 파일데이터에 buffer를 붙여 저장
void File::writeFile(int fd, char* buffer)//,  TableManager& tm, FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	// (FT)   fd를 타고가서 inode에서 data block idx받아오기
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	Inode inode = inodeE->inode;
	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	/*
		(FS)   data block[idx]에 저장된 데이터 읽어오기
		하나의 data에다 각 데이터블럭에 분산됬던 것들을 합침
	*/
	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	string filedata = data + buffer;   // 기존데이터블럭의 내용과 새로운 내용을 합친 것

	fs.resetDataBlock(dataIdx, blocks);// writeFile에서 writeFS를 부르기 전에 BlockBitmap의 파일에 할당됬던 indxe 초기화
									   /* 참고 BlockBitmap이 1인 경우만 데이터블럭이 저장되어있는 것이므로 getDatablock은 blockbitmap이 1인지 검사 후 저장된 data return */

									   /*
									   합쳐진 data를 블럭사이즈에 맞게 쪼개서 (반복문)
									   writeFS에 넘겨서 저장해준다
									   (FS)   block idx와 blockdata를 넘겨서  writeFS에서는 해당 데이터 블럭에 blockdata를 쓴다
									   */
	int length = filedata.length();
	char fileSize[7];
	itoa(length, fileSize);

	inode.size = fileSize;

	// 파일의 데이터를 FS의 DataBlock에 써주는 부분
	char* blockData;
	int new_Blocks = 0;
	while (length != 0)
	{
		if (length > BLOCK_SIZE)
		{
			blockData = new char[BLOCK_SIZE];
			strcpy(blockData, (filedata.substr(0, BLOCK_SIZE)).c_str());

			length -= BLOCK_SIZE;
		}

		else
		{
			blockData = new char[length];
			strcpy(blockData, (filedata.substr(0, length)).c_str());

			length = 0;
		}

		int returnIdx = fs.writeFS(blockData);
		/*
		fs.writeFS 에서는
		block descriptor Table 미할당된 블록 수, -> 초기화할때랑 다시 써줄 때 갱신해줘야 함
		Block Bitmap에 idx에 해당하는 비트 1로 set -> write에서 데이터블럭에 쓴 후 바로 바로 해주는 걸로
		write에서  Datablock idx찾아서 DataBlock에 저장 후 blockBitmap 설정해줘야함 그리고 idx 반환해주는 걸로
		*/
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);

	/* (FS)   Inode Block 의 size, time(파일 마지막 접근시간), mtime(파일이 마지막으로 수정된 시간) blocks, block[] 갱신 -> FT, FS 모두 갱신 */
	char* currTime;
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	//update Inode
	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);

	delete dataIdx;
}// writeFile

// 기존에 있던 파일데이터를 buffer로 덮어 씌움
void File::writeFile(int fd, char* buffer, int size)//,  TableManager& tm, FileSystem& fs )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	// (FT)   fd를 타고가서 inode에서 data block idx받아오기
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	Inode inode = inodeE->inode;
	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	string filedata = buffer;   // 기존데이터블럭의 내용과 새로운 내용을 합친 것

	fs.resetDataBlock(dataIdx, blocks);// writeFile에서 writeFS를 부르기 전에 BlockBitmap의 파일에 할당됬던 indxe 초기화
									   /* 참고 BlockBitmap이 1인 경우만 데이터블럭이 저장되어있는 것이므로 getDatablock은 blockbitmap이 1인지 검사 후 저장된 data return */

									   /*
									   합쳐진 data를 블럭사이즈에 맞게 쪼개서 (반복문)
									   writeFS에 넘겨서 저장해준다
									   (FS)   block idx와 blockdata를 넘겨서  writeFS에서는 해당 데이터 블럭에 blockdata를 쓴다
									   */
	char fileSize[4] = { 0 };
	itoa( size, fileSize );
	inode.size = fileSize;

	char* blockData;
	int new_Blocks = 0;
	while (size != 0)
	{
		if (size > BLOCK_SIZE)
		{
			blockData = new char[BLOCK_SIZE];
			strcpy(blockData, (filedata.substr(0, BLOCK_SIZE)).c_str());

			size -= BLOCK_SIZE;
		}

		else
		{
			blockData = new char[size];
			strcpy(blockData, (filedata.substr(0, size)).c_str());

			size = 0;
		}

		int returnIdx = fs.writeFS( blockData );
		delete blockData;
		
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks, 1); // inode에 할당받은 데이터블럭 인덱스 저장

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);	// blocks( 할당받은 데이터블럭 수 ) 저장 

	/* (FS)   Inode Block 의 size, time(파일 마지막 접근시간), mtime(파일이 마지막으로 수정된 시간) blocks, block[] 갱신 -> FT, FS 모두 갱신 */
	char* currTime;
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);
}// writeFile

void File::seekFile(int fd, int offset)//, TableManager& tm )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	int filePointer;

	if (offset < 0)   // offset이 음수인 경우, 파일의 끝을 향하도록 한다
	{      //fp = getFileSize();
		Inode inode = ((InodeElement*)tm.getElement(INODET, fd))->inode;
		filePointer = atoi(inode.size) - 1;
	}
	else
		filePointer = offset;   // fp = offset; 

								// (FT)   fp 를 시스템파일테이블의 파일포인터로 갱신
	tm.updateFilePointer(fd, filePointer);
}

void File::closeFile(int fd)//, TableManager& tm )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	//inode Table에서 inode를 읽어와 inodeBlock update 해줘야 함
	Inode inode = ((InodeElement*)tm.getElement(INODET, fd))->inode;

	// (FT)  Inode Table 정보 삭제, 시스템파일테이블 정보 삭제, 파일디스크립터테이블 정보 삭제
	tm.fileCloseEvent(fd);
}

int File::unlinkFile(char* file) // file 은 absPath 형태
{
	FileSystem& fs = *FileSystem::getInstance();
	PathManager& pm = *PathManager::getInstance();
	TableManager& tm = *TableManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	Directory* dir;

	Entry* fileEntry = findFile( file );

	// 현재 시스템의 테이블에 존재한다는 것은 오픈되어있다는 것이므로 실패!
	if (tm.isExistInInodeTable( fileEntry->inodeNum ) )
		return 1;

	// 없는경우 정상적으로 파일을 unlink해줘야 함.
	// 아이노드 번호에 해당하는 아이노드 정보를 읽어옴
	// 속하는 디렉토리에서 해당 파일과 아이노드번호 삭제 --> 디렉토리 클레스에서 파일명을 통해 엔트리 제거하는 함수 필요
	vector<string>* pathFiles = pm.getAllAbsPath( file );

	*dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));
	dir->rmDirectory( fileEntry->inodeNum, dm.returnInodeNum(stringToCharArr((*pathFiles)[pathFiles->size()-2])));
	/* 원래 inode의 링크수를 따져서 링크수가 0인것만 지워야하지만, 이 과제에서는 링크수를 고려하지 않으니깐 바로 삭제할 것 */

	fs.writeFS( fileEntry->inodeNum ); // inode 번호 넘겨서 파일정보 초기화

	return 0;
}

/* 쉘 관련 */

Entry* File::findFile( char* filename,  Directory* dir ) // 상대경로 혹은 절대경로 
{
	FileSystem& fs = *FileSystem::getInstance();
	PathManager& pm = *PathManager::getInstance();
	TableManager& tm = *TableManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	char* absPath = pm.getAbsolutePath( filename );
	vector<string>* pathFiles = pm.getAllAbsPath( absPath );

	Entry* fileEntry;
	
	int count = pathFiles->size();

	// 타겟파일 제외한 절대경로를 통해 디렉토리를 찾아 아이노드 번호를 받음
	int dirInodeNo = dm.returnInodeNum( (char*)*pathFiles->at( count -2 ).c_str() );
	// 디렉토리의 아이노드 번호를 통해 디렉토리를 받아옴
	dir = dm.returnDir( dirInodeNo );

	cout << "파일이 추가될 디렉토리의 아이노드 넘버 :  "<< dirInodeNo << endl;

	fileEntry = dir->findName( filename ); // 디렉토리에서 파일엔트리를 찾음

	if (fileEntry == nullptr)	// 디렉토리에 파일엔트리가 없는 경우, 파일을 생성해 준다
	{
		vector<string> files = pm.doAnalyzeFolder( absPath );
		string filename = files[ files.size() - 1];

		createFile( (char*)filename.c_str() , *dir );
	}

	return fileEntry;
}

//file entry가 nullptr이면 파일을 생성해줘야하는 명령어 : cat, 
int File::createAndOpen( Entry* fileEntry, Directory& dir )
{
	PathManager& pm = *PathManager::getInstance();
	char* absPath = pm.getAbsolutePath( fileEntry->name );

	if (fileEntry == nullptr)	// 디렉토리에 파일엔트리가 없는 경우, 파일을 생성해 준다
	{
		vector<string> files = pm.doAnalyzeFolder( absPath );
		string filename = files[ files.size() - 1];

		createFile( (char*)filename.c_str() , dir );
	}

	return openFile( *fileEntry );
}

//file entry가 nullptr이면 예외처리가 필요한 명령어 : rm ,  chmod, copy( a는 예외처리 , b는 생성해줄것 ) , split, paste ( 매개변수 둘다 find)
int File::open( Entry* fileEntry )
{
	int fd;

	if (fileEntry == nullptr)
	{
		cout << endl << "해당 디렉토리에 파일이 없음." << endl;
		fd = -1;
		return fd;
	}
	
	return openFile( *fileEntry );
}

/*  chmod 관련   */
void File::changeFileMode(char* file, char* mode) // file 은filename
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	int fd = open( findFile(file) );

	InodeElement* targetFileInodeE = (InodeElement*)tm.getElement(INODET, fd);
	Inode inode = targetFileInodeE->inode; // file에 해당하는 inode를 얻어온다
										   // file의 inode에서 mode를 변경해준다
	inode.mode = new char[5];
	for (int i = 0; i < 5; i++)
		inode.mode[i] = mode[i];

	// update해준다--> closeFile해서 FS도 갱신해줘야함
	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(targetFileInodeE->inode_number, inode);
}

/*  cat 관련   */

// cat > a 의 경우 : 덮어쓰기
void File::overwriteCat(char* file, string data) // file은 filename만 pathX
{
	Directory dir;
	int fd = createAndOpen( findFile(file, &dir), dir  );

	writeFile(fd, (char*)data.c_str(), data.length());
}

// cat >> a 의 경우 : 이어쓰기
void File::joinCat(char* file, char* data) // file 은 filename
{
	Directory dir;
	int fd = createAndOpen( findFile(file, &dir), dir  );

	writeFile(fd, data);
}

// cat a 의 경우 :: 20줄씩 보기, 줄 기준은 endline( enter ASCII 13번 )
// file이 없는 경우 쉘에서 예외처리 해줄 것 
void File::displayCat(int fd)
{
	//int fd = openFile( file, *(TableManager::getInstance()) ); // dispalyCat 외부에서 해줄 것

	int perLineChar = 50;
	int lineNo = 20;
	int size = perLineChar * lineNo; // 한줄에 출력해줄 char수 50 , 20라인

	char* buffer = nullptr;

	if (buffer == nullptr)
		return;

	readFile(fd, buffer, size);
	for (int j = 0; j < lineNo; j++)
	{
		for (int i = 0; i < perLineChar; i++)
			cout << buffer[i];
		cout << endl;
	}
}

/*  rm 관련   */
void File::removeFile(char* file)// file은 filename
{
	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	int error = unlinkFile(pm.getAbsolutePath(file));

	if (!error) // error = 0 정상 / error = 1 오픈파일에러
		cout << "파일이 오픈되어있어 삭제할 수 없습니다." << endl;
}

/*  chmod 관련   */
void File::splitFile(char* sourceFile, char* first_target, char* second_target) // sourceFile은 file이름 first_target은 xFilenamea, second_target은 xFilenameb 
{
	// source파일의 데이터를 읽어온다
	int fd = open( findFile(sourceFile) );

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	InodeElement* sourceInodeE = (InodeElement*)tm.getElement(INODET, fd);
	int halfSize = atoi(sourceInodeE->inode.size) / 2;

	// first_target파일에 반만큼 저장
	Directory dir;
	int firstTargetFd = createAndOpen( findFile( first_target, &dir ), dir );
	char* firstFileData;
	readFile(fd, firstFileData, halfSize);
	writeFile(firstTargetFd, firstFileData, halfSize);

	// second_target에 나머지 반 저장
	int secondTargetFd = createAndOpen( findFile(second_target, &dir), dir );
	char* secondFileData;
	readFile(fd, secondFileData, halfSize);
	writeFile(secondTargetFd, secondFileData, halfSize);

}

/*  paste 관련   */
void File::pasteFile(char* firstFile, char* secondFile)
{
	int firstFileFd = open( findFile(firstFile) );
	int secondFileFd = open( findFile(secondFile) );

	char* secondFileData;
	readFile(secondFileFd, secondFileData, 0);

	writeFile(firstFileFd, secondFileData);
}

/*  file copy 관련   */
string File::readSystemFile(char* filename)
{
	ifstream is;
	is.open(filename);

	string data = "";
	while (!is.eof())
	{
		string tmpData;
		getline(is, tmpData);
		data += tmpData;
	}
	is.close();

	return data;
}

void File::copyFile(char* sourceFile, char* targetFile)
{
	string data = readSystemFile(sourceFile);//   System file read 
	Directory dir;
	int fd = createAndOpen( findFile(targetFile, &dir), dir );//-> filename으로 file create해줄 것 //-> file open할것 
	writeFile(fd, (char*)data.c_str(), data.length());//-> filewrite 해줄 것
}

/*  close 관련   */