#include "Shell.h"

const char* emul_msg = "LinuxEmulator\0";
const char* user_msg = "JJJJ\0";

void Shell::run()
{
	PathManager& pm = *PathManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();
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
		if (str == "quit") {
			analyzeCmd("display t");
			analyzeCmd("display 0");
			analyzeCmd("display 1");
			analyzeCmd("display 2");
			analyzeCmd("display 3");
			analyzeCmd("display 4");
			analyzeCmd("display 5");
			break;
		}
		analyzeCmd(stringToCharArr(str));
	}
}

void Shell::processCmd(CmdList cl, vector<string>& param)
{
	PathManager& pm = *PathManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();
	FileSystem& fs = *FileSystem::getInstance();
	TableManager& tm = *TableManager::getInstance();

	switch (cl)
	{
	case _ls:
		if (param.size() == 1)
		{
			Directory d = dm.Dir_Read(pm.getCurrentPath());
			cout << "total " << d.entryCnt << endl;
			for (int i = 0; i < d.entryCnt; i++)
			{
				Inode inodeData = fs.inodeBlock->getInodeData(d.entryList[i].inodeNum);
				cout << to_string(d.entryList[i].inodeNum) << " ";
				//모드 출력
				displayMode(inodeData.mode);
				cout << " " << inodeData.linksCount << " " << inodeData.size << " " << inodeData.ctime << " " << d.entryList[i].name << endl;
			}
		} //일반 ls
		else if (param.size() == 2)
		{
			//경로 변환
			string path = param[1];
			if (pm.isRelativePath(stringToCharArr(path)))
				path = pm.getAbsolutePath(stringToCharArr(path));

			//해당 경로가 존재하는 지 확인
			vector<string>& allPath = *pm.getAllAbsPath(stringToCharArr(path));
			if (!dm.isReallyExist(stringToCharArr(allPath[allPath.size() - 1])))
			{
				display("해당 경로가 존재하지 않습니다.");
				display((char*)path.c_str());
				return;
			}

			//처리
			Directory d = dm.Dir_Read(stringToCharArr(path));

			cout << "total " << d.entryCnt << endl;
			for (int i = 0; i < d.entryCnt; i++)
			{
				Inode inodeData = fs.inodeBlock->getInodeData(d.entryList[i].inodeNum);
				cout << to_string(d.entryList[i].inodeNum) << " ";
				//모드 출력
				displayMode(inodeData.mode);
				cout << " " << inodeData.linksCount << " " << inodeData.size << " " << inodeData.ctime << " " << d.entryList[i].name << endl;
			}

		} //절대경로, 상대경로
		else cout << "error" << endl;
		break;

	case _mkdir:
		display(pm.getCurrentPath());
		if (param.size() == 2)
		{
			//경로 변환
			string path = param[1];
			if (pm.isRelativePath(stringToCharArr(path)))
				path = pm.getAbsolutePath(stringToCharArr(path));

			//경로 존재 확인
			vector<string>& allPath = *pm.getAllAbsPath(stringToCharArr(path));
			if (allPath.size() > 1 && !dm.isReallyExist(stringToCharArr(allPath[allPath.size() - 2]))) {
				display("해당 경로가 존재하지 않습니다.");
				return;
			}
			if (dm.isReallyExist(stringToCharArr(allPath[allPath.size() - 1])))
			{
				display("해당 경로가 이미 존재합니다.");
				return;
			}
			
			dm.Dir_Create(stringToCharArr(path));
		}
		else cout << "error" << endl;
		break;

	case _rmdir:
		if (param.size() == 2)
		{
			//경로 변환
			string path = param[1];
			if (pm.isRelativePath(stringToCharArr(path)))
				path = pm.getAbsolutePath(stringToCharArr(path));

			//경로 존재 확인
			vector<string>& allPath = *pm.getAllAbsPath(stringToCharArr(path));
			if (!dm.isReallyExist(stringToCharArr(allPath[allPath.size() - 1])))
			{
				display("해당 경로가 존재하지 않습니다.");
				return;
			}
			dm.Dir_Unlink(stringToCharArr(path));

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
         string path = param[1];
         DirectoryManager& dirm = *DirectoryManager::getInstance();
         PathManager& pm = *PathManager::getInstance();
         char* filename = stringToCharArr(pm.doAnalyzeFolder(stringToCharArr(path))[pm.doAnalyzeFolder(stringToCharArr(path)).size()-1]);
         
         if(pm.isRelativePath(stringToCharArr(path)))
            path = pm.getAbsolutePath(stringToCharArr(path));

         vector<string> vAllAbs = *pm.getAllAbsPath(stringToCharArr(path));
         Directory d = dirm.Dir_Read(stringToCharArr(vAllAbs[vAllAbs.size()-2]));
         char kinds = isFile(filename, d);
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
			string path = param[2];
			if(pm.isRelativePath(stringToCharArr(param[2])))
				path = pm.getAbsolutePath(stringToCharArr(param[2]));
			int mode = stoi(param[1]);
			caseOfChmod(stringToCharArr(path), mode);
		} // chmod 222 a
		else cout << "error" << endl;
		break;

	case _mv:
		if (param.size() == 3)
		{
			//if ( isSameDirectory(param[1].c_str(), param[2].c_str()) )
			//{
			//		//a, b같은 디렉토리면 a를 b로 파일명 변경
			//}
		//	else
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

				//int temp = curinode;

				curdr.rmDirectory(topinode,curinode);
				topdr.rmDirectory(curinode,topinode);

				Directory mvdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 1])); //옮겨질 상위디렉토리

				int mvinode = mvdr.findName("..")->inodeNum;

				Entry e;
				e.inodeNum = curinode;
				strcpy(e.name, stringToCharArr(curDirName));
				mvdr.addDirectory(e, mvinode);
			}
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

			dm.closeAllDir();
			if (pm.isRelativePath(stringToCharArr(param[1])) == true)  //상대경로 일 때
			{
				pm.setAbsPathInPathStack(pm.getAbsolutePath(stringToCharArr(param[1])));
			}
			else if (pm.isRelativePath(stringToCharArr(param[1])) == false)
			{
				pm.setAbsPathInPathStack(stringToCharArr(param[1]));
			}
			dm.openAllDir(pm.getCurrentPath());
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
			caseOfCopyFile((char*)param[1].c_str(), (char*)param[2].c_str());
		}
		else cout << "error" << endl;
		break;

	case _close:
		if (param.size() == 2)
		{
			int fd = stoi(param[1]);

			InodeElement* fileInode = tm.getInodeByFD(fd);
			
			if (fileInode->inode.mode[0] == 'f')
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
	case _clear:
		system("cls");
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
	else if (cmd == "cd")
		processCmd(_cd, result);
	else if (cmd == "quit")
		processCmd(_quit, result);
	else if (cmd == "clear")
		processCmd(_clear, result);
}


