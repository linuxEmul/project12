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
		if (str == "quit")
			break;
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
         if (param[1] == "/")
         {
            display("/는 삭제할 수 없습니다.");
            return;
         }
         string path = param[1];
         DirectoryManager& dm = *DirectoryManager::getInstance();
         PathManager& pm = *PathManager::getInstance();
         char* filename = stringToCharArr(pm.doAnalyzeFolder(stringToCharArr(path))[pm.doAnalyzeFolder(stringToCharArr(path)).size() - 1]);

         if (pm.isRelativePath(stringToCharArr(path)))
            path = pm.getAbsolutePath(stringToCharArr(path));

		 if(!dm.isReallyExist(stringToCharArr(path)))
		 {
			 display("경로가 존재하지 않습니다.");
		 }

         vector<string> vAllAbs = *pm.getAllAbsPath(stringToCharArr(path));
		 vector<string>& vArr = *pm.getAllAbsPath(pm.getCurrentPath());
		 if (vAllAbs.size() == 1)
         {
            display("/는 삭제할 수 없습니다.");
            return;
         }
		 if(path==pm.getCurrentPath())
		 {
			 display(".과 ..은 삭제할 수 없습니다.");
            return;
		 }
		 else if(path == vArr[vArr.size()-2])
		 {
			 display(".과 ..은 삭제할 수 없습니다.");
            return;
		 }
		 
		 for(int i=0;i < vArr.size(); i++)
		 {
			if(path == vArr[i]){
				display("현재 경로의 상위 디렉토리는 삭제할 수 없습니다.");
				return;
			}
		 }

		 //추가로 내 현재 경로 상위 폴더에 있는 애들을 삭제하려고 하면 차단해야 함

         Directory d = dm.Dir_Read(stringToCharArr(vAllAbs[vAllAbs.size() - 2]));
         char kinds = dm.isFile(filename, d);
         if (kinds == 'f')
         {
            
            caseOfRemoveFile(filename);
         }
         else
            dm.Dir_Unlink_All(stringToCharArr(path));

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
			if (pm.isRelativePath(stringToCharArr(param[2])))
				path = pm.getAbsolutePath(stringToCharArr(param[2]));
			int mode = stoi(param[1]);
			caseOfChmod(stringToCharArr(path), mode);
		} // chmod 222 a
		else cout << "error" << endl;
		break;


	case _mv:

		if (param.size() == 3)
		{
			string path = param[1], path2 = param[2];
			if (pm.isRelativePath(stringToCharArr(path)))
				param[1] = pm.getAbsolutePath(stringToCharArr(path));
			if (pm.isRelativePath(stringToCharArr(path2)))
				param[2] = pm.getAbsolutePath(stringToCharArr(path2));

			vector<string> b;
			pm.doAnalyzeFolder(stringToCharArr(param[2]), b);
			string bName = b[b.size() - 1];

			vector<string> a;
			pm.doAnalyzeFolder(stringToCharArr(param[1]), a);
			string aName = a[a.size() - 1];

			if (isSameDirectory(stringToCharArr(param[1]), stringToCharArr(param[2])) == true)
			{
				vector<string>& arr = *pm.getAllAbsPath(stringToCharArr(param[1]));
				vector<string>& arr2 = *pm.getAllAbsPath(stringToCharArr(param[2]));

				Directory topdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 2]));



				//cout << stringToCharArr(bName );
				//a, b같은 디렉토리면 a를 b로 파일명 변경
				if (topdr.findName(stringToCharArr(bName)) == NULL)//이름이 존재하지 않으면
				{
					cout << "if문 안에 들어 왔다 " << endl;

					//  source   inodenum
					int cn = topdr.findName(stringToCharArr(aName))->inodeNum;
					// 상위디렉토리의 inodenum
					int myn = topdr.findName(".")->inodeNum;

					//source Entry 정보
					Entry e;
					e.inodeNum = topdr.findName(stringToCharArr(aName))->inodeNum;
					strcpy(e.name, stringToCharArr(stringToCharArr(bName)));

					topdr.rmDirectory(cn, myn);
					topdr.addDirectory(e, myn);

				}
				else
				{
					cout << "존재" << endl;
				}

			}
			else
			{
				// 상대경로 안됨.. 
				// mv 파일 아직 안함

				vector<string>& arr = *pm.getAllAbsPath(stringToCharArr(param[1]));
				vector<string>& arr2 = *pm.getAllAbsPath(stringToCharArr(param[2]));
				//pm.getAllAbsPath( stringToCharArr(param[2]) );
				Directory topdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 2]));//상위디렉토리 객체

																					// 옮길 대상이 파일일 경우
				if (dm.isFile(stringToCharArr(aName), topdr) == 'f') {
					Entry& entry = *topdr.findName(stringToCharArr(aName));
					int curinode = entry.inodeNum;
					int topinode = topdr.findName(".")->inodeNum;

					// .. 삭제
					topdr.rmDirectory(curinode, topinode);

					Directory mvdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 1])); //옮겨질 상위디렉토리

					int mvinode = mvdr.findName(".")->inodeNum;

					Entry e;
					e.inodeNum = curinode;
					strcpy(e.name, stringToCharArr(aName));
					mvdr.addDirectory(e, mvinode);
				}
				// 옮길 대상이 디렉토리일 경우
				else {

					Directory curdr = dm.Dir_Read(stringToCharArr(arr[arr.size() - 1]));


					int curinode = topdr.findName(stringToCharArr(aName))->inodeNum;
					int topinode = curdr.findName("..")->inodeNum;

					//int temp = curinode;

					// .. 삭제
					curdr.rmDirectory(topinode, curinode);
					topdr.rmDirectory(curinode, topinode);

					Directory mvdr = dm.Dir_Read(stringToCharArr(arr2[arr2.size() - 1])); //옮겨질 상위디렉토리

					int mvinode = mvdr.findName(".")->inodeNum;

					Entry e;
					e.inodeNum = curinode;
					strcpy(e.name, stringToCharArr(aName));
					mvdr.addDirectory(e, mvinode);

					// .. 추가
					e.inodeNum = mvinode;
					strcpy(e.name, stringToCharArr(".."));
					curdr.addDirectory(e, curinode);
				}
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
			caseOfFileCopy((char*)param[1].c_str(), (char*)param[2].c_str());
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

