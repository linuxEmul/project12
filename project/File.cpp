#include "File.h"


File::File() { }

File::~File() { }


void File::createFile(Entry* fileEntry, int& dirInodeNo)//, DirectoryManager& dm )
{
	FileSystem& fs = *FileSystem::getInstance();

	Inode inode;

	char mode[5] = "f666";

	char time[13];
	getCurrentTime(time);

	char linkCount[2] = "1";
	char blocks[2] = "1";

	char size[2] = "0"; // getSizeOfDataBlock( int idx );
						// Inode Table�� ���� ���ֱ� ( char* mode ,int size, struct tm lastTime, struct tm creatTime, struct tm modifyTime,  int links_count, int blocks, int dataIdx) 
						// Inode Table������ block[]�� dataIdx ������

	inode.mode = mode;
	inode.size = size;

	// ���ϻ����ÿ��� ��� �����Ƿ� �Ű������� ������ Ÿ�� ��ü�� �ѱ�
	inode.time = time;      // ���� ������ ���� �ð� ����
	inode.ctime = time;      // ���� ���� �ð� ����
	inode.mtime = time;      // ������ ���������� ������ �ð� ����
	inode.linksCount = linkCount;
	inode.blocks = blocks;

	int inodeNo = fs.writeFS(inode);
	/*
	(FS)   BlockDescriptor Table�� �� �Ҵ�� ��� ��, �� �Ҵ�� inode �� ����
	inode�� writeFS�� ���� FS�� ������ ���� ���� �������� FS�� �������ְ�,  inodeNo�� �޾ƿ´�.
	*/

	fileEntry->inodeNum = inodeNo;
	
	// directory.addDir( inodeNo, filename ); // ���ϸ�� ����InodeNo�� ���丮�� ����
	Directory dir;
	dir.addDirectory( *fileEntry, dirInodeNo );
}   // createFile

int File::openFile( Entry& file )//, FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	PathManager& pm = *PathManager::getInstance();

	int fd = 0;

	Inode inode = fs.readFS( file.inodeNum );
	/* (FS)   InodeBlock���� inodeNo��° �о� �� */

	if ( tm.isExistInInodeTable( file.inodeNum ) )
	{
		cout << endl << "open�Ǿ��ִ� ������" << endl;
		return fd;// fd = 0 -> �̹� ���µǾ��ִ� ���
	}

	fd = tm.fileOpenEvent( file.inodeNum , inode );
	/*
	InodeTable�� �о�� inode���� ����->�ý����������̺�->���ϵ�ũ���� ������

	InodeBlocks���� ������ ���̳�� ������ �о� ���̳�� ���̺��� �ε��� n�� ����
	�ý��� ���� ���̺��� �ε��� k�� ���� ������ ����
	inode��ȣ�� �ε���n�� �����ϰ� ���� �����ʹ� 0���� �ʱ�ȭ
	���� ��ũ���� ���̺��� �ε��� j �� �ý��� ���� ���̺��� �ε��� k ����
	�ε��� j ����
	*/

	return fd;      //���� ��ũ���� ��ȯ
} // openFile

void File::readFile(int fd, char* buffer, int size)//,TableManager& tm , FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	
	InodeElement* inodeE = tm.getInodeByFD( fd );
	Inode inode = inodeE->inode;

	int blocks = atoi(inode.blocks);
	int dataIdx[20];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	int filePointer = ((SFTElement*)tm.getElement(SFT,  *(int*)tm.getElement( FDT, fd ) ) )->file_pointer;// (FT)   getFilePoint( fd ) fd�� ����Ű�� �ý����������̺��� ���������͸� �޾ƿ´�

																		  // �ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
   string data = "";
   for (int i = 0; i < blocks; i++)
   {
      char blockData[BLOCK_SIZE];
      fs.readFS(dataIdx[i], blockData);
      data += blockData;
   }

	int fileSize = data.length();
	if (size == 0) // ���ϳ��� ��ü ���� �� ���
		size = fileSize;
	else if (fileSize < size) // ����ũ�Ⱑ ����ڰ� ���� size���� ū ���, ���� ���Ͽ� ������ ���븸ŭ���� ������ ����
		size = fileSize;

	std::strcpy(buffer, data.substr(filePointer, size).c_str());
	tm.updateFilePointer(fd, filePointer + size);

	getCurrentTime(inode.time);

	// (FT)   Inode Block �� time(���� ������ ���ٽð�) ����
	tm.updateInode(fd, inode);
	// (FS)   inode ����
	fs.updateInode_readFile(inodeE->inode_number, inode);
}//readFile

 // ������ �ִ� ���ϵ����Ϳ� buffer�� �ٿ� ����
