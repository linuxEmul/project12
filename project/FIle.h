#ifndef FILE_H
#define FILE_H

#include "CommonLibrary.h"
#include "TableManager.h"
#include "FileSystem.h"
#include "DirectoryManager.h"
#include "PathManager.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class File
{
public:
	File();
	~File();

	void createFile(Entry* filename, int& dirInodeNo );	// dir�� filename �̸��� ���� file ����

	int openFile( Entry& file );

	void readFile(int fd, char* buffer, int size);//, TableManager& tm, FileSystem& fs );

	void writeFile(int fd, char* buffer); // ������ �ִ� ���ϵ����Ϳ� buffer�� �ٿ� ����
	void writeFile(int fd, char* buffer, int size);// ������ �ִ� ���ϵ����͸� buffer�� ���� ���� //,  TableManager& tm, FileSystem& fs );

	void seekFile(int fd, int offset);//, TableManager& tm  );

	void closeFile(int fd);//, TableManager& tm );

	int unlinkFile(char* file);//, TableManager& tm, FileSystem& fs );

							   /* �� */
	Entry* findFile( char* filename,  int* dirInode = 0 );
	//int createAndOpen( char* filename, Entry* fileEntry, int* dirInodeNo = NULL );
	int createAndOpen( char* filename, Entry* fileEntry, int& dirInodeNo );
	int open( Entry* fileEntry );

							  // chmod
	void changeFileMode(char* file, char* mode);

	// cat
	void overwriteCat(char* file, string data);
	void joinCat(char* file, char* data);
	bool displayCat(int fd);

	// rm
	void removeFile(char* file);

	// split
	void splitFile(char* source, char* first_target, char* second_target);

	// paste
	void pasteFile(char* firstFile, char* secondFile);

	// file copy
	string readSystemFile(char* filename);
	void copyFile(char* sourceFile, char* targetFile);

	// close

private:
};


#endif FILE_H