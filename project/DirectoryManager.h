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
};
