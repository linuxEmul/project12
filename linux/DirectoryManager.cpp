#include "DirectoryManager.h"
#include "File.h"
#include "PathManager.h"
#include "InodeBlock.h"
#include "DataBlock.h"


// Test �뵵
PathStack ppp;

void DirectoryManager::Dir_Create(char* direc)
{
	try {
		//Bitmap ���� �Լ��� ���� ��Ʈ�� ����
		string path = direc;

		Inode inode;
		
		// inode ����
		//��Ʈ�ʵ� 1�� ���� ..
		
		inode.mode = "d777";

		// int EmptyDataNum = dBlockBitmap.getEmpty();
		// int EmptyInodeNum = inodeBitmap.getEmpty();

		ppp.push("DC");
		// ���� ��� �м�
		PathManager& pm = *PathManager::getInstance();
		vector<string> arr;

		pm.doAnalyzeFolder(direc, arr);
		

		int n = arr.size(); // n: ��λ��� ���� ����?

		string currDir = arr[n - 1]; // ������丮 �̸�(���� ���� ���丮)
		string topDir = ""; // �������丮 �̸�

		if (strcmp(direc, "/") == 0)
			topDir = arr[n - 1]; // ������丮 �̸�(���� ���� ���丮)
		else
			topDir = arr[n - 2];
		//addDirectory 

		int currDirInode = -1;
		int topDirInode = -1;
		
		if(path == "/")
			topDirInode = returnInodeNum((char*)topDir.c_str());
		else
			topDirInode = returnInodeNum(stringToCharArr((*pm.getAllAbsPath(direc))[n - 2]));


		FileSystem& fs = *FileSystem::getInstance();

		//memcpy(dB.data, (void *)content.c_str, sizeof(content)); 


		Directory curDr;
		Directory topDr;

		char time[13];
		getCurrentTime(time);

		char linkCount[2] = "0";

		//Inode ���� ����
		inode.blocks = "1";
		inode.linksCount = linkCount;
		inode.mtime = time;
		inode.size = "0";
		inode.time = time;
		inode.ctime = time;

		if (strcmp(direc, "/") != 0 && topDr.isExist((char*)currDir.c_str()) == true)
		{
			cout << "dir exist" << endl;
			return;
		}//���丮 �ߺ� �˻�
		ppp.push("2");
		currDirInode = fs.writeFS_Dir(inode);
		
		ppp.push("3");
		


		if (strcmp(direc, "/") != 0)
		{
			topDr = *returnDir(topDirInode);
			Entry e;
			ppp.push("4");
			e.inodeNum = currDirInode;
			strcpy(e.name, currDir.c_str());
			topDr.addDirectory(e, topDirInode);
			ppp.push("5");
		}
		curDr.setInodeNum(currDirInode, topDirInode);
		Entry *enList = curDr.entryList;

		string content = ".,";

		//cout << enList[0].inodeNum << endl;
		content.append(to_string(enList[0].inodeNum));
		content.append(";..," + to_string(enList[1].inodeNum));
		content.append(";");
		//�����ͺ��Ͽ� ������ �߰�(idx�� datablock Index)
		
		//�Ҵ���� ���� �޾ƿ���
		char* tmpBlockList = fs.inodeBlock->getDataBlockList(currDirInode);
		int idx = atoi(tmpBlockList);

		//�Ҵ���� ���� ����
		fs.resetDataBlock(&idx, 1);
		
		//�ٽ� ����
		int assignedIdx = fs.writeFS((char*)content.c_str());
		char dataBlockList[] = "   \0";
		_itoa(assignedIdx, dataBlockList);

		char size[4];
		memcpy(size, stringToCharArr(to_string(content.length())), strlen(stringToCharArr(to_string(content.length())))+1);

		//��ũ ���� �ٽ� �о�;� ��
		inode = fs.readFS(currDirInode);

		//Inode ���� ����
		getCurrentTime(time);

		inode.blocks = "1";
		inode.mtime = time;
		inode.size = size;
		inode.dataBlockList = dataBlockList;

		ppp.push("6");
		//������ ���� �߰� �� ������Ʈ
		fs.updateInode_writeFile(currDirInode, inode);
		ppp.push("7");
	}
	catch (char* msg)
	{
		cerr << "error : " << msg << endl;
	}
}

