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
	
	// �����θ� �Ѱ��ָ� ���̳�� �ѹ� ��ȯ���ִ� �Լ�.
	int returnInodeNum(char* direc);
	
	// ���̳���ȣ�� ���ؼ� ���丮 ��ü�� ��ȯ�ϴ� �Լ�.
	// �� �Լ��� �� �� try - catch �ؾ� ��
	// catch����� �ϴ� �� char* ������ �޽���
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

	//���� �ֱٿ� ���� Directory ��ȯ �Լ�
	Directory getRecentlyOpenedDir() {	return openedDirList[openedDirList.size() - 1]; }
	
	//��� �� ��� ���丮 ����
	void openAllDir(char* path);

	//��� �� ��� ���丮 �ݱ�
	void closeAllDir();

	/*
		�ܺη� ���� �� �ִ� �Լ�
	*/

	//�⺻ ���丮 ���� �Լ�
	void makeDefaultDirectory();
};
