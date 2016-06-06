#include "Shell.h"

const char* emul_msg = "LinuxEmulator 1.0\0";
const char* user_msg = "J4\0";

void Shell::run()
{
	PathManager& pm = *PathManager::getInstance();
	login();
	while (true)
	{
		char* path = pm.getCurrentPath();
		cout << user_msg << "@" << emul_msg << ":";
		string p = path;
		if (p == "/home")
			cout << "~";
		else
			cout << p;
		cout << "$";
		string str = input();
		if (str == "quit")
			break;
		analyzeCmd(stringToCharArr(str));
	}
}

void Shell::processCmd(CmdList cl, vector<string>& param)
{
	PathManager& pm = *PathManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();

	switch (cl)
	{
	case _ls:
		if (param.size() == 1)
		{
			dm.Dir_Read(pm.getCurrentPath());
		} //일반 ls
		else if (param.size() == 2)
		{
			if (pm.isRelativePath(stringToCharArr(param[1])) == true)
				dm.Dir_Read(pm.getAbsolutePath(stringToCharArr(param[1])));
			else if (pm.isRelativePath(stringToCharArr(param[1])) == false)
				dm.Dir_Read(stringToCharArr(param[1]));

		} //절대경로, 상대경로
		else cout << "error" << endl;
		break;

	case _mkdir:
		if (param.size() == 2)
		{
			//절대 경로로 바꿔줘야 함 direc

			if (pm.isRelativePath(stringToCharArr(param[1])) == true)  //상대경로일 경우 ( mkdir ../a )
				dm.Dir_Create(pm.getAbsolutePath(stringToCharArr(param[1])));
			else if (pm.isRelativePath(stringToCharArr(param[1])) == false)  // ( mkdir /a/b ) 
				dm.Dir_Create(stringToCharArr(param[1]));

			// 절대경로에 맞는 위치에 디렉토리 생성
			// 상대경로에 맞는 위치에 디렉토리 생성
		}
		else cout << "error" << endl;
		break;

	case _rmdir:
		if (param.size() == 2)
		{
			if (pm.isRelativePath(stringToCharArr(param[1])) == true)
				dm.Dir_Unlink(pm.getAbsolutePath(stringToCharArr(param[1])));
			else if (pm.isRelativePath(stringToCharArr(param[1])) == false)
				dm.Dir_Unlink(stringToCharArr(param[1]));

		}
		else cout << "error" << endl;
		break;

	case _cat:
		if (param.size() == 2)
		{
			caseOfdisplayCat((char*)param[1].c_str());
		} // cat a
		else if (param.size() == 3)
		{
			string userInputData = getUserInputData();

			if (param[1] == ">")
				caseOfoverwriteCat((char*)param[2].c_str(), userInputData);

			else if (param[1] == ">>")
				caseOfjoinCat((char*)param[2].c_str(), userInputData);

		} // cat > a, cat >> a
		else cout << "error" << endl;
		break;


	case _rm:
		if (param.size() == 2)
		{
			char* filename = (char*)param[1].c_str();
			char kinds = isFile(filename);
			if (kinds = 'f')
			{
				caseOfRemoveFile(filename);
			}
			else
				;// casOfRemoveDir이든 뭐든 dir unlink를 불러주는 함수를 넣어주;

		}
		else cout << "error" << endl;
		break;


	case _pwd:
		if (param.size() == 1)
		{
			display(pm.getCurrentPath());
		}
		else cout << "error" << endl;
		break;

	case _chmod:
		if (param.size() == 3)
		{
			int mode = stoi(param[1]);
			caseOfChmod((char*)param[2].c_str(), mode);
		} // chmod 222 a
		else cout << "error" << endl;
		break;


	case _mv:
		if (param.size() == 3)
		{
			/*
			if (a, b같은 디렉토리면 a를 b로 파일명 변경)
			{

			}
			else
			{
				vector<string> a;
				pm.doAnalyzeFolder(stringToCharArr(param[1]), a);
				string curDirName = a[a.size() - 1];
				vector<string>& arr = *pm.getAllAbsPath(stringToCharArr(param[1]));
				vector<string>& arr2 = *pm.getAllAbsPath(stringToCharArr(param[2]));
				//pm.getAllAbsPath( stringToCharArr(param[2]) );

				Directory curdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 1]));
				Directory topdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 2])); //상위디렉토리 객체

				int curinode = topdr.findName(stringToCharArr(curDirName))->inodeNum;
				int topinode = curdr.findName("..")->inodeNum;

				curdr.rmDirectory(topinode);
				topdr.rmDirectory(curinode);

				Directory mvdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 1])); //옮겨질 상위디렉토리

				Entry e;
				e.inodeNum = curinode;
				strcpy(e.name, stringToCharArr(curDirName));
				mvdr.addDirectory(e, curinode);

				//dm.Dir_Unlink( stringToCharArr(arr[arr.size()-1]) );
				//경로 a를 경로 b로 이동
			}
			*/
		}
		else cout << "error" << endl;
		break;

	case _cp:
		if (param.size() == 3)
		{
			vector<string> a;
			pm.doAnalyzeFolder(stringToCharArr(param[1]), a);
			string curDirName = a[a.size() - 1];
			vector<string>& arr = *pm.getAllAbsPath(stringToCharArr(param[1]));
			vector<string>& arr2 = *pm.getAllAbsPath(stringToCharArr(param[2]));
			//pm.getAllAbsPath( stringToCharArr(param[2]) );

			Directory curdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 1]));
			Directory topdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 2])); //상위디렉토리 객체

			int curinode = topdr.findName(stringToCharArr(curDirName))->inodeNum;
			int topinode = curdr.findName("..")->inodeNum;

			//curdr.rmDirectory(topinode);
			//topdr.rmDirectory(curinode);

			Directory mvdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 1])); //옮겨질 상위디렉토리

			Entry e;
			e.inodeNum = curinode;
			strcpy(e.name, stringToCharArr(curDirName));
			mvdr.addDirectory(e, curinode);
		}
		else cout << "error" << endl;
		break;

	case _cd:
		if (param.size() == 2)
		{
			// cd a 현재 디렉토리에 있는 디렉토리 a로 이동
			// cd /a/b 절대경로
			// cd .. 상위 디렉토리로 이동
			// cd ../a 상대경로

			if (pm.isRelativePath(stringToCharArr(param[1])) == true)  //상대경로 일 때
			{
				pm.setAbsPathInPathStack(pm.getAbsolutePath(stringToCharArr(param[1])));
			}
			else if (pm.isRelativePath(stringToCharArr(param[1])) == false)
			{
				pm.setAbsPathInPathStack(stringToCharArr(param[1]));
			}
		}
		else cout << "error" << endl;
		break;

	case _split:
		if (param.size() == 2)
		{
			caseOfSplitFile((char*)param[1].c_str());
		}
		else cout << "error" << endl;
		break;

	case _paste:
		if (param.size() == 3)
		{
			caseOfPasteFile((char*)param[1].c_str(), (char*)param[2].c_str());
		}
		else cout << "error" << endl;
		break;

	case _filecopy:
		if (param.size() == 3)
		{
		}
		else cout << "error" << endl;
		break;

	case _close:
		if (param.size() == 2)
		{
			int fd = stoi(param[1]);
			char* filename = (char*)param[1].c_str();
			char kinds = isFile(filename);
			if (kinds = 'f')
			{
				File file;
				file.closeFile(fd);
			}
		}
		else cout << "error" << endl;
		break;

	case _display:
		if (param.size() == 2)
		{
			Display d;
			d;
			if (param[1] == "t")
			{
				d.displayFileDiscriptorTable();
				d.displaySystemFileTable();
				d.displayInodeTable();				
			}
			else
			{
				d.printBlockNum(stoi(param[1]));
			}
		}
		else cout << "error" << endl;
		break;

	case _quit:
		if (param.size() == 1)
		{
		}
		else cout << "error" << endl;
		break;
	}
}

