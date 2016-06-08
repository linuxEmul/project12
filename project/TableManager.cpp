#include "TableManager.h"

void TableManager::initialize() {
	//index �ʱ�ȭ
	fdtLastIndex = -1;
	inodeLastIndex = -1;
	sftLastIndex = -1;

	//Pointer �ʱ�ȭ
	fileDescriptorTable = NULL;
	inodeTable = NULL;
	systemFileTable = NULL;

	//Max Value �ʱ�ȭ
	fdtMax = TABLE_DEFAULT_MAX;
	inodeMax = TABLE_DEFAULT_MAX;
	sftMax = TABLE_DEFAULT_MAX;

	fileDescriptorTable = new int[fdtMax];

	inodeTable = new InodeElement[inodeMax];
	systemFileTable = new SFTElement[sftMax];

	//-1�� �ʱ�ȭ
	tableInit(FDT);
	tableInit(INODET);
	tableInit(SFT);

	//Reserved
	fileDescriptorTable[0] = 0; // ǥ�� �Է�
	fileDescriptorTable[1] = 1; // ǥ�� ���
	fileDescriptorTable[2] = 2; // ǥ�� ����
	fdtLastIndex = 2;
}

void TableManager::tableInit(TableType type)
{
	if (type == FDT)
	{
		for (int i = 0; i < fdtMax; i++)
			fileDescriptorTable[i] = -1;
	}
	else if (type == INODET)
	{
		for (int i = 0; i < inodeMax; i++)
			inodeTable[i].inode_number = -1;
	}
	else if (type == SFT)
	{
		for (int i = 0; i < sftMax; i++)
			systemFileTable[i].inodeTableIdx = -1;
	}
	else {

	}
}

void TableManager::tableInit(TableType type, void * arr, int max)
{
	if (type == FDT)
	{
		int* tmp = (int*)arr;
		for (int i = 0; i < max; i++)
			tmp[i] = -1;
	}
	else if (type == INODET)
	{
		InodeElement* tmp = (InodeElement*)arr;
		for (int i = 0; i < max; i++)
			tmp[i].inode_number = -1;
	}
	else if (type == SFT)
	{
		SFTElement* tmp = (SFTElement*)arr;
		for (int i = 0; i < max; i++)
			tmp[i].inodeTableIdx = -1;
	}
	else {

	}
}

void TableManager::resize(TableType type) {

	if (type == FDT) {
		int* newFDT = new int[fdtMax * 2];
		tableInit(type, newFDT, fdtMax * 2);
		for (int i = 0; i <= fdtLastIndex; i++) {
			newFDT[i] = fileDescriptorTable[i];
		}
		delete[] fileDescriptorTable;
		fileDescriptorTable = newFDT;
		fdtMax *= 2;
	}
	else if (type == INODET)
	{
		InodeElement* newInodeTable = new InodeElement[inodeMax * 2];
		tableInit(type, newInodeTable, inodeMax * 2);
		for (int i = 0; i <= inodeLastIndex; i++) {
			newInodeTable[i] = inodeTable[i];
		}
		delete[] inodeTable;
		inodeTable = newInodeTable;
		inodeMax *= 2;
	}
	else if (type == SFT)
	{
		SFTElement* newSFT = new SFTElement[sftMax * 2];
		tableInit(type, newSFT, sftMax * 2);
		for (int i = 0; i <= sftLastIndex; i++) {
			newSFT[i] = systemFileTable[i];
		}
		delete[] systemFileTable;
		systemFileTable = newSFT;
		sftMax *= 2;
	}
	else {
		cout << "error in TableManager.cpp, resize Func" << endl;
	}
}

TableManager::~TableManager() {
	if (allowDelete) {
		if (instance)
			delete instance;
		if (systemFileTable)
			delete[] systemFileTable;
		if (fileDescriptorTable)
			delete[] fileDescriptorTable;
		if (inodeTable)
			delete[] inodeTable;
	}
}


int TableManager::addElement(TableType type, void* data)
{
	int index = -1;
	if (type == SFT) {
		if (getCount(type) + 1 == sftMax)
			resize(type);
		index = getIndex(type);
		if (index > sftLastIndex)
			sftLastIndex = index;
		systemFileTable[index] = *(SFTElement*)data;
		systemFileTable[index].file_pointer = 0;
	}
	else {
		int inputData = *(int*)data;
		switch (type) {
		case FDT:
			if (getCount(type) + 1 == fdtMax)
				resize(type);
			index = getIndex(type);
			if (index > fdtLastIndex)
				fdtLastIndex = index;
			fileDescriptorTable[index] = inputData;
			break;
		case INODET:
			if (getCount(type) + 1 == inodeMax)
				resize(type);
			index = getIndex(type);
			if (index > inodeLastIndex)
				inodeLastIndex = index;
			inodeTable[index] = *(InodeElement*)data;
			break;
		default:
			cout << "error in TableManager.cpp, Add Func" << endl;
			break;
		}
	}
	return index;
}

