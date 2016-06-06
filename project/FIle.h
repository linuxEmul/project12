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

	void createFile(char* file); //, DirectoryManager& dm );
	int openFile(char* file, TableManager& tm);//, TableManager& tm, FileSystem& fs );
	void readFile(int fd, char* buffer, int size);//, TableManager& tm, FileSystem& fs );
	void writeFile(int fd, char* buffer); // 기존에 있던 파일데이터에 buffer를 붙여 저장
	void writeFile(int fd, char* buffer, int size);// 기존에 있던 파일데이터를 buffer로 덮어 씌움 //,  TableManager& tm, FileSystem& fs );
	void seekFile(int fd, int offset);//, TableManager& tm  );
	void closeFile(int fd);//, TableManager& tm );
	int unlinkFile(char* file);//, TableManager& tm, FileSystem& fs );

							   /* 쉘 */
	int findFile(char* file); // 파일이름만 넘어옴

							  // chmod
	void changeFileMode(char* file, char* mode);

	// cat
	void overwriteCat(char* file, string data);
	void joinCat(char* file, char* data);
	void displayCat(int fd);

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