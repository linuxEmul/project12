#include "FileSystem.h"

struct Entry
{
	char name[255]; // 엔트리 이름
	int inodeNum; // inode number
	//int mode; // 종류 디렉토리 1, 파일 2
};

class Directory
{
public:
	Entry entryList[20]; // 20 ??
	int entryCnt;

	Directory();
	Directory(const Directory& t)
	{
		entryCnt = t.entryCnt;
		for (int i = 0; i < entryCnt; i++)
		{
			entryList[i] = t.entryList[i];
		}
	}
	Directory& operator=(const Directory& t)
	{
		entryCnt = t.entryCnt;
		for (int i = 0; i < entryCnt; i++)
		{
			entryList[i] = t.entryList[i];
		}
		return *this;
	}

	

	void setInodeNum(int cur, int top); //디렉토리 만들때 호출. 초기화 해줌.

	void addDirectory(Entry entry);
	void addDirectory(Entry entry, int inodeNum);
	void rmDirectory(int inodeN, int myInodeNum);
	Entry* findName( char *);

	bool isExist(char *);
};
