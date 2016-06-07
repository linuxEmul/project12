#include "DirectoryManager.h"

#include "PathManager.h"
#include "InodeBlock.h"
#include "DataBlock.h"

void DirectoryManager::Dir_Create(char* direc)
{
	try {
		//Bitmap 갱신 함수를 통해 비트맵 갱신
		string aaa = direc;

		Inode inode;

		// inode 설정
		//비트맵들 1로 설정 ..

		inode.mode = "d777";

		// int EmptyDataNum = dBlockBitmap.getEmpty();
		// int EmptyInodeNum = inodeBitmap.getEmpty();


		// 절대 경로 분석
		PathManager& pm = *PathManager::getInstance();
		vector<string> arr;

		pm.doAnalyzeFolder(direc, arr);
		

		int n = arr.size(); // n: 경로상의 폴더 개수?

		string currDir = arr[n - 1]; // 현재디렉토리 이름(내가 만들 디렉토리)
		string topDir = ""; // 상위디렉토리 이름

		if (strcmp(direc, "/") == 0)
			topDir = arr[n - 1]; // 현재디렉토리 이름(내가 만들 디렉토리)
		else
			topDir = arr[n - 2];
		//addDirectory 

		int currDirInode = -1;
		int topDirInode = -1;
		
		if(aaa == "/")
			topDirInode = returnInodeNum((char*)topDir.c_str());
		else
			topDirInode = returnInodeNum(stringToCharArr((*pm.getAllAbsPath(direc))[n - 2]));


		FileSystem& fs = *FileSystem::getInstance();

		//memcpy(dB.data, (void *)content.c_str, sizeof(content)); 


		Directory curDr;
		Directory topDr;

		char time[13];
		getCurrentTime(time);

		char linkCount[2] = "2";

		//Inode 정보 설정
		inode.blocks = "0";
		inode.linksCount = linkCount;
		inode.mtime = time;
		inode.size = "0";
		inode.time = time;
		inode.ctime = time;


		currDirInode = fs.writeFS_Dir(inode);

		if (strcmp(direc, "/") != 0 && topDr.isExist((char*)currDir.c_str()) == true)
		{
			cout << "dir exist" << endl;
			return;
		}//디렉토리 중복 검사

		curDr.setInodeNum(currDirInode, topDirInode);

		if (strcmp(direc, "/") != 0)
		{
			topDr = *returnDir(topDirInode);
			Entry e;

			e.inodeNum = currDirInode;
			strcpy(e.name, currDir.c_str());
			topDr.addDirectory(e, topDirInode);
		}
		Entry *enList = curDr.entryList;

		string content = ".,";

		//cout << enList[0].inodeNum << endl;
		content.append(to_string(enList[0].inodeNum));
		content.append(";..," + to_string(enList[1].inodeNum));
		content.append(";");
		//데이터블록에 데이터 추가(idx는 datablock Index)
		int idx = fs.writeFS((char*)content.c_str());
		char dataBlockList[] = "   \0";
		itoa(idx, dataBlockList);

		char size[2] = { '0' + content.length()};
		//char dataBlockList = number;

		//Inode 정보 설정
		inode.blocks = "1";
		inode.linksCount = linkCount;
		inode.mtime = time;
		inode.size = size;
		inode.time = time;
		inode.dataBlockList = dataBlockList;

		//데이터 블록 추가 후 업데이트
		fs.updateInode_writeFile(currDirInode, inode);
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
			throw "파일을 입력했습니다.";
		else {
			Directory dir = *returnDir(accessNum);
			//openedDirList.push_back(dir);
			//openedDirList.erase(openedDirList.begin+5);
			return dir;
		}
	}
	catch (char* msg)
	{
		if (strcmp(direc, "파일을 입력했습니다.") == 0)
			throw msg;
		cerr << "error : " << msg << endl;
	}
}