void File::writeFile(int fd, char* buffer)//,  TableManager& tm, FileSystem& fs )
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	// (FT)   fd�� Ÿ���� inode���� data block idx�޾ƿ���
	InodeElement* inodeE = tm.getInodeByFD( fd );
	Inode inode = inodeE->inode;
	int blocks = atoi(inode.blocks);
	int dataIdx[20];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	/*
		(FS)   data block[idx]�� ����� ������ �о����
		�ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	*/
   string data = "";
   for (int i = 0; i < blocks; i++)
   {
      char blockData[BLOCK_SIZE];
      fs.readFS(dataIdx[i], blockData);
      data += blockData;
   }
	//cout << data.length() << endl;
	//string filedata = data;// + buffer;   // ���������ͺ��� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ��� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ���� blockdata�� ����
									   */

	if ( inode.size[0] == '0' )
		data.clear();

	data += buffer;
	int length = data.length();
	char fileSize[7];
	itoa(length, inode.size);
	inode.size[strlen( inode.size )] ='\0';

	// ������ �����͸� FS�� DataBlock�� ���ִ� �κ�
	char blockData[BLOCK_SIZE];
	int new_Blocks = 0;
	while (length != 0)
	{
		if (length > BLOCK_SIZE)
		{
			strcpy(blockData, (data.substr(0, BLOCK_SIZE)).c_str());

			length -= BLOCK_SIZE;
		}

		else
		{
			strcpy(blockData, (data.substr(0, length)).c_str());

			length = 0;
		}

		int returnIdx = fs.writeFS(blockData);
		/*
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ��� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ��� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
		write����  Datablock idxã�Ƽ� DataBlock�� ���� �� blockBitmap ����������� �׸��� idx ��ȯ���ִ� �ɷ�
		*/
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
	char currTime[13] = { 0 };
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	//update Inode
	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);
}// writeFile

// ������ �ִ� ���ϵ����͸� buffer�� ���� ����
void File::writeFile(int fd, char* buffer, int size)//,  TableManager& tm, FileSystem& fs )
{
	TableManager& tm = *TableManager::getInstance();
	FileSystem& fs = *FileSystem::getInstance();

	if( size >= BLOCK_SIZE * 12 )
	{
		cout << " error:: overflow file size " << endl;
		return ;
	}

	// (FT)   fd�� Ÿ���� inode���� data block idx�޾ƿ���
	InodeElement* inodeE = tm.getInodeByFD( fd );
	Inode inode = inodeE->inode;
	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	string filedata = buffer;   // ���������ͺ��� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ��� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ���� blockdata�� ����
									   */
	char fileSize[7] = { 0 };
	if ( size != 0 )
	{
		itoa( size, fileSize );
		inode.size = fileSize;
	}

	char* blockData;
	int new_Blocks = 0;
	int fp = 0;
	while (size != 0)
	{
		if (size > BLOCK_SIZE)
		{
			blockData = new char[BLOCK_SIZE];
			strcpy(blockData, (filedata.substr(fp, BLOCK_SIZE)).c_str());

			size -= BLOCK_SIZE;
			fp += BLOCK_SIZE;
		}

		else
		{
			blockData = new char[size];
			strcpy(blockData, (filedata.substr(0, size)).c_str());

			size = 0;
		}

		int returnIdx = fs.writeFS( blockData );
		
		//translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks, 1); // inode�� �Ҵ���� �����ͺ� �ε��� ����
		itoa( returnIdx, &inode.dataBlockList[ new_Blocks*3 ] );

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);	// blocks( �Ҵ���� �����ͺ� �� ) ���� 

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
	char currTime[13] = { 0 };
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);
}// writeFile

