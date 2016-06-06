#include "FIle.h"


File::File() { }

File::~File() { }


void File::createFile(char* file)//, DirectoryManager& dm )
{
	DirectoryManager* d = DirectoryManager::getInstance();
	DirectoryManager& dm = *d;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	Inode inode;

	char mode[5] = "f666";

	char time[13];
	getCurrentTime(time);

	char linkCount[2] = "1";
	char blocks[2] = "1";

	/* ( 참고 ) 파일 생성 시 데이터 블럭의 갯수는 1 ( 빈 파일을 생성하므로 ), 데이터 블럭의 인덱스는 FileSystem에서 할당해줌 */
	// datablock[dataIdx] 에 파일명 쓰기 setDatablock( int idx, char* data ) --> ( dataIdx, file );
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

	vector<string>* pathFiles = pm.getAllAbsPath(file);
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));

	Entry fileEntry;
	fileEntry.inodeNum = inodeNo;

	vector<string> files;
	pm.doAnalyzeFolder(file, files);// 파일을 차례로 오픈해줌

	strcpy(fileEntry.name, files[files.size() - 1].c_str());

	// directory.addDir( inodeNo, filename ); // 파일명과 파일InodeNo를 디렉토리에 보냄
	dir.addDirectory(fileEntry, inodeNo);
}   // createFile

int File::openFile(char* file, TableManager& tm)//, FileSystem& fs )
{
	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	int fd = 0;

	// InodeTable에서 현재 디렉토리의 데이터 블럭 idx 받아옴
	vector<string>* pathFiles = pm.getAllAbsPath(file);// 파일을 차례로 오픈해줌
	DirectoryManager dm = DirectoryManager::getTmpInstance();
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));

	// 디렉터리의 데이터블럭 읽어옴
	Entry* dirFileEntry = dir.findName(stringToCharArr((*pathFiles)[pathFiles->size() - 1]));

	if (dirFileEntry == nullptr)
	{
		cout << endl << "해당 디렉토리에 파일이 없음." << endl;
		fd = -1;
		return fd;
	}

	int inodeNo = dirFileEntry->inodeNum;
	Inode inode = fs.readFS(inodeNo);
	/* (FS)   InodeBlock에서 inodeNo번째 읽어 옴 */

	if (tm.isExistInInodeTable(inodeNo))
	{
		cout << endl << "open되어있는 함수임" << endl;
		return fd;// fd = 0 -> 이미 오픈되어있는 경우
	}

	fd = tm.fileOpenEvent(inodeNo, inode);
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
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

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

void File::writeFile(int fd, char* buffer)//,  TableManager& tm, FileSystem& fs )
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

	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}
	/*
	(FS)   data block[idx]에 저장된 데이터 읽어오기
	하나의 data에다 각 데이터블럭에 분산됬던 것들을 합침
	*/
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
	for (int i = 0; i <7; i++)
		fileSize[i] = ' ';
	itoa(length, fileSize);

	inode.size = fileSize;

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
	/* 아마 translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);와 같은 역할, 안되면 바꿔서 해보기

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block 의 size, time(파일 마지막 접근시간), mtime(파일이 마지막으로 수정된 시간) blocks, block[] 갱신 -> FT, FS 모두 갱신 */
	char* currTime;
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);
}// writeFile

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
	memcpy(fileSize, &size, sizeof(int));
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

		int returnIdx = fs.writeFS(blockData);
		/*
		fs.writeFS 에서는
		block descriptor Table 미할당된 블록 수, -> 초기화할때랑 다시 써줄 때 갱신해줘야 함
		Block Bitmap에 idx에 해당하는 비트 1로 set -> write에서 데이터블럭에 쓴 후 바로 바로 해주는 걸로
		write에서  Datablock idx찾아서 DataBlock에 저장 후 blockBitmap 설정해줘야함 그리고 idx 반환해주는 걸로
		*/
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);
	/* 아마 translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);와 같은 역할, 안되면 바꿔서 해보기

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

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

int File::unlinkFile(char* file)//, TableManager& tm, FileSystem& fs )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	// 전체 System의 TableManager에서 unlink할 file이 open되어있는지 검사
	// 상위 디렉토리로 부터 아이노드를 읽어와 tmpTm말고 파라미터로 넘어온 시스템의 TM에서 파일이 오픈되어있는지 검색한다 --> inode테이블에 inode로 검색하는 기능 추가
	// 아이노드 번호가 전체 시스템의 시스템파일 테이블에 있으면 파일이 오픈되어있는 것이므로 Unlink 불가
	// 파일이 속한 디렉토리로부터 파일의 아이노드번호를 받아온다
	// 절대경로(file)을 path클래스의 함수를 통해 차례로 오픈해준다.
	TableManager tmpTM = TableManager::getTmpInstance();

	vector<string>* pathFiles = pm.getAllAbsPath(file);// 파일을 차례로 오픈해줌
													   // 대상 파일포함하여 모두 오픈 ( Mode 변경해야하니깐 대상 파일도 오픈 )
	int count = pathFiles->size();
	int *fd = new int[count];
	for (int i = 0; i < count; i++)
	{
		fd[i] = openFile(stringToCharArr((*pathFiles)[i]), tmpTM);
	}

	InodeElement* delFileInode = (InodeElement*)tmpTM.getElement(INODET, fd[count - 1]);

	// 현재 시스템의 테이블에 존재한다는 것은 오픈되어있다는 것이므로 실패!
	if (tm.isExistInInodeTable(delFileInode->inode_number))
		return 1;

	// 없는경우 정상적으로 파일을 unlink해줘야 함.
	// 아이노드 번호에 해당하는 아이노드 정보를 읽어옴
	// 속하는 디렉토리에서 해당 파일과 아이노드번호 삭제 --> 디렉토리 클레스에서 파일명을 통해 엔트리 제거하는 함수 필요
	DirectoryManager dm = DirectoryManager::getTmpInstance();
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));
	dir.rmDirectory(delFileInode->inode_number);
	/* 원래 inode의 링크수를 따져서 링크수가 0인것만 지워야하지만, 이 과제에서는 링크수를 고려하지 않으니깐 바로 삭제할 것 */

	fs.writeFS(delFileInode->inode_number);
	/*
	InodeBitmap의 해당 비트 0으로 set
	BlockBitmap의 dataIdx 에 해당하는 비트 0으로 set --> blockBitmap이 0이면 비활성이므로 따로 초기화과정을 거치지 않아도 됨
	InodeBlock에서 아이노드 번호에 해당하는 아이노드 삭제 -->  InodeBitmap이 0이면 비활성이므로 따로 초기화과정을 거치지 않아도 됨
	*/

	return 0;
}