char* getAbsPath(char* filename)
{
	PathManager& pm = *PathManager::getInstance();
	char* absFilename = pm.getAbsolutePath(filename);

	return absFilename;
}

void Shell::caseOfdisplayCat(char* filename)
{
	char* absFilename = getAbsPath(filename);

	File file;
	int* dirInodeNo = new int;
	int fd = file.open(file.findFile(absFilename, dirInodeNo));

	delete dirInodeNo;

	if (fd <= 0)
		return;

	string input;
	while (input != "q")
	{
		if (!file.displayCat(fd))
		{
			cout << endl;
			return;
		}

		cout << endl;

		getline(cin, input);
		if (input.at(0) == '\n')
			cout << endl << endl;
	}
}

string Shell::getUserInputData()
{
	string userInputData;
	string inputLine = "";
	while (true)
	{
		getline(cin, inputLine);

		if (inputLine == "/quit")
			break;

		userInputData += '\n' + inputLine;
	}

	return userInputData;
}

void Shell::caseOfoverwriteCat(char* filename, string userInputData)
{
	char* absFilename = getAbsPath(filename);
	File file;
	file.overwriteCat(absFilename, userInputData);
}

void Shell::caseOfjoinCat(char* filename, string userInputData)
{
	char* absFilename = getAbsPath(filename);
	File file;
	file.joinCat(absFilename, (char*)userInputData.c_str());
}

void Shell::caseOfChmod(char* path, int mode)
{
	DirectoryManager& dm = *DirectoryManager::getInstance();
	PathManager& pm = *PathManager::getInstance();
	char* filename = stringToCharArr(pm.doAnalyzeFolder(path)[pm.doAnalyzeFolder(path).size() - 1]);

	vector<string> vAllAbs = *pm.getAllAbsPath(path);
	Directory d = dm.Dir_Read(stringToCharArr(vAllAbs[vAllAbs.size() - 2]));

	cout << filename << endl;
	if (dm.isFile(filename, d) == 'f')
	{
		File file;
		char c_mode[5];
		c_mode[0] = 'f';
		itoa(mode, &c_mode[1]);
		c_mode[4] = '\0';
		file.changeFileMode((char*)vAllAbs[vAllAbs.size() - 1].c_str(), c_mode);
	}
	else
	{
		char c_mode[5];
		c_mode[0] = 'd';
		itoa(mode, &c_mode[1], 10);
		c_mode[4] = '\0';
		dm.changeDirMode(stringToCharArr(vAllAbs[vAllAbs.size() - 1]), c_mode);
	}

}

void Shell::caseOfRemoveFile(char* filename)
{
	char* absFilename = getAbsPath(filename);
	File file;
	file.removeFile(absFilename);
}

void Shell::caseOfSplitFile(char* filename)
{
	PathManager& pm = *PathManager::getInstance();

	char* absFilename = getAbsPath(filename);
	vector<string>absFilenames = *pm.getAllAbsPath( absFilename );
	vector<string> filenames = pm.doAnalyzeFolder(absFilename);
	filename = (char*)filenames[filenames.size() - 1].c_str();

	string firstTargetFilename = "x";
	firstTargetFilename = absFilenames[absFilenames.size()-2] + "/" + firstTargetFilename + filename + "a"; // 절대경로로 만들어 줌
	
	string secondTargetFilename = "x";
	secondTargetFilename = absFilenames[absFilenames.size()-2] + "/" + secondTargetFilename + filename + "b";

	File file;
	file.splitFile(absFilename, (char*)firstTargetFilename.c_str(), (char*)secondTargetFilename.c_str());

}

void Shell::caseOfPasteFile(char* firstFile, char* secondFile)
{
	File file;
	char* absFirstFilename = getAbsPath(firstFile);
	char* absSecondFilename = getAbsPath(secondFile);

	file.pasteFile(absFirstFilename, absSecondFilename);

}

void Shell::caseOfCopyFile( char* sourceFile, char* targetFile )
{
	char* absSourceFile = getAbsPath( sourceFile );
	char* absTargetFile = getAbsPath( targetFile );

}

void Shell::caseOfFileCopy(char* sourceSysFile, char* targetFile)
{
	File file;
	//char* absSourceFilename = getAbsPath(sourceSysFile);
	char* absTargetFilename = getAbsPath(targetFile);
	file.copyFile(sourceSysFile, targetFile);
}

bool Shell::isSameDirectory(char* firstFile, char* secondFile)
{
	PathManager& pm = *PathManager::getInstance();

	char* firstFilePath = pm.getAbsolutePath(firstFile);
	char* secondFilePath = pm.getAbsolutePath(secondFile);

	vector<string>* firstFileAllPath = pm.getAllAbsPath(firstFile);
	vector<string>* secondFileAllPath = pm.getAllAbsPath(secondFile);

	if (firstFileAllPath->at(firstFileAllPath->size() - 2) == secondFileAllPath->at(secondFileAllPath->size() - 2))
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