void File::seekFile(int fd, int offset)//, TableManager& tm )
{
	TableManager& tm = *TableManager::getInstance();

	int filePointer;

	if (offset < 0)   // offset�� ������ ���, ������ ���� ���ϵ��� �Ѵ�
	{      //fp = getFileSize();
		Inode inode = ((InodeElement*)tm.getElement(INODET, fd))->inode;
		filePointer = atoi(inode.size) - 1;
	}
	else
		filePointer = offset;   // fp = offset; 

								// (FT)   fp �� �ý����������̺��� ���������ͷ� ����
	tm.updateFilePointer(fd, filePointer);
}

void File::closeFile(int fd)//, TableManager& tm )
{
	TableManager& tm = *TableManager::getInstance();

	//inode Table���� inode�� �о�� inodeBlock update ����� ��
	Inode inode = ((InodeElement*)tm.getElement(INODET, fd))->inode;

	// (FT)  Inode Table ���� ����, �ý����������̺� ���� ����, ���ϵ�ũ�������̺� ���� ����
	tm.fileCloseEvent(fd);
}

int File::unlinkFile(char* file) // file �� absPath ����
{
	FileSystem& fs = *FileSystem::getInstance();
	PathManager& pm = *PathManager::getInstance();
	TableManager& tm = *TableManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	int* dirInodeNo = new int;
	Entry* fileEntry = findFile( file, dirInodeNo );

	if( fileEntry == nullptr )
	{
		cout << " �������� �ʴ� ���� !  " << endl;
		return 1;
	}

	// ���� �ý����� ���̺� �����Ѵٴ� ���� ���µǾ��ִٴ� ���̹Ƿ� ����!
	if (tm.isExistInInodeTable( fileEntry->inodeNum ) )
	{
		cout << "������ ���µǾ��ֽ��ϴ�" << endl;
		return 1;
	}
	// ���°�� ���������� ������ unlink����� ��.
	// ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ������ �о��
	// ���ϴ� ���丮���� �ش� ���ϰ� ���̳���ȣ ���� --> ���丮 Ŭ�������� ���ϸ��� ���� ��Ʈ�� �����ϴ� �Լ� �ʿ�
	vector<string>* pathFiles = pm.getAllAbsPath( file );

	Directory* dir = dm.returnDir( *dirInodeNo );
	dir->rmDirectory( fileEntry->inodeNum, *dirInodeNo );
	/* ���� inode�� ��ũ���� ������ ��ũ���� 0�ΰ͸� ������������, �� ���������� ��ũ���� ������� �����ϱ� �ٷ� ������ �� */
	delete dirInodeNo;
	delete dir;
	//fs.writeFS( fileEntry->inodeNum ); // inode ��ȣ �Ѱܼ� �������� �ʱ�ȭ

	return 0;
}

/* �� ���� */

Entry* File::findFile( char* filename,  int* dirInodeNo ) // ������ 
{
	FileSystem& fs = *FileSystem::getInstance();
	PathManager& pm = *PathManager::getInstance();
	TableManager& tm = *TableManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	vector<string>& filenames = pm.doAnalyzeFolder( filename );
	vector<string>& pathFiles = *pm.getAllAbsPath( filename );

	Entry* fileEntry;
	
	//int count = pathFiles.size();
	vector<string>::size_type count = pathFiles.size();
	// Ÿ������ ������ �����θ� ���� ���丮�� ã�� ���̳�� ��ȣ�� ����


	*dirInodeNo = dm.returnInodeNum( (char*) pathFiles.at( count -2 ).c_str() );
	// ���丮�� ���̳�� ��ȣ�� ���� ���丮�� �޾ƿ�
	Directory* dir = dm.returnDir( *dirInodeNo );
	fileEntry = dir->findName( (char*)filenames[ filenames.size()-1].c_str() ); // ���丮���� ���Ͽ�Ʈ���� ã��
	//delete dir;

	return fileEntry; // ��ã�� ��� nullptr
}

