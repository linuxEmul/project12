#include "DirectoryManager.h"

#include "PathManager.h"
#include "InodeBlock.h"
#include "DataBlock.h"

void DirectoryManager::Dir_Create(char* direc)
{
	//Bitmap ���� �Լ��� ���� ��Ʈ�� ����
	
	Inode inode;

	// inode ����
	//��Ʈ�ʵ� 1�� ���� ..

	inode.mode = "d777";

	// int EmptyDataNum = dBlockBitmap.getEmpty();
	// int EmptyInodeNum = inodeBitmap.getEmpty();


	// ���� ��� �м�
	PathManager& pm = *PathManager::getInstance();
	vector<string> arr;

	pm.doAnalyzeFolder(direc, arr);

	int n = arr.size() ; // n: ��λ��� ���� ����?

	string currDir = arr[n-1]; // ������丮 �̸�
	string topDir = arr[n-2]; // �������丮 �̸�

	if ( direc == "/")
	{
		topDir = arr[n-1]; // ������丮 �̸�
	}
	//addDirectory 

	int currDirInode = returnInodeNum((char*)currDir.c_str());
	int topDirInode = returnInodeNum((char*)topDir.c_str());

	Directory curDr;
	Directory topDr;

	curDr.setInodeNum(currDirInode, topDirInode);

	if ( direc != "/")
	{
		Entry e;

		e.inodeNum = currDirInode;
		strcpy(e.name, currDir.c_str());

		if ( topDr.isExist((char*)currDir.c_str()) == true )
		{
			cout << "dir exist" << endl; 
			return ;
		}//���丮 �ߺ� �˻�


		topDr.addDirectory(e, currDirInode);
	}
	DataBlock dB;

	Entry *enList = curDr.entryList;

	string content = ".,";
	content += enList[0].inodeNum + ";..," + enList[1].inodeNum;
	
	FileSystem& fs = *FileSystem::getInstance();
	
	//memcpy(dB.data, (void *)content.c_str, sizeof(content)); 
	
	//�����ͺ�Ͽ� ������ �߰�(idx�� datablock Index)
	int idx = fs.writeFS((char*)content.c_str());
	
	char time[13];
	getCurrentTime(time);
	char size[2] = { '0' + content.length(), };
	char linkCount[2] = "1";

	//Inode ���� ����
	inode.blocks = "1";
	inode.linksCount = linkCount;
	inode.mtime = time;
	inode.size = size;
	inode.time = time;

	fs.writeFS(inode);
}

Directory DirectoryManager::Dir_Read(char* direc)
{
	FileSystem& fs = *FileSystem::getInstance();
	int accessNum = returnInodeNum(direc);
	InodeBlock inode = ((accessNum > 31) ? fs.inodeBlock[1] : fs.inodeBlock[0]);
	char* mode = inode.getMode(accessNum);
	if (strchr(mode,'f'))
		throw "������ �Է��߽��ϴ�.";
	else {
		Directory dir = *returnDir(accessNum);
		//openedDirList.push_back(dir);
		//openedDirList.erase(openedDirList.begin+5);
		return dir;
	}
}

void DirectoryManager::Dir_Unlink(char* direc)
{
	/*
	direc�� ��Ÿ���� ���丮�� ����.
	�ش� ���丮�� �����ϴ� ���� ���丮������ ���� ���� ����
	data block, inode�� ����, ���� bitmap�� 0���� ����
	���丮�� ������ �����ϸ� ���� �޽��� ���
	��Ʈ ���丮�� ���� �Ұ�
	*/

	Directory dr = Dir_Read(direc);

	if ( dr.entryCnt == 2 )
	{
		int topInodeNum = dr.findName("..")->inodeNum;
		int currInodeNum = dr.findName(".")->inodeNum;

		// ���̳�� ��ȣ�� �ý������� ���̺� ������ ���� Unlink������ �Ѵ�

		// ���ϴ� ���� ���丮�� ��Ʈ���� �ش� ���ϰ� ���̳���ȣ ����
		Directory d = *returnDir(topInodeNum);
		d.rmDirectory(currInodeNum);

		FileSystem& fs = *FileSystem::getInstance();

		// InodeBitmap�� �ش� ��Ʈ 0���� set
		// BlockBitmap�� dataIdx �� �ش��ϴ� ��Ʈ 0���� set
		// InodeBlock���� ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ����
		fs.writeFS(currInodeNum);
	}
}

