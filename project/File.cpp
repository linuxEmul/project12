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

	/* ( ���� ) ���� ���� �� ������ ���� ������ 1 ( �� ������ �����ϹǷ� ), ������ ���� �ε����� FileSystem���� �Ҵ����� */
	// datablock[dataIdx] �� ���ϸ� ���� setDatablock( int idx, char* data ) --> ( dataIdx, file );
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

	vector<string>* pathFiles = pm.getAllAbsPath(file);
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));

	Entry fileEntry;
	fileEntry.inodeNum = inodeNo;

	vector<string> files;
	pm.doAnalyzeFolder(file, files);// ������ ���ʷ� ��������

	strcpy(fileEntry.name, files[files.size() - 1].c_str());

	// directory.addDir( inodeNo, filename ); // ���ϸ�� ����InodeNo�� ���丮�� ����
	dir.addDirectory(fileEntry, inodeNo);
}   // createFile

int File::openFile(char* file, TableManager& tm)//, FileSystem& fs )
{
	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	int fd = 0;

	// InodeTable���� ���� ���丮�� ������ �� idx �޾ƿ�
	vector<string>* pathFiles = pm.getAllAbsPath(file);// ������ ���ʷ� ��������
	DirectoryManager dm = DirectoryManager::getTmpInstance();
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));

	// ���͸��� �����ͺ� �о��
	Entry* dirFileEntry = dir.findName(stringToCharArr((*pathFiles)[pathFiles->size() - 1]));

	if (dirFileEntry == nullptr)
	{
		cout << endl << "�ش� ���丮�� ������ ����." << endl;
		fd = -1;
		return fd;
	}

	int inodeNo = dirFileEntry->inodeNum;
	Inode inode = fs.readFS(inodeNo);
	/* (FS)   InodeBlock���� inodeNo��° �о� �� */

	if (tm.isExistInInodeTable(inodeNo))
	{
		cout << endl << "open�Ǿ��ִ� �Լ���" << endl;
		return fd;// fd = 0 -> �̹� ���µǾ��ִ� ���
	}

	fd = tm.fileOpenEvent(inodeNo, inode);
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
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

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

void File::writeFile(int fd, char* buffer)//,  TableManager& tm, FileSystem& fs )
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

	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}
	/*
	(FS)   data block[idx]�� ����� ������ �о����
	�ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	*/
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
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ��� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ��� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
		write����  Datablock idxã�Ƽ� DataBlock�� ���� �� blockBitmap ����������� �׸��� idx ��ȯ���ִ� �ɷ�
		*/
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);
	/* �Ƹ� translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);�� ���� ����, �ȵǸ� �ٲ㼭 �غ���

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
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
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ��� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ��� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
		write����  Datablock idxã�Ƽ� DataBlock�� ���� �� blockBitmap ����������� �׸��� idx ��ȯ���ִ� �ɷ�
		*/
		translateIntArrToCharArr(&returnIdx, inode.dataBlockList + new_Blocks, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inode.blocks, 1);
	/* �Ƹ� translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);�� ���� ����, �ȵǸ� �ٲ㼭 �غ���

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

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

int File::unlinkFile(char* file)//, TableManager& tm, FileSystem& fs )
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	// ��ü System�� TableManager���� unlink�� file�� open�Ǿ��ִ��� �˻�
	// ���� ���丮�� ���� ���̳�带 �о�� tmpTm���� �Ķ���ͷ� �Ѿ�� �ý����� TM���� ������ ���µǾ��ִ��� �˻��Ѵ� --> inode���̺� inode�� �˻��ϴ� ��� �߰�
	// ���̳�� ��ȣ�� ��ü �ý����� �ý������� ���̺� ������ ������ ���µǾ��ִ� ���̹Ƿ� Unlink �Ұ�
	// ������ ���� ���丮�κ��� ������ ���̳���ȣ�� �޾ƿ´�
	// ������(file)�� pathŬ������ �Լ��� ���� ���ʷ� �������ش�.
	TableManager tmpTM = TableManager::getTmpInstance();

	vector<string>* pathFiles = pm.getAllAbsPath(file);// ������ ���ʷ� ��������
													   // ��� ���������Ͽ� ��� ���� ( Mode �����ؾ��ϴϱ� ��� ���ϵ� ���� )
	int count = pathFiles->size();
	int *fd = new int[count];
	for (int i = 0; i < count; i++)
	{
		fd[i] = openFile(stringToCharArr((*pathFiles)[i]), tmpTM);
	}

	InodeElement* delFileInode = (InodeElement*)tmpTM.getElement(INODET, fd[count - 1]);

	// ���� �ý����� ���̺� �����Ѵٴ� ���� ���µǾ��ִٴ� ���̹Ƿ� ����!
	if (tm.isExistInInodeTable(delFileInode->inode_number))
		return 1;

	// ���°�� ���������� ������ unlink����� ��.
	// ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ������ �о��
	// ���ϴ� ���丮���� �ش� ���ϰ� ���̳���ȣ ���� --> ���丮 Ŭ�������� ���ϸ��� ���� ��Ʈ�� �����ϴ� �Լ� �ʿ�
	DirectoryManager dm = DirectoryManager::getTmpInstance();
	Directory dir = dm.Dir_Read(stringToCharArr((*pathFiles)[pathFiles->size() - 2]));
	dir.rmDirectory(delFileInode->inode_number, dm.returnInodeNum(stringToCharArr((*pathFiles)[pathFiles->size()-2])));
	/* ���� inode�� ��ũ���� ������ ��ũ���� 0�ΰ͸� ������������, �� ���������� ��ũ���� ������� �����ϱ� �ٷ� ������ �� */

	fs.writeFS(delFileInode->inode_number);
	/*
	InodeBitmap�� �ش� ��Ʈ 0���� set
	BlockBitmap�� dataIdx �� �ش��ϴ� ��Ʈ 0���� set --> blockBitmap�� 0�̸� ��Ȱ���̹Ƿ� ���� �ʱ�ȭ������ ��ġ�� �ʾƵ� ��
	InodeBlock���� ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ���� -->  InodeBitmap�� 0�̸� ��Ȱ���̹Ƿ� ���� �ʱ�ȭ������ ��ġ�� �ʾƵ� ��
	*/

	return 0;
}