bool TableManager::removeElement(TableType type, int index)
{
	try {
		switch (type)
		{
		case FDT:
			if (index >= fdtLastIndex)
				throw "�������� �ʴ� FDT index";
			fileDescriptorTable[index] = -1;
			if (index == fdtLastIndex)
			{
				for (int i = fdtLastIndex - 1; i >= 0; i--)
				{
					if (fileDescriptorTable[i] != -1)
					{
						fdtLastIndex = i;
						return true;
					}
				}
				//�� �̻� �������� �����Ƿ�.
				fdtLastIndex = -1;
			}
			break;
		case INODET:
			if (index >= inodeLastIndex)
				throw "�������� �ʴ� INODE index";
			inodeTable[index].inode_number = -1;
			if (index == inodeLastIndex)
			{
				for (int i = inodeLastIndex - 1; i >= 0; i--)
				{
					if (inodeTable[i].inode_number != -1)
					{
						inodeLastIndex = i;
						return true;
					}
				}
				//�� �̻� �������� �����Ƿ�.
				inodeLastIndex = -1;
			}
			break;
		case SFT:
			if (index >= sftLastIndex)
				throw "�������� �ʴ� SFT index";
			systemFileTable[index].inodeTableIdx = -1;
			if (index == sftLastIndex)
			{
				for (int i = sftLastIndex - 1; i >= 0; i--)
				{
					if (systemFileTable[i].inodeTableIdx != -1)
					{
						sftLastIndex = i;
						return true;
					}
				}
				//�� �̻� �������� �����Ƿ�.
				sftLastIndex = -1;
			}
			break;
		default:
			break;
		}
		return true;
	}
	catch (char* msg)
	{
		cerr << "error : " << msg << endl;
	}
	return false;
}

void TableManager::updateFilePointer(int index, int filePointer)
{
	systemFileTable[fileDescriptorTable[index]].file_pointer = filePointer;
}

void TableManager::updateInode(int fdIdx, Inode inodeData)
{
	// 1. FDT index�� ����� SFT index�� ã��
	// 2. �ű⿡ ����� Inode Table index�� ã��
	// 3. Inode ����
	int* sftIndex = (int*)getElement(FDT, fdIdx);
	SFTElement* myelement = (SFTElement*)getElement(SFT, *sftIndex);
	int* inodeIndex = &myelement->inodeTableIdx;
	InodeElement* myinode = (InodeElement*)getElement(INODET, *inodeIndex);
	myinode->inode = inodeData;
}

int TableManager::getInodeNumByFD(int fd)
{
	int* sftIndex = (int*)getElement(FDT, fd);
	SFTElement* myelement = (SFTElement*)getElement(SFT, *sftIndex);
	int* inodeIndex = &myelement->inodeTableIdx;
	InodeElement* myinode = (InodeElement*)getElement(INODET, *inodeIndex);
	return myinode->inode_number;
}

InodeElement* TableManager::getInodeByFD(int fd)
{
	int* sftIndex = (int*)getElement(FDT, fd);
	SFTElement* myelement = (SFTElement*)getElement(SFT, *sftIndex);
	int* inodeIndex = &myelement->inodeTableIdx;
	InodeElement* myinode = (InodeElement*)getElement(INODET, *inodeIndex);
	return myinode;
}

int TableManager::getIndex(TableType type)
{
	if (type == FDT) {
		for (int i = 0; i < fdtMax; i++)
		{
			if (fileDescriptorTable[i] == -1)
				return i;
		}
		if (fdtLastIndex == -1)
			return 0;
		return -1;
	}
	else if (type == INODET)
	{
		for (int i = 0; i < inodeMax; i++)
		{
			if (inodeTable[i].inode_number == -1)
				return i;
		}
		if (inodeLastIndex == -1)
			return 0;
		return -1;
	}
	else if (type == SFT)
	{
		for (int i = 0; i < sftMax; i++)
		{
			if (systemFileTable[i].inodeTableIdx == -1)
				return i;
		}
		if (sftLastIndex == -1)
			return 0;
		return -1;
	}
	else
		cout << "�߸��� �Է�" << endl;
	return -1;
}

int TableManager::getCount(TableType type)
{
	int count = 0;
	switch (type)
	{
	case FDT:
		if (fdtLastIndex == -1)
			break;
		for (int i = 0; i <= fdtLastIndex; i++)
		{
			if (fileDescriptorTable[i] == -1)
				count++;
		}
		count = fdtLastIndex - count + 1;
		break;
	case INODET:
		if (inodeLastIndex == -1)
			break;
		for (int i = 0; i <= inodeLastIndex; i++)
		{
			if (inodeTable[i].inode_number == -1)
				count++;
		}
		count = inodeLastIndex - count + 1;
		break;
	case SFT:
		if (sftLastIndex == -1)
			break;
		for (int i = 0; i <= sftLastIndex; i++)
		{
			if (systemFileTable[i].inodeTableIdx == -1)
				count++;
		}
		count = sftLastIndex - count + 1;
		break;
	default:

		break;
	}
	return count;
}