//file entry�� nullptr�̸� ������ ����������ϴ� ��ɾ� : cat, 
int File::createAndOpen( char* file, Entry* fileEntry, int& dirInodeNo )
{
	PathManager& pm = *PathManager::getInstance();
	vector<string> filenames = pm.doAnalyzeFolder( file );
	char*  filename = (char*)filenames[ filenames.size() -1 ].c_str();

	if (fileEntry == nullptr)	// ���丮�� ���Ͽ�Ʈ���� ���� ���, ������ ������ �ش�
	{
		int nameLen = strlen(filename);
		fileEntry = new Entry;
		memcpy( fileEntry->name, filename,  nameLen );
		fileEntry->name[nameLen] = '\0';
		createFile( fileEntry, dirInodeNo );
	}

	return openFile( *fileEntry );
}

//file entry�� nullptr�̸� ����ó���� �ʿ��� ��ɾ� : rm ,  chmod, copy( a�� ����ó�� , b�� �������ٰ� ) , split, paste ( �Ű����� �Ѵ� find)
int File::open( Entry* fileEntry )
{
	int fd;

	if (fileEntry == nullptr)
	{
		cout << endl << "�ش� ������ ����." << endl;
		fd = -1;
		return fd;
	}
	
	return openFile( *fileEntry );
}

/*  chmod ����   */
void File::changeFileMode(char* file, char* mode) // file �� ������
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	PathManager& pm = *PathManager::getInstance();

	int* dirInodeNo = new int;

	int fd = open( findFile(file, dirInodeNo) );

	InodeElement* targetFileInodeE = tm.getInodeByFD( fd );
	Inode inode = targetFileInodeE->inode; // file�� �ش��ϴ� inode�� ���´�
										   // file�� inode���� mode�� �������ش�
	inode.mode = new char[5];
	for (int i = 0; i < 5; i++)
		inode.mode[i] = mode[i];
	// update���ش�--> closeFile�ؼ� FS�� �����������
	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(targetFileInodeE->inode_number, inode);
}

/*  cat ����   */

// cat > a �� ��� : �����
void File::overwriteCat(char* file, string data) // file�� path
{
	int* dirInodeNo = new int;
	int fd = createAndOpen( file, findFile(file, dirInodeNo), *dirInodeNo  );
	
	if (fd <= 0 )
		return;
	
	delete dirInodeNo;
	writeFile(fd, (char*)data.c_str(), data.length());
}

// cat >> a �� ��� : �̾��
void File::joinCat(char* file, char* data) // file �� filename
{
	int* dirInodeNo = new int;
	int fd = createAndOpen( file, findFile(file, dirInodeNo), *dirInodeNo  );
	delete dirInodeNo;
	
	if (fd <= 0 )
		return;
	
	writeFile(fd, data);
}

// cat a �� ��� :: 20�پ� ����, �� ������ endline( enter ASCII 13�� )
// file�� ���� ��� ������ ����ó�� ���� �� 
bool File::displayCat(int fd)
{
	//int fd = openFile( file, *(TableManager::getInstance()) ); // dispalyCat �ܺο��� ���� ��

	const int perLineChar = 50;	// �� ������ ���� ��
	const int lineNo = 20; // �ѹ��� ������� ���� ��

	char buffer[ perLineChar * lineNo ];

	int size = perLineChar * lineNo;
	readFile(fd, buffer,  size );

	int dataSize = strlen(buffer);

	int i = 0;
	int lines = 0;
	while( dataSize >= 0 )
	{
		cout << buffer[i];
		i++;

		if( i % perLineChar == 0 )
		{
			lines++;
			cout << endl;
		}

		if ( lines == lineNo ) // ������ ��� �̾����� ��� 
			return true;

		dataSize--;
	}
	
	return false; // ������ ���� ���
}

