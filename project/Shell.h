#pragma once
#include <string>
#include <iostream>
#include "CommonLibrary.h"
#include "File.h"
#include "DirectoryManager.h"
#include "PathManager.h"
#include "Display.h"

using namespace std;
class Shell
{
private:
	enum CmdList{_ls, _mkdir, _rmdir, _cat, _rm, _pwd, 
		_chmod, _mv, _cp, _cd, _split, _paste, _filecopy,
		_close, _display, _clear, _quit};
	enum ColorList {
		__RED, __DEFAULT, __GREEN, __BLUE, __YELLOW, __PURPLE, __LIGHT_BLUE, __WHITE
	};
public:
	~Shell() {
		cout << "JJJJ! Bye!" << endl;
	}
	void run();
	void analyzeCmd(const char* str); //명령어 분석
	void processCmd(CmdList cl, vector<string>& param);

	void display(char* msg) { cout << msg << endl; }
	void colorDisplay(char* msg, ColorList color = __DEFAULT) {
		switch (color)
		{
		case Shell::__DEFAULT:
			printf("%s\n[1;30m", msg);
			break;
		case Shell::__RED:
			printf("%s[1;31m", msg);
			break;
		case Shell::__GREEN:
			printf("%s\n[1;32m", msg);
			break;
		case Shell::__BLUE:
			printf("%s\n[1;34m", msg);
			break;
		case Shell::__YELLOW:
			printf("%s\n[1;33m", msg);
			break;
		case Shell::__PURPLE:
			printf("%s\n[1;35m", msg);
			break;
		case Shell::__LIGHT_BLUE:
			printf("%s\n[1;36m", msg);
			break;
		case Shell::__WHITE:
			printf("%s\n[1;37m", msg);
			break;
		default:
			break;
		}
	}
	string input() {
		string str;
		getline(cin, str);
		return str;
	}

	string getUserInputData();


	void caseOfdisplayCat( char* filename );
	void caseOfoverwriteCat( char* filename, string userInputData );
	void caseOfjoinCat( char* filename, string userInputData );

	void caseOfChmod( char* path, int mode );

	void caseOfRemoveFile( char * filename );

	void caseOfSplitFile( char* filename );
	void caseOfPasteFile( char* firstFile, char* secondFile );
	void caseOfCopyFile( char* sourceFile, char* targetFile );
	void caseOfFileCopy(char* sourceSysFile, char* targetFile);

	bool isSameDirectory( char* firstFile, char* secondFile );
	void displayMode(char* mode);
	void login();
};	