int* TableManager::getTableInfo(TableType type, void* result)
{
	int cnt = 0;
	string tmp = "";
	int* indexList = NULL;
	switch (type)
	{
	case FDT:
		indexList = new int[getCount(FDT)-3];
		for (int i = 3; i <= fdtLastIndex; i++) {
			if (fileDescriptorTable[i] != -1) {
				indexList[cnt++] = i;
				tmp = to_string(fileDescriptorTable[i]);
				(*reinterpret_cast<vector<string>*>(result)).push_back(tmp);
			}
		}
		break;
	case SFT:
		indexList = new int[getCount(SFT)];
		//result = new vector<SFTElement>;
		for (int i = 0; i <= sftLastIndex; i++) {
			if (systemFileTable[i].inodeTableIdx != -1) {
				indexList[cnt++] = i;
				(*reinterpret_cast<vector<SFTElement>*>(result)).push_back(systemFileTable[i]);
			}
		}
		break;
	case INODET:
		indexList = new int[getCount(INODET)];
		//result = new vector<InodeElement>;
		for (int i = 0; i <= fdtLastIndex; i++) {
			if (fileDescriptorTable[i] != -1) {
				indexList[cnt++] = i;
				(*reinterpret_cast<vector<InodeElement>*>(result)).push_back(inodeTable[i]);
			}
		}
		break;
	}
	return indexList;
}

int TableManager::fileOpenEvent(int inodeNumber, Inode inodeBlock) {
	// 1. �� ������ �ִ� ���丮�� ��Ʈ���� ���� ��
	// 2. ���ϸ����� ��Ʈ���� �˻��Ͽ� inode��ȣ�� ã��
	//DirectoryManager Ȥ�� DirectoryEntryManager �̿�
	//TableManager �ܿ��� ó������� �Ұ�

	// 3. Inode Blocks���� �ش� Inode ������ Inode ���̺� ����
	InodeElement* inode = new InodeElement();
	inode->inode_number = inodeNumber;
	inode->inode = inodeBlock;
	int inodeTableIndex = addElement(INODET, inode);

	// 4. System File Table�� inodeNumber�� filePointer = 0 ����
	SFTElement* sftElement = new SFTElement();
	sftElement->inodeTableIdx = inodeTableIndex;
	sftElement->file_pointer = 0;
	int sftIndex = addElement(SFT, sftElement);
	//cout << "sftIndex : " << sftIndex << endl;
	// 5. File Descriptor Table�� System File Table�� Index ����
	int fd = addElement(FDT, &sftIndex);

	// 6. �ش� FDT index�� return(�̰� fd ��ȣ)
	return fd;
}

bool TableManager::fileCloseEvent(int fd) {
	// 1. FDT index�� ����� SFT index�� ã��
	// 2. �ű⿡ ����� Inode Table index�� ã��
	// 3. �ű⿡ �ִ� ��� ������ �����ϰ�
	// 4. SFT index�� �ִ� ������ ��� ����
	// 5. FDT index�� �ִ� ������ ����
	int* sftIndex = (int*)getElement(FDT, fd);
	SFTElement* myelement = (SFTElement*)getElement(SFT, *sftIndex);
	int* inodeIndex = &myelement->inodeTableIdx;
	InodeElement* myinode = (InodeElement*)getElement(INODET, *inodeIndex);
	//myinode->inode = NULL;

	//last index �ٿ��ֱ�
	if (*sftIndex == sftLastIndex)
	{
		do {
			sftLastIndex--;
		} while (-1 < sftLastIndex && systemFileTable[sftLastIndex].inodeTableIdx == -1);
	}
	if ((*myelement).inodeTableIdx == inodeLastIndex)
	{
		do {
			inodeLastIndex--;
		} while (-1 < inodeLastIndex && inodeTable[inodeLastIndex].inode_number == -1);
	}
	if (fd == fdtLastIndex)
	{
		do {
			fdtLastIndex--;
		} while (-1 < fdtLastIndex && fileDescriptorTable[fdtLastIndex] == -1);
	}

	myinode->inode_number = -1;
	*inodeIndex = -1;
	myelement->inodeTableIdx = -1;
	myelement->file_pointer = 0;
	*sftIndex = -1;
	return true;
}

void* TableManager::getElement(TableType type, int index)
{
	switch (type)
	{
	case FDT:
		return &fileDescriptorTable[index];
		break;
	case INODET:
		return &inodeTable[index];
		break;
	case SFT:
		return &systemFileTable[index];
		break;
	default:

		break;
	}
	return NULL;
}
bool TableManager::isExistInInodeTable(int inodeNum)
{
	for (int i = 0; i <= inodeLastIndex; i++)
	{
		if (inodeTable[i].inode_number == inodeNum)
			return true;
	}
	return false;
}
void * TableManager::getAllElement(TableType type, int index)
{
	switch (type) {
	case FDT:
		return fileDescriptorTable;
		break;
	case INODET:
		return inodeTable;
		break;
	case SFT:
		return systemFileTable;
		break;
	default:

		break;
	}
	return NULL;
}
TableManager* TableManager::instance = NULL;
bool TableManager::allowDelete = false;