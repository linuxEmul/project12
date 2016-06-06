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

	//���̺� ���� ���
	void displayFileDiscriptorTable();
	void displaySystemFileTable();
	void displayInodeTable();
	
	//��� ���� ��� : ����� ��� ��ȣ�� �Է¹ް� �ش� ����� ������ ���
	// FS_Read() �Լ��� �̿��Ͽ� �����Ѵ�.

	void displaySuperBlock(); //��� 0��
	void displayBlockDescriptorTable(); //��� 1��

	void displayInodeBlock(); //���4��

	void printBlockNum(int bNum);
};