void Shell::caseOfdisplayCat(char* filename)
{
	File file;
	 int* dirInodeNo = new int;
   int fd = file.open( file.findFile(filename, dirInodeNo) );
   if ( fd == 0 )
      return;

   delete dirInodeNo;

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

void Shell::caseOfChmod(char* path, int mode)
{
	DirectoryManager& dirm = *DirectoryManager::getInstance();
	PathManager& pm = *PathManager::getInstance();
	char* filename = stringToCharArr(pm.doAnalyzeFolder(path)[pm.doAnalyzeFolder(path).size()-1]);
	
	vector<string> vAllAbs = *pm.getAllAbsPath(path);
	Directory d = dirm.Dir_Read(stringToCharArr(vAllAbs[vAllAbs.size()-2]));

	cout << filename << endl;
	if ( isFile(filename, d) == 'f' )
	{
		File file;
		char c_mode[5];
		c_mode[0] = 'f';
		itoa(mode, &c_mode[1], 10);
		c_mode[4] = '\0';
		file.changeFileMode(filename, c_mode);
	}
	else
	{		
		char c_mode[5];
		c_mode[0] = 'd';
		itoa(mode, &c_mode[1], 10);
		c_mode[4] = '\0';
		dirm.changeDirMode(stringToCharArr(vAllAbs[vAllAbs.size()-1]), c_mode);
	}

}


char Shell::isFile(char * filename, Directory currentDir)
{
	if(strcmp(filename, "/") == 0)
	{
		return 'd';
	}
	DirectoryManager& dm = *DirectoryManager::getInstance();

	Entry* fileEntry = currentDir.findName(filename);
	if(fileEntry == NULL){
		throw "파일이 존재하지 않습니다.";
	}

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

void Shell::caseOfCopyFile( char* sourceFile, char* targetFile )
{
	File file;
	file.copyFile( sourceFile, targetFile );
}

bool isSameDirectory( char* firstFile, char* secondFile )
{
	PathManager& pm = *PathManager::getInstance();
	
	char* firstFilePath = pm.getAbsolutePath( firstFile );
	char* secondFilePath = pm.getAbsolutePath( secondFile );
	
	vector<string>* firstFileAllPath = pm.getAllAbsPath( firstFile );
	vector<string>* secondFileAllPath = pm.getAllAbsPath( secondFile );

	if ( firstFileAllPath[ firstFileAllPath->size()-2 ] == secondFileAllPath[ secondFileAllPath->size()-2 ] )
		return true;

	return false;
}

void Shell::login()
{
	cout << "--------------------------------------------" << endl;
	cout << "-------------------login--------------------" << endl;
	cout << "--------------------------------------------" << endl;

	//Path Manager 초기화
	//PathStack은 기본적으로 /home을 가짐
	PathManager& pm = *PathManager::getInstance();
	DirectoryManager& dm = *DirectoryManager::getInstance();
	PathStack ps;
	ps.push("/");
	ps.push("home");
	pm.setPathStack(ps);

	dm.openAllDir(pm.getCurrentPath());
	cout << endl;
	cout << "-----------------login success--------------" << endl;

}
void Shell::displayMode(char* mode) {
	if (mode[0] == 'd')
		cout << mode[0];
	else
		cout << '-';

	for (int i = 1; i < 4; i++)
	{
		switch (mode[i])
		{
		case '1':
			cout << "r--";
			break;
		case '2':
			cout << "-w-";
			break;
		case '3':
			cout << "rw-";
			break;
		case '4':
			cout << "--x";
			break;
		case '5':
			cout << "r-x";
			break;
		case '6':
			cout << "-wx";
			break;
		case '7':
			cout << "rwx";
			break;
		}
	}

}