/*
Inode는 파일을 오픈할때 파일시스템의 아이노드블럭에서 읽어와 아이노드테이블에 넣어준다
파일이 열려있는 동안에는 아이노드테이블에서만 아이노드를 얻어오므로 아이노드테이블을 갱신해준다.
파일 클로즈 시에 아이노드테이블의 아이노드를 읽어와서 아이노드 블럭에 덮어씌운다.
*/

/*
void File::writeFile( int fd, void* buffer, int size)
{
char* data ;
if ( size > BLOCK_SIZE )
{
memcpy( data, buffer, BLOCK_SIZE);
//writeFS( data );

memcpy( data, ((char*)buffer + BLOCK_SIZE) , BLOCK_SIZE);
writeFile( fd, data, sizeof(data));
}
else
memcpy( data, buffer, size);
}
*/



/* 쉘 관련 */

int File::findFile(char* file) // 상대경로 혹은 절대경로 
{
	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	char* absFile = pm.getAbsolutePath(file);

	TableManager tmpTM = TableManager::getTmpInstance();
	vector<string>* pathFiles = pm.getAllAbsPath(file);// 파일을 차례로 오픈해줌
	int count = pathFiles->size();
	int *fd = new int[count];
	for (int i = 0; i < count - 1; i++)
	{
		fd[i] = openFile(stringToCharArr((*pathFiles)[i]), tmpTM);
	}
	InodeElement* dirInode = (InodeElement*)tmpTM.getElement(INODET, fd[count - 2]);
	DirectoryManager dm = DirectoryManager::getTmpInstance();
	Directory* dir = dm.returnDir(dirInode->inode_number);

	Entry* fileEntry = dir->findName(file);

	if (fileEntry == nullptr)
	{
		string path = pm.getCurrentPath() + '/';
		string filename = file;
		path = path + filename;

		createFile((char*)path.c_str());
	}

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	return openFile(file, tm);
}

/*  chmod 관련   */
void File::changeFileMode(char* file, char* mode) // file 은filename
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;
	int fd = findFile(file);

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

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
	int fd = findFile(file);

	writeFile(fd, (char*)data.c_str(), data.length());
}

// cat >> a 의 경우 : 이어쓰기
void File::joinCat(char* file, char* data) // file 은 filename
{
	int fd = findFile(file);

	writeFile(fd, data);
}

// cat a 의 경우 :: 20줄씩 보기, 줄 기준은 endline( enter ASCII 13번 )
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
	int fd = findFile(sourceFile);

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	InodeElement* sourceInodeE = (InodeElement*)tm.getElement(INODET, fd);
	int halfSize = atoi(sourceInodeE->inode.size) / 2;

	// first_target파일에 반만큼 저장
	int firstTargetFd = findFile(first_target);
	char* firstFileData;
	readFile(fd, firstFileData, halfSize);
	writeFile(firstTargetFd, firstFileData, halfSize);

	// second_target에 나머지 반 저장
	int secondTargetFd = findFile(second_target);
	char* secondFileData;
	readFile(fd, secondFileData, halfSize);
	writeFile(secondTargetFd, secondFileData, halfSize);

}

/*  paste 관련   */
void File::pasteFile(char* firstFile, char* secondFile)
{
	int firstFileFd = findFile(firstFile);
	int secondFileFd = findFile(secondFile);

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
	int fd = findFile(targetFile);//-> filename으로 file create해줄 것 //-> file open할것 
	writeFile(fd, (char*)data.c_str(), data.length());//-> filewrite 해줄 것
}

/*  close 관련   */