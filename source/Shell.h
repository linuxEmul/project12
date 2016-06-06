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
		_close, _display, _quit};
public:

	void run();
	void analyzeCmd(const char* str); //명령어 분석
	void processCmd(CmdList cl, vector<string>& param);

	void display(char* msg) { cout << msg << endl; }
	string input() {
		string str;
		getline(cin, str);
		return str;
	}

	string getUserInputData();
	void caseOfdisplayCat( char* filename );
	void caseOfoverwriteCat( char* filename, string userInputData );
	void caseOfjoinCat( char* filename, string userInputData );

	void caseOfChmod( char* filename, int mode );

	char isFile( char * filename );
	void caseOfRemoveFile( char * filename );

	void caseOfSplitFile( char* filename );
	void caseOfPasteFile( char* firstFile, char* secondFile );

	void login();
};	