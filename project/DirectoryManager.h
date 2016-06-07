#pragma once
#include "Directory.h"
#include "FileSystem.h"
#include "CommonLibrary.h"
#include "TableManager.h"
#include <cstring>

class DirectoryManager
{
private:
	static DirectoryManager* instance;
	vector<Directory> openedDirList;
	vector<int> openedDir_FDList;

	DirectoryManager() {

	}
public:
	void Dir_Create(char* direc);
	Directory Dir_Read(char* direc);
	void Dir_Unlink(char* direc);
	
	// 절대경로를 넘겨주면 아이노드 넘버 반환해주는 함수.
	int returnInodeNum(char* direc);
	
	// 아이노드번호를 통해서 디렉토리 객체를 반환하는 함수.
	// 이 함수를 쓸 때 try - catch 해야 함
	// catch해줘야 하는 건 char* 형식의 메시지
	Directory* returnDir(int in);

	static DirectoryManager* getInstance() {
		if (!instance)
			instance = new DirectoryManager();
		return instance;
	}
	static DirectoryManager getTmpInstance() {
		DirectoryManager tmp;
		return tmp;
	}

	int getOpenedDirCount() { return openedDirList.size(); }
	vector<Directory> getOpenedDirList() { return openedDirList; }

	//가장 최근에 열린 Directory 반환 함수
	Directory getRecentlyOpenedDir() {	return openedDirList[openedDirList.size() - 1]; }
	
	//경로 상 모든 디렉토리 열기
	void openAllDir(char* path);

	//경로 상 모든 디렉토리 닫기
	void closeAllDir();

	/*
		외부로 빼낼 수 있는 함수
	*/

	//기본 디렉토리 생성 함수
	void makeDefaultDirectory();

	bool isReallyExist(char* path);

	void changeDirMode(char* path, char* mode) 
	{
		FileSystem& fs = *FileSystem::getInstance();
		TableManager& tm = *TableManager::getInstance();

		DirectoryManager& dm = *DirectoryManager::getInstance();

		// 디렉토리 이름으로 파일 디스크립터에서 디렉토리 인덱스 받아오기 
		int dI =  dm.returnInodeNum(path);

		Inode dInodeData = fs.inodeBlock->getInodeData(dI);

		dInodeData.mode = new char[5];

		for (int i = 0; i < 5; i++)
			dInodeData.mode[i] = mode[i];

		if(tm.isExistInInodeTable(dI))
			tm.updateInode(dI, dInodeData);
		fs.updateInode_writeFile(dI, dInodeData);
	}
};