int DirectoryManager::returnInodeNum( char * direc )
{
	// ���� ��ο� ���� �͵��� ���µǾ��  ...
	// ���ͷ� ��ȯ���� �͵� ���ʴ�� �� ����

	// Root���� ���ʷ� Ž��
	PathManager& pm = *PathManager::getInstance();
	vector<string> arr;

	pm.doAnalyzeFolder(direc, arr);

	FileSystem& fs = *FileSystem::getInstance();
	
	Inode inode =  fs.inodeBlock[0].getInodeData(0);

	DataBlock dBlock ; // fs�� ���� �����´�
	string str = "";


	for ( int i = 0 ; i < atoi(inode.blocks); i ++ )
	{
		str += dBlock.data[ inode.dataBlockList[i] ]; // ��ϸ���Ʈ�� �ִ� �����ͺ�ϸ���Ʈ
	}

	//2. ��Ʈ�� �� �����ϱ� -> ������ ";"

	//3. ��Ʈ�� �Ӽ� �̾Ƴ��� -> ������ ","

	string inodeNum;

	vector<string>& entry = *tokenize(str,";");
	for ( int j = 0; j < entry.size(); j ++ )
	{
		vector<string>& tmp = *tokenize(entry[j],",");

		if ( tmp[0] == arr[1] ) 
		{
			inodeNum = tmp[1];
			break;
		}

	}

	for ( int i = 1 ; i < arr.size()-1; i ++ )
	{
		Inode inode = ((stoi(inodeNum) < 32) ? fs.inodeBlock[0].getInodeData(stoi(inodeNum)) : fs.inodeBlock[1].getInodeData(stoi(inodeNum)));

		DataBlock dBlock ; // fs�� ���� �����´�
		str = "";

		for ( int i = 0 ; i < atoi(inode.blocks); i ++ )
		{
			str += dBlock.data[ inode.dataBlockList[i] ]; // ��ϸ���Ʈ�� �ִ� �����ͺ�ϸ���Ʈ
		}

		vector<string>& entry = *tokenize(str,";");
		for ( int j = 0; j < entry.size(); j ++ )
		{
			vector<string>& tmp = *tokenize(entry[j],",");

			if ( tmp[0] == arr[1] ) 
			{
				inodeNum = tmp[1];
				break;
			}

		}
	}
	return stoi(inodeNum);

}

Directory* DirectoryManager::returnDir(int in)
{
	//�� ���̳�� �ѹ��� ����Ǿ��ִ� ��� �����´�.
	//�� ������� �����ͺ�Ͽ� �����Ͽ� �����ͺ�� �����´�.
	//�� �����ͺ������ ���丮 ��ü�� �����. ( ��Ʈ���� )
	FileSystem& fs = *FileSystem::getInstance();
	Inode inode = ((in < 32) ? fs.inodeBlock[0].getInodeData(in) : fs.inodeBlock[1].getInodeData(in));
	if (strchr(inode.mode, 'f'))
		return NULL;
	int blocks = atoi(inode.blocks); //�ش� ������ ��� ��
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, 3);

	DataBlock* dBlock = new DataBlock[blocks];

	for ( int i = 0 ; i < blocks ; i ++ )
	{
		dBlock[i] = fs.dataBlocks[dataIdx[i]]; //dataBlock ����� 
	}
	string data = "";
	for( int i = 0; i < blocks; i++ )
	{   
		char* blockData;
		fs.readFS( dataIdx[ i ], blockData );
		data += blockData;
	}
	Directory* dr = new Directory;

	//1. ��Ʈ�� ����Ʈ �޾ƿ��� -> ������ "/"
	//vector<string>& eList = *tokenize(data,"/");
	//2. ��Ʈ�� �� �����ϱ� -> ������ ";"

	vector<Entry> vEntry;

	//3. ��Ʈ�� �Ӽ� �̾Ƴ��� -> ������ ","
	vector<string>& entry = *tokenize(data,";");
	for ( int j = 0; j < entry.size(); j ++ )
	{
		Entry en;
		vector<string>& tmp = *tokenize(entry[j],",");

		strcpy(en.name, tmp[0].c_str());
		en.inodeNum = stoi(tmp[1]);
		vEntry.push_back(en);
	}
	
	for ( int i = 0; i < vEntry.size(); i ++ )
	{
		dr->addDirectory(vEntry[i], in);
	}
	return dr;
}
void DirectoryManager::openAllDir(char * path)
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	PathManager& pm = *PathManager::getInstance();
	
	vector<string> vStr = *pm.getAllAbsPath(path);

		
	Directory dir = *returnDir(0); // root�� dir ��ü ��������
	int fd = tm.fileOpenEvent(0, fs.inodeBlock->getInodeData(0));
	openedDir_FDList.push_back(fd);

	for (int i = 1; i < vStr.size(); i++)
	{
		// ���� ���丮�� ���� ���� inodeNum�� Block�� ��´�.
		int inodeNum = dir.findName(stringToCharArr(vStr[i]))->inodeNum;
		Inode inodeBl = fs.inodeBlock->getInodeData(inodeNum);
		dir = Dir_Read(stringToCharArr(vStr[i]));

		if (tm.isExistInInodeTable(inodeNum))
		{
			cout << endl << "open�Ǿ��ִ� ���丮��" << endl;
			throw "error in DirectoryManager.cpp in allOpen Func";// fd = 0 -> �̹� ���µǾ��ִ� ���
		}

		fd = tm.fileOpenEvent(inodeNum, inodeBl);
		openedDir_FDList.push_back(fd);
		openedDirList.push_back(dir);
	}
}
void DirectoryManager::closeAllDir()
{
	TableManager& tm = *TableManager::getInstance();

	for (int i = 0; i < openedDir_FDList.size(); i++)
	{
		tm.fileCloseEvent(openedDir_FDList[i]);
	}

	openedDirList.clear();
	openedDir_FDList.clear();
}
DirectoryManager* DirectoryManager::instance = NULL;