/*
Inode�� ������ �����Ҷ� ���Ͻý����� ���̳������� �о�� ���̳�����̺� �־��ش�
������ �����ִ� ���ȿ��� ���̳�����̺����� ���̳�带 �����Ƿ� ���̳�����̺��� �������ش�.
���� Ŭ���� �ÿ� ���̳�����̺��� ���̳�带 �о�ͼ� ���̳�� ���� ������.
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



/* �� ���� */

int File::findFile(char* file) // ����� Ȥ�� ������ 
{
	PathManager* p = PathManager::getInstance();
	PathManager& pm = *p;

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

	char* absFile = pm.getAbsolutePath(file);

	TableManager tmpTM = TableManager::getTmpInstance();
	vector<string>* pathFiles = pm.getAllAbsPath(file);// ������ ���ʷ� ��������
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

/*  chmod ����   */
void File::changeFileMode(char* file, char* mode) // file ��filename
{
	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;
	int fd = findFile(file);

	FileSystem* f = FileSystem::getInstance();
	FileSystem& fs = *f;

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
	int fd = findFile(file);

	writeFile(fd, (char*)data.c_str(), data.length());
}

// cat >> a �� ��� : �̾��
void File::joinCat(char* file, char* data) // file �� filename
{
	int fd = findFile(file);

	writeFile(fd, data);
}

// cat a �� ��� :: 20�پ� ����, �� ������ endline( enter ASCII 13�� )
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
	int fd = findFile(sourceFile);

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	InodeElement* sourceInodeE = (InodeElement*)tm.getElement(INODET, fd);
	int halfSize = atoi(sourceInodeE->inode.size) / 2;

	// first_target���Ͽ� �ݸ�ŭ ����
	int firstTargetFd = findFile(first_target);
	char* firstFileData;
	readFile(fd, firstFileData, halfSize);
	writeFile(firstTargetFd, firstFileData, halfSize);

	// second_target�� ������ �� ����
	int secondTargetFd = findFile(second_target);
	char* secondFileData;
	readFile(fd, secondFileData, halfSize);
	writeFile(secondTargetFd, secondFileData, halfSize);

}

/*  paste ����   */
void File::pasteFile(char* firstFile, char* secondFile)
{
	int firstFileFd = findFile(firstFile);
	int secondFileFd = findFile(secondFile);

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
	int fd = findFile(targetFile);//-> filename���� file create���� �� //-> file open�Ұ� 
	writeFile(fd, (char*)data.c_str(), data.length());//-> filewrite ���� ��
}

/*  close ����   */