void Shell::analyzeCmd(const char * str)
{
	vector<string>& result = *tokenize(str);
	string cmd = result[0];

	if (cmd == "ls")
		processCmd(_ls, result);
	else if (cmd == "mkdir")
		processCmd(_mkdir, result);
	else if (cmd == "rmdir")
		processCmd(_rmdir, result);
	else if (cmd == "cat")
		processCmd(_cat, result);
	else if (cmd == "rm")
		processCmd(_rm, result);
	else if (cmd == "pwd")
		processCmd(_pwd, result);
	else if (cmd == "chmod")
		processCmd(_chmod, result);
	else if (cmd == "mv")
		processCmd(_mv, result);
	else if (cmd == "cp")
		processCmd(_cp, result);
	else if (cmd == "split")
		processCmd(_split, result);
	else if (cmd == "paste")
		processCmd(_paste, result);
	else if (cmd == "filecopy")
		processCmd(_filecopy, result);
	else if (cmd == "close")
		processCmd(_close, result);
	else if (cmd == "display")
		processCmd(_display, result);
	else if (cmd == "quit")
		processCmd(_quit, result);
}


void Shell::caseOfdisplayCat(char* filename)
{
	File file;
	int fd = file.findFile(filename);

	char enter = 13;
	string sEnter = enter + "";
	string input = sEnter;
	while (input != "q")
	{
		if (input == sEnter || input == "")
			file.displayCat(fd);

		cin >> input;
	}
}

