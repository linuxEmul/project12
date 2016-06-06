#include "FileSystem.h"

struct Entry
{
	char name[255]; // ��Ʈ�� �̸�
	int inodeNum; // inode number
	//int mode; // ���� ���丮 1, ���� 2
};

class Directory
{
public:
	Directory();

	Entry entryList[20]; // 20 ??
	int entryCnt;

	void setInodeNum(int cur, int top); //���丮 ���鶧 ȣ��. �ʱ�ȭ ����.

	void addDirectory(Entry entry, int inodeNum);
	void rmDirectory(int inodeN);
	Entry* findName( char *);

	bool isExist(char *);
};