Directory DirectoryManager::Dir_Read(char* direc)
{
	try {
		FileSystem& fs = *FileSystem::getInstance();
		int accessNum = returnInodeNum(direc);
		InodeBlock inode = ((accessNum > 31) ? fs.inodeBlock[1] : fs.inodeBlock[0]);
		char* mode = inode.getMode(accessNum);
		if (strchr(mode, 'f'))
			throw "������ �Է��߽��ϴ�.";
		else {
			Directory dir = *returnDir(accessNum);
			//openedDirList.push_back(dir);
			//openedDirList.erase(openedDirList.begin+5);
			return dir;
		}
	}
	catch (char* msg)
	{
		if (strcmp(direc, "������ �Է��߽��ϴ�.") == 0)
			throw msg;
		cerr << "error : " << msg << endl;
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

	if (dr.entryCnt == 2)
	{
		int topInodeNum = dr.findName("..")->inodeNum;
		int currInodeNum = dr.findName(".")->inodeNum;

		// ���̳�� ��ȣ�� �ý������� ���̺��� ������ ���� Unlink������ �Ѵ�

		// ���ϴ� ���� ���丮�� ��Ʈ���� �ش� ���ϰ� ���̳���ȣ ����
		Directory d = *returnDir(topInodeNum);
		d.rmDirectory(currInodeNum, topInodeNum);

		FileSystem& fs = *FileSystem::getInstance();

		// InodeBitmap�� �ش� ��Ʈ 0���� set
		// BlockBitmap�� dataIdx �� �ش��ϴ� ��Ʈ 0���� set
		// InodeBlock���� ���̳�� ��ȣ�� �ش��ϴ� ���̳�� ����
		Inode inodeData = fs.inodeBlock->getInodeData(currInodeNum);
		int blocks = atoi(inodeData.blocks);
		int* dataIdx = new int[blocks];
		translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

		fs.resetDataBlock(dataIdx, blocks);

		fs.writeFS(currInodeNum);
	}
	else
		throw "���丮�� ������� �ʽ��ϴ�.";
}

void DirectoryManager::Dir_Unlink_All(char * direc)
{
	Directory dr = Dir_Read(direc);

	if (dr.entryCnt != 2)
	{
		for (int i = 0; i < dr.entryCnt; i++)
		{
			string path = direc;
			path.append("/");
			path.append(dr.entryList[i].name);

			if (isFile(dr.entryList[i].name, dr) == 'f')
			{
				File f;
				f.removeFile(dr.entryList[i].name);
			}
			else
				Dir_Unlink_All(stringToCharArr(path));
		}
	}
	Dir_Unlink(direc);
}

int DirectoryManager::returnInodeNum(char * direc)
{
	// ���� ��ο� ���� �͵��� ���µǾ��  ...
	// ���ͷ� ��ȯ���� �͵� ���ʴ�� �� ����

	if (strcmp(direc, "/") == 0)
		return 0;

	// Root���� ���ʷ� Ž��
	PathManager& pm = *PathManager::getInstance();
	vector<string> arr;

	pm.doAnalyzeFolder(direc, arr);

	FileSystem& fs = *FileSystem::getInstance();

	Inode inodeData = fs.inodeBlock[0].getInodeData(0);
	string str = "";

	int* idx = new int[atoi(inodeData.blocks)];
	translateCharArrToIntArr(inodeData.dataBlockList, idx, atoi(inodeData.blocks));
	for (int i = 0; i < atoi(inodeData.blocks); i++)
	{
		str += fs.dataBlocks[idx[i] - 6].getDataBlockData(); // ���ϸ���Ʈ�� �ִ� �����ͺ��ϸ���Ʈ
	}

	//2. ��Ʈ�� �� �����ϱ� -> ������ ";"
	//3. ��Ʈ�� �Ӽ� �̾Ƴ��� -> ������ ","
	string inodeNum;

	vector<string>& entry = *tokenize(str, ";");
	for (int j = 0; j < entry.size(); j++)
	{
		vector<string>& tmp = *tokenize(entry[j], ",");

		if (tmp[0] == arr[1])
		{
			inodeNum = tmp[1];
			break;
		}

	}
	if (inodeNum == "") {
		//cout << "�Ѿ�� ��� : " << direc << ", ";
		throw "inodeNumber�� ���� �� �����ϴ�.";
	}
	string newInodeNum = "";
	if (arr.size()-1 < 2)
		newInodeNum = inodeNum;
	for (int i = 1; i < arr.size() - 1; i++)
	{
		if(i == 1)
			inodeData = ((stoi(inodeNum) < 32) ? fs.inodeBlock[0].getInodeData(stoi(inodeNum)) : fs.inodeBlock[1].getInodeData(stoi(inodeNum)));
		else
			inodeData = ((stoi(newInodeNum) < 32) ? fs.inodeBlock[0].getInodeData(stoi(newInodeNum)) : fs.inodeBlock[1].getInodeData(stoi(newInodeNum)));
		str = "";

		idx = new int[atoi(inodeData.blocks)];
		translateCharArrToIntArr(inodeData.dataBlockList, idx, atoi(inodeData.blocks));
		for (int i = 0; i < atoi(inodeData.blocks); i++)
		{
			str += fs.dataBlocks[idx[i] - 6].getDataBlockData();
		}

		vector<string>& entry = *tokenize(str, ";");
		for (int j = 0; j < entry.size(); j++)
		{
			vector<string>& tmp = *tokenize(entry[j], ",");

			if (tmp[0] == arr[i+1])
			{
				newInodeNum = tmp[1];
				break;
			}
			if (j == entry.size() - 1)
				newInodeNum = "";
		}
	}
	if (newInodeNum == "") {
		//cout << "�Ѿ�� ��� : " << direc << ", ";
		throw "inodeNumber�� ���� �� �����ϴ�.";
	}
	return stoi(newInodeNum);

}

Directory* DirectoryManager::returnDir(int in)
{
	//�� ���̳�� �ѹ��� ����Ǿ��ִ� ���� �����´�.
	//�� �������� �����ͺ��Ͽ� �����Ͽ� �����ͺ��� �����´�.
	//�� �����ͺ������� ���丮 ��ü�� �����. ( ��Ʈ���� )
	FileSystem& fs = *FileSystem::getInstance();
	Inode inode = fs.readFS(in);
	if (strchr(inode.mode, 'f'))
		return NULL;
	int blocks = atoi(inode.blocks); //�ش� ������ ���� ��
	int dataIdx[20];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, blocks);

	DataBlock* dBlock = new DataBlock[blocks];

	for (int i = 0; i < blocks; i++)
	{
		dBlock[i] = fs.dataBlocks[dataIdx[i]-6]; //dataBlock ����� 
	}
	string data = "";
	cout << "blocks " << blocks << endl;
	for (int i = 0; i < blocks; i++)
	{
		char blockData[BLOCK_SIZE];
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	Directory* dr = new Directory;

	//1. ��Ʈ�� ����Ʈ �޾ƿ��� -> ������ "/"
	//vector<string>& eList = *tokenize(data,"/");
	//2. ��Ʈ�� �� �����ϱ� -> ������ ";"


	// Trimming
	string str = data;
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);


	vector<Entry> vEntry;

	//3. ��Ʈ�� �Ӽ� �̾Ƴ��� -> ������ ","
	vector<string>& entry = *tokenize(str.c_str(), ";");
	for (int j = 0; j < entry.size(); j++)
	{
		Entry en;
		vector<string>& tmp = *tokenize(entry[j], ",");

		strcpy(en.name, tmp[0].c_str());
		en.inodeNum = stoi(tmp[1]);
		vEntry.push_back(en);
	}

	for (int i = 0; i < vEntry.size(); i++)
	{
		dr->addDirectory(vEntry[i]);
	}
	return dr;
}
void DirectoryManager::openAllDir(char * path)
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();
	PathManager& pm = *PathManager::getInstance();

	vector<string> vStr = *pm.getAllAbsPath(path);
	vector<string> vAllDirec = pm.doAnalyzeFolder(path);


	Directory dir = *returnDir(0); // root�� dir ��ü ��������
	int fd = tm.fileOpenEvent(0, fs.inodeBlock->getInodeData(0));
	openedDir_FDList.push_back(fd);

	for (int i = 1; i < vStr.size(); i++)
	{
		// ���� ���丮�� ���� ���� inodeNum�� Block�� ��´�.
		Entry* en = dir.findName(stringToCharArr(vAllDirec[i]));
		if (!en) {
			cout << "Name : " << vAllDirec[i] << endl;
			throw "dir�� ��Ʈ������ name�� ã�� ����.";
		}
		int inodeNum = en->inodeNum;
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
void DirectoryManager::makeDefaultDirectory()
{
	char* pathList[] = { "/","/bin","/dev","/etc","/home","/lib","/var" };
	int count = 7;
	for (int i = 0; i < count; i++)
		Dir_Create(pathList[i]);
}
bool DirectoryManager::isReallyExist(char * path)
{
	try {
		int i = returnInodeNum(path);
		//cout << "i : " << i << endl;
		return true;
	}
	catch (char* msg) {
		//cout << msg << endl;
		return false;
	}
}
void DirectoryManager::changeDirMode(char* path, char* mode)
{
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	DirectoryManager& dm = *DirectoryManager::getInstance();

	// ���丮 �̸����� ���� ��ũ���Ϳ��� ���丮 �ε��� �޾ƿ��� 
	int dI = dm.returnInodeNum(path);

	Inode dInodeData = fs.inodeBlock->getInodeData(dI);

	dInodeData.mode = new char[5];

	for (int i = 0; i < 5; i++)
		dInodeData.mode[i] = mode[i];

	if (tm.isExistInInodeTable(dI))
		tm.updateInode(dI, dInodeData);
	fs.updateInode_writeFile(dI, dInodeData);
}
char DirectoryManager::isFile(char* filename, Directory currentDir)
{
	if (strcmp(filename, "/") == 0)
	{
		return 'd';
	}
	Entry* fileEntry = currentDir.findName(filename);
	if (fileEntry == NULL) {
		throw "������ �������� �ʽ��ϴ�.";
	}

	Directory* dir = returnDir(fileEntry->inodeNum);
	if (dir == NULL)
		return 'f';
	return 'd';
}
DirectoryManager* DirectoryManager::instance = NULL;
bool DirectoryManager::allowDelete = false;