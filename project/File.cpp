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

	Entry fileEntry;
	strcpy( fileEntry.name,  filename );
	fileEntry.inodeNum = inodeNo;
	
	// directory.addDir( inodeNo, filename ); // ���ϸ�� ����InodeNo�� ���丮�� ����
	dir.addDirectory(fileEntry, inodeNo);
}   // createFile

int File::openFile( Entry file )//, FileSystem& fs )
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
	
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	Inode inode = inodeE->inode;

	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	int filePointer = ((SFTElement*)tm.getElement(SFT, fd))->file_pointer;// (FT)   getFilePoint( fd ) fd�� ����Ű�� �ý����������̺��� ���������͸� �޾ƿ´�

																		  // �ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	int fileSize = data.length();
	if (size == 0) // ���ϳ��� ��ü ���� �� ���
		size = fileSize;
	else if (fileSize < size) // ����ũ�Ⱑ ����ڰ� ���� size���� ū ���, ���� ���Ͽ� ������ ���븸ŭ���� ������ ����
		size = fileSize;

	buffer = new char[size];

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
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	Inode inode = inodeE->inode;
	int blocks = atoi(inode.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	/*
		(FS)   data block[idx]�� ����� ������ �о����
		�ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	*/
	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	string filedata = data + buffer;   // ���������ͺ��� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ��� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ���� blockdata�� ����
									   */
	int length = filedata.length();
	char fileSize[7];
	itoa(length, fileSize);

	inode.size = fileSize;

	// ������ �����͸� FS�� DataBlock�� ���ִ� �κ�
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
	char* currTime;
	getCurrentTime(currTime);
	inode.time = currTime;
	inode.mtime = currTime;

	//update Inode
	tm.updateInode(fd, inode);
	fs.updateInode_writeFile(inodeE->inode_number, inode);

	delete dataIdx;
}// writeFile

// ������ �ִ� ���ϵ����͸� buffer�� ���� ����
void File::writeFile(int fd, char* buffer, int size)//,  TableManager& tm, FileSystem& fs )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	// (FT)   fd�� Ÿ���� inode���� data block idx�޾ƿ���
	InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
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
		
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks, 1); // inode�� �Ҵ���� �����ͺ� �ε��� ����

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);	// blocks( �Ҵ���� �����ͺ� �� ) ���� 

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
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
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

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

	Directory* dir;

	Entry* fileEntry = findFile( file );

	// ���� �ý����� ���̺� �����Ѵٴ� ���� ���µǾ��ִٴ� ���̹Ƿ� ����!
	if (tm.isExistInInodeTable( fileEntry->inodeNum ) )
		return 1;

	// ���°�� ���������� ������ unlink����� ��.
	// ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ������ �о��
	// ���ϴ� ���丮���� �ش� ���ϰ� ���̳���ȣ ���� --> ���丮 Ŭ�������� ���ϸ��� ���� ��Ʈ�� �����ϴ� �Լ� �ʿ�
	vector<string>* pathFiles = pm.getAllAbsPath( file );

	*dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));
	dir->rmDirectory( fileEntry->inodeNum, dm.returnInodeNum(stringToCharArr((*pathFiles)[pathFiles->size()-2])));
	/* ���� inode�� ��ũ���� ������ ��ũ���� 0�ΰ͸� ������������, �� ���������� ��ũ���� ������� �����ϱ� �ٷ� ������ �� */

	fs.writeFS( fileEntry->inodeNum ); // inode ��ȣ �Ѱܼ� �������� �ʱ�ȭ

	return 0;
}

/* �� ���� */

Entry* File::findFile( char* filename,  Directory* dir ) // ����� Ȥ�� ������ 
{
	FileSystem& fs = *FileSystem::getInstance();
	PathManager& pm = *PathManager::getInstance();
	TableManager& tm = *TableManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	char* absPath = pm.getAbsolutePath( filename );
	vector<string>* pathFiles = pm.getAllAbsPath( absPath );

	Entry* fileEntry;
	
	int count = pathFiles->size();

	// Ÿ������ ������ �����θ� ���� ���丮�� ã�� ���̳�� ��ȣ�� ����
	int dirInodeNo = dm.returnInodeNum( (char*)*pathFiles->at( count -2 ).c_str() );
	// ���丮�� ���̳�� ��ȣ�� ���� ���丮�� �޾ƿ�
	dir = dm.returnDir( dirInodeNo );

	cout << "������ �߰��� ���丮�� ���̳�� �ѹ� :  "<< dirInodeNo << endl;

	fileEntry = dir->findName( filename ); // ���丮���� ���Ͽ�Ʈ���� ã��

	if (fileEntry == nullptr)	// ���丮�� ���Ͽ�Ʈ���� ���� ���, ������ ������ �ش�
	{
		vector<string> files = pm.doAnalyzeFolder( absPath );
		string filename = files[ files.size() - 1];

		createFile( (char*)filename.c_str() , *dir );
	}

	return fileEntry;
}

