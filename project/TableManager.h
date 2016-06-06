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
	int inode_number; // inode 블록에서의 inode number
	Inode inode;
};


const int TABLE_DEFAULT_MAX = 100;
enum TableType { FDT, INODET, SFT };
class TableManager
{
private:
	//유일한 Instance
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

	//Singleton Pattern을 위해 Constructor를 숨김
	TableManager() {
		initialize();
	}


	/*
	Class Methods
	*/

	//초기화
	void initialize();
	void tableInit(TableType type);
	void tableInit(TableType type, void* arr, int max);

	//Resize...
	void resize(TableType type);

	//빈 index 중 가장 작은 index 반환
	int getIndex(TableType type);

	


public:
	//할당한 모든 변수 반환
	//~TableManager();

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

	//Data 추가
	int addElement(TableType type, void* data);

	//Data 삭제
	//return 값 : 삭제 성공 여부
	bool removeElement(TableType type, int index);

	//파일포인터 업데이트
	void updateFilePointer(int fdIdx, int filePointer);

	//Inode 정보 업데이트
	void updateInode(int fdIdx, Inode inode);

	//fd를 통해 inode number를 return 해주는 함수
	int getInodeNumByFD(int fd);

	//FileOpen에 대응하는 함수
	int fileOpenEvent(int inodeNumber, Inode inodeBlock);

	//FileClose에 대응하는 함수
	bool fileCloseEvent(int fd);

	//getter & setter
	void* getElement(TableType type, int index);

	//inode 번호를 통해 해당 inode 번호가 inode 테이블에 있는지 알려주는 함수
	bool isExistInInodeTable(int inodeNum);

	//count 반환
	int getCount(TableType type);
	
	void getTableInfo(TableType type, int* idxList, void* result);
};