void DirectoryManager::Dir_Unlink(char* direc)
{
	/*
	direc이 나타내는 디렉토리를 삭제.
	해당 디렉토리를 포함하는 상위 디렉토리에서도 관련 내용 삭제
	data block, inode도 삭제, 관련 bitmap도 0으로 설정
	디렉토리에 파일이 존재하면 에러 메시지 출력
	루트 디렉토리는 삭제 불가
	*/

	Directory dr = Dir_Read(direc);

	if (dr.entryCnt == 2)
	{
		int topInodeNum = dr.findName("..")->inodeNum;
		int currInodeNum = dr.findName(".")->inodeNum;

		// 아이노드 번호가 시스템파일 테이블에 없으면 파일 Unlink과정을 한다

		// 속하는 상위 디렉토리의 엔트리에 해당 파일과 아이노드번호 삭제
		Directory d = *returnDir(topInodeNum);
		d.rmDirectory(currInodeNum, topInodeNum);

		FileSystem& fs = *FileSystem::getInstance();

		// InodeBitmap의 해당 비트 0으로 set
		// BlockBitmap의 dataIdx 에 해당하는 비트 0으로 set
		// InodeBlock에서 아이노드 번호에 해당하는 아이노드 삭제
		Inode inodeData = fs.inodeBlock->getInodeData(currInodeNum);
		int blocks = atoi(inodeData.blocks);
		int* dataIdx = new int[blocks];
		translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

		fs.resetDataBlock(dataIdx, blocks);

		fs.writeFS(currInodeNum);
	}
}

int DirectoryManager::returnInodeNum(char * direc)
{
	// 절대 경로에 속한 것들이 오픈되어야  ...
	// 벡터로 반환받을 것들 차례대로 다 오픈

	if (strcmp(direc, "/") == 0)
		return 0;

	// Root부터 차례로 탐색
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
		str += fs.dataBlocks[idx[i] - 6].getDataBlockData(); // 블록리스트에 있는 데이터블록리스트
	}

	//2. 엔트리 간 구분하기 -> 구분자 ";"
	//3. 엔트리 속성 뽑아내기 -> 구분자 ","
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
		//cout << "넘어온 경로 : " << direc << ", ";
		throw "inodeNumber를 구할 수 없습니다.";
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
		//cout << "넘어온 경로 : " << direc << ", ";
		throw "inodeNumber를 구할 수 없습니다.";
	}
	return stoi(newInodeNum);

}

Directory* DirectoryManager::returnDir(int in)
{
	//이 아이노드 넘버가 저장되어있는 블락 가져온다.
	//이 블락으로 데이터블록에 접근하여 데이터블록 가져온다.
	//이 데이터블록으로 디렉토리 객체를 만든다. ( 엔트리들 )
	FileSystem& fs = *FileSystem::getInstance();
	Inode inode = fs.readFS(in);
	if (strchr(inode.mode, 'f'))
		return NULL;
	int blocks = atoi(inode.blocks); //해당 파일의 블록 수
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inode.dataBlockList, dataIdx, 3);

	DataBlock* dBlock = new DataBlock[blocks];

	for (int i = 0; i < blocks; i++)
	{
		dBlock[i] = fs.dataBlocks[dataIdx[i]-6]; //dataBlock 내용들 
	}
	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char blockData[BLOCK_SIZE];
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}

	Directory* dr = new Directory;

	//1. 엔트리 리스트 받아오기 -> 구분자 "/"
	//vector<string>& eList = *tokenize(data,"/");
	//2. 엔트리 간 구분하기 -> 구분자 ";"


	// Trimming
	string str = data;
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);


	vector<Entry> vEntry;

	//3. 엔트리 속성 뽑아내기 -> 구분자 ","
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


	Directory dir = *returnDir(0); // root의 dir 객체 가져오기
	int fd = tm.fileOpenEvent(0, fs.inodeBlock->getInodeData(0));
	openedDir_FDList.push_back(fd);

	for (int i = 1; i < vStr.size(); i++)
	{
		// 상위 디렉토리를 통해 먼저 inodeNum과 Block을 얻는다.
		int inodeNum = dir.findName(stringToCharArr(vAllDirec[i]))->inodeNum;
		Inode inodeBl = fs.inodeBlock->getInodeData(inodeNum);
		dir = Dir_Read(stringToCharArr(vStr[i]));

		if (tm.isExistInInodeTable(inodeNum))
		{
			cout << endl << "open되어있는 디렉토리임" << endl;
			throw "error in DirectoryManager.cpp in allOpen Func";// fd = 0 -> 이미 오픈되어있는 경우
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
		cout << "i : " << i << endl;
		return true;
	}
	catch (char* msg) {
		//cout << msg << endl;
		return false;
	}
}
DirectoryManager* DirectoryManager::instance = NULL;