//file entry�� nullptr�̸� ������ ����������ϴ� ��ɾ� : cat, 
int File::createAndOpen( Entry* fileEntry, Directory& dir )
{
	PathManager& pm = *PathManager::getInstance();
	char* absPath = pm.getAbsolutePath( fileEntry->name );

	if (fileEntry == nullptr)	// ���丮�� ���Ͽ�Ʈ���� ���� ���, ������ ������ �ش�
	{
		vector<string> files = pm.doAnalyzeFolder( absPath );
		string filename = files[ files.size() - 1];

		createFile( (char*)filename.c_str() , dir );
	}

	return openFile( *fileEntry );
}

//file entry�� nullptr�̸� ����ó���� �ʿ��� ��ɾ� : rm ,  chmod, copy( a�� ����ó�� , b�� �������ٰ� ) , split, paste ( �Ű����� �Ѵ� find)
int File::open( Entry* fileEntry )
{
	int fd;

	if (fileEntry == nullptr)
	{
		cout << endl << "�ش� ���丮�� ������ ����." << endl;
		fd = -1;
		return fd;
	}
	
	return openFile( *fileEntry );
}

/*  chmod ����   */
void File::changeFileMode(char* file, char* mode) // file ��filename
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	int fd = open( findFile(file) );

	InodeElement* targetFileInodeE = (InodeElement*)tm.getElement(INODET, fd);
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
void File::overwriteCat(char* file, string data) // file�� filename�� pathX
{
	Directory dir;
	int fd = createAndOpen( findFile(file, &dir), dir  );

	writeFile(fd, (char*)data.c_str(), data.length());
}

// cat >> a �� ��� : �̾��
void File::joinCat(char* file, char* data) // file �� filename
{
	Directory dir;
	int fd = createAndOpen( findFile(file, &dir), dir  );

	writeFile(fd, data);
}

// cat a �� ��� :: 20�پ� ����, �� ������ endline( enter ASCII 13�� )
// file�� ���� ��� ������ ����ó�� ���� �� 
void File::displayCat(int fd)
{
	//int fd = openFile( file, *(TableManager::getInstance()) ); // dispalyCat �ܺο��� ���� ��

	int perLineChar = 50;
	int lineNo = 20;
	int size = perLineChar * lineNo; // ���ٿ� ������� char�� 50 , 20����

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

/*  rm ����   */
void File::removeFile(char* file)// file�� filename
{
	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	int error = unlinkFile(pm.getAbsolutePath(file));

	if (!error) // error = 0 ���� / error = 1 �������Ͽ���
		cout << "������ ���µǾ��־� ������ �� �����ϴ�." << endl;
}

/*  chmod ����   */
void File::splitFile(char* sourceFile, char* first_target, char* second_target) // sourceFile�� file�̸� first_target�� xFilenamea, second_target�� xFilenameb 
{
	// source������ �����͸� �о�´�
	int fd = open( findFile(sourceFile) );

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	InodeElement* sourceInodeE = (InodeElement*)tm.getElement(INODET, fd);
	int halfSize = atoi(sourceInodeE->inode.size) / 2;

	// first_target���Ͽ� �ݸ�ŭ ����
	Directory dir;
	int firstTargetFd = createAndOpen( findFile( first_target, &dir ), dir );
	char* firstFileData;
	readFile(fd, firstFileData, halfSize);
	writeFile(firstTargetFd, firstFileData, halfSize);

	// second_target�� ������ �� ����
	int secondTargetFd = createAndOpen( findFile(second_target, &dir), dir );
	char* secondFileData;
	readFile(fd, secondFileData, halfSize);
	writeFile(secondTargetFd, secondFileData, halfSize);

}

/*  paste ����   */
void File::pasteFile(char* firstFile, char* secondFile)
{
	int firstFileFd = open( findFile(firstFile) );
	int secondFileFd = open( findFile(secondFile) );

	char* secondFileData;
	readFile(secondFileFd, secondFileData, 0);

	writeFile(firstFileFd, secondFileData);
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

void File::copyFile(char* sourceFile, char* targetFile)
{
	string data = readSystemFile(sourceFile);//   System file read 
	Directory dir;
	int fd = createAndOpen( findFile(targetFile, &dir), dir );//-> filename���� file create���� �� //-> file open�Ұ� 
	writeFile(fd, (char*)data.c_str(), data.length());//-> filewrite ���� ��
}

/*  close ����   */