string Shell::getUserInputData()
{
	string userInputData;
	string inputLine = "";
	while (inputLine != "/quit")
	{
		userInputData += inputLine;
		getline(cin, inputLine);
	}

	return userInputData;
}

void Shell::caseOfoverwriteCat(char* filename, string userInputData)
{
	File file;
	file.overwriteCat(filename, userInputData);
}

void Shell::caseOfjoinCat(char* filename, string userInputData)
{
	File file;
	file.joinCat(filename, (char*)userInputData.c_str());
}

void Shell::caseOfChmod(char* filename, int mode)
{
	File file;
	char c_mode[5];
	c_mode[0] = 'f';
	itoa(mode, &c_mode[1], 10);
	c_mode[4] = '\0';
	file.changeFileMode(filename, c_mode);
}

char Shell::isFile(char * filename)
{
	DirectoryManager* d = DirectoryManager::getInstance();
	DirectoryManager& dm = *d;

	TableManager* t = TableManager::getInstance();
	TableManager& tm = *t;

	Directory currentDir = dm.getRecentlyOpenedDir();

	Entry* fileEntry = currentDir.findName(filename);
	tm.isExistInInodeTable(fileEntry->inodeNum);

	Directory* dir = dm.returnDir(fileEntry->inodeNum);
	if (dir == NULL)
		return 'f';

	return 'd';
}

void Shell::caseOfRemoveFile(char* filename)
{
	File file;
	file.removeFile(filename);
}

void Shell::caseOfSplitFile(char* filename)
{
	File file;

	string firstTargetFilename = "x";
	firstTargetFilename = firstTargetFilename + filename + "a";
	string secondTargetFilename = "x";
	secondTargetFilename = secondTargetFilename + filename + "b";

	file.splitFile(filename, (char*)firstTargetFilename.c_str(), (char*)secondTargetFilename.c_str());
}

void Shell::caseOfPasteFile(char* firstFile, char* secondFile)
{
	File file;
	file.pasteFile(firstFile, secondFile);
}

void Shell::login()
{
	cout << "--------------------------------------------" << endl;
	cout << "-------------------login--------------------" << endl;
	cout << "--------------------------------------------" << endl;

	//Path Manager 초기화
	//PathStack은 기본적으로 /home을 가짐
	PathManager& pm = *PathManager::getInstance();
	PathStack ps;
	ps.push("/");
	ps.push("home");
	pm.setPathStack(ps);

	cout << endl;
	cout << "-----------------login success--------------" << endl;

}
