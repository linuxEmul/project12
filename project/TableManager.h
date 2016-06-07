#pragma once
#include <iostream>
#include "InodeBlock.h"
#include <vector>
using namespace std;

struct SFTElement
{
	int inodeTableIdx;
	int file_pointer;
};
struct InodeElement
{
	int inode_number; // inode ��Ͽ����� inode number
	Inode inode;
};


const int TABLE_DEFAULT_MAX = 100;
enum TableType { FDT, INODET, SFT };
class TableManager
{
private:
	//������ Instance
	static TableManager* instance;
	

	//Tables
	int* fileDescriptorTable;
	InodeElement* inodeTable;
	SFTElement* systemFileTable;

	//Table Elements Last Index Member
	int fdtLastIndex;
	int inodeLastIndex;
	int sftLastIndex;

	//Table Elements Max Size
	int fdtMax;
	int inodeMax;
	int sftMax;

	//Singleton Pattern�� ���� Constructor�� ����
	TableManager() {
		initialize();
	}


	/*
	Class Methods
	*/

	//�ʱ�ȭ
	void initialize();
	void tableInit(TableType type);
	void tableInit(TableType type, void* arr, int max);

	//Resize...
	void resize(TableType type);

	//�� index �� ���� ���� index ��ȯ
	int getIndex(TableType type);

	


public:
	//������ ����Ѵ�
	static bool allowDelete;

	//�Ҵ��� ��� ���� ��ȯ
	~TableManager();

	//Static Members
	static TableManager* getInstance() {
		if (!instance) {
			instance = new TableManager();
		}
		return instance;
	}

	static TableManager getTmpInstance() {
		TableManager tmp;
		return tmp;
	}


	/*
	Class Methods
	*/

	//Data �߰�
	int addElement(TableType type, void* data);

	//Data ����
	//return �� : ���� ���� ����
	bool removeElement(TableType type, int index);

	//���������� ������Ʈ
	void updateFilePointer(int fdIdx, int filePointer);

	//Inode ���� ������Ʈ
	void updateInode(int fdIdx, Inode inode);

	//fd�� ���� inode number�� return ���ִ� �Լ�
	int getInodeNumByFD(int fd);

	InodeElement* getInodeByFD(int fd);

	//FileOpen�� �����ϴ� �Լ�
	int fileOpenEvent(int inodeNumber, Inode inodeBlock);

	//FileClose�� �����ϴ� �Լ�
	bool fileCloseEvent(int fd);

	//getter & setter
	void* getElement(TableType type, int index);

	//inode ��ȣ�� ���� �ش� inode ��ȣ�� inode ���̺� �ִ��� �˷��ִ� �Լ�
	bool isExistInInodeTable(int inodeNum);

	//count ��ȯ
	int getCount(TableType type);
	
	int* getTableInfo(TableType type, void* result);
	
};