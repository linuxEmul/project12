#pragma once
#include "TableManager.h"
#include "SuperBlock.h"
#include "FileSystem.h"
#include "BlockDescriptorTable.h"
#include "InodeBlock.h"

#include <iostream>
class Display
{
public:

	//테이블 정보 출력
	void displayFileDiscriptorTable();
	void displaySystemFileTable();
	void displayInodeTable();
	
	//블록 정보 출력 : 출력할 블록 번호를 입력받고 해당 블록의 정보를 출력
	// FS_Read() 함수를 이용하여 구현한다.

	void displaySuperBlock(); //블록 0번
	void displayBlockDescriptorTable(); //블록 1번

	void displayInodeBlock(); //블록4번

	void printBlockNum(int bNum);
};