/*  rm ����   */
void File::removeFile(char* file)// file�� ������
{
	int error = unlinkFile( file );

	if ( error == 1 ) // error = 0 ���� / error = 1 ����
		cout << "���� ���� �������� ������ �߻��߽��ϴ�..." << endl;
}

/*  chmod ����   */
void File::splitFile(char* sourceFile, char* first_target, char* second_target) // sourceFile�� file�� ������, first_target�� xFilenamea, second_target�� xFilenameb 
{
	// source������ �����͸� �о�´�
	int* dirInodeNo = new int;
	int fd = open( findFile(sourceFile, dirInodeNo) );

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	InodeElement* sourceInodeE = tm.getInodeByFD( fd );
	int fileSize = atoi(sourceInodeE->inode.size);
	int halfSize1 = 0;
	int halfSize2 = 0;
	if ( ( fileSize % 2 ) == 0 )
	{
		halfSize1 = fileSize/2;
		halfSize2 = halfSize1;
	}
	else
	{
			halfSize1 = fileSize/2 + 1;
			halfSize2 = halfSize1 - 1;
	}

	// first_target���Ͽ� �ݸ�ŭ ����
	int firstTargetFd = createAndOpen( first_target, findFile( first_target, dirInodeNo ), *dirInodeNo );

	// second_target�� ������ �� ����
	int secondTargetFd = createAndOpen( second_target, findFile(second_target, dirInodeNo), *dirInodeNo );
	
	if( firstTargetFd <= 0 || secondTargetFd <= 0 )
		return;

	char firstFileData[BLOCK_SIZE*12];
	readFile(fd, firstFileData, halfSize1);
	writeFile(firstTargetFd, firstFileData, halfSize1);

	char secondFileData[BLOCK_SIZE*12];
	readFile(fd, secondFileData, halfSize2);
	writeFile(secondTargetFd, secondFileData, halfSize2);

	delete dirInodeNo;
}

/*  paste ����   */
void File::pasteFile(char* firstFile, char* secondFile)
{
	int* dirInodeNo = new int;
	int firstFileFd = open( findFile(firstFile, dirInodeNo ) );
	int secondFileFd = open( findFile(secondFile, dirInodeNo ) );
	delete dirInodeNo;

	if( firstFileFd <= 0 || secondFile <= 0 )
		return;

	char secondFileData[BLOCK_SIZE*12];
	readFile(secondFileFd, secondFileData, 0);

	writeFile(firstFileFd, secondFileData);
}

/* copy (local) file */
void File::copyFile( char* sourceFile, char* targetFile )
{
	int* dirInodeNo = new int;
	int sourceFileFd = open( findFile(sourceFile, dirInodeNo ) );
	
	if( sourceFileFd <= 0 )
		return;

	int targetFileFd = createAndOpen( targetFile, findFile(targetFile, dirInodeNo ), *dirInodeNo );
	delete dirInodeNo;


	char sourceFileData[BLOCK_SIZE*12];
	readFile(sourceFileFd, sourceFileData, 0);

	writeFile(targetFileFd, sourceFileData, strlen(sourceFileData) );
}

/*  file copy ����   */
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

void File::copySysFile(char* sourceSysFile, char* targetFile)
{
	string data = readSystemFile(sourceSysFile);//   System file read 

	int* dirInodeNo = new int;
	int fd = createAndOpen(targetFile, findFile(targetFile, dirInodeNo), *dirInodeNo );//-> filename���� file create���� �� //-> file open�Ұ� 

	if( fd <= 0 )
		return;

	writeFile(fd, (char*)data.c_str(), data.length());//-> filewrite ���� ��

	delete dirInodeNo;
}

/*  close ����   */