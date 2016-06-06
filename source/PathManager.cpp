#include "PathManager.h"

// 절대 경로만 들어온다고 가정.
void PathManager::doAnalyzeFolder(char* direct, vector<string>& arr)
{
	string tmp = direct;

	vector<string>& vStr = *tokenize(tmp, "/");
	arr = vStr;
}

bool PathManager::isRelativePath(char * path)
{
	string tmp = path;
	vector<string> vTmp = *tokenize(tmp, "/");

	for (int i = 0; i < vTmp.size(); i++) {
		if (vTmp[i] == ".." || vTmp[i] == ".")
			return true;
	}
	if (tmp.length() == 2 && tmp.find("..") != string::npos)
		return true;
	if (tmp.length() == 1 && tmp.find(".") != string::npos)
		return true;
	if (tmp[0] != '/') // 파일명이나 a/ab/b 이런식으로도 들어오면 이것도 상대경로
		return true;
	return false;
}

// 절대 경로만 들어온다고 가정
vector<string>* PathManager::getAllAbsPath(char * direc)
{
	string tmp = direc;

	vector<string>* vStr = tokenize(tmp, "/");
	
	vector<string>* path = new vector<string>;
	
	// root 추가
	path->push_back("/");
	for (int i = 0; i < vStr->size(); i++)
	{
		string tmp = "/"; //root부터 시작
		for (int j = 0; j <= i; j++)
		{
			tmp += (*vStr)[j];
		}
		path->push_back(tmp);
	}
	return path;
}

char* PathManager::getAbsolutePath(char* path)
{
	
	if (!isRelativePath(path))
		return path;

	if (path[0] == '/') { // 파일명만 들어올 경우
		string tmp = getCurrentPath();
		tmp = tmp +"/" + path;
		return stringToCharArr(tmp);
	}

	//PathStack 체크
	//if (ps.isEmpty())

	//Tokenize
	vector<string>& tokens = *tokenize(path,  "/");

	//현재 경로 복사
	PathStack stack = ps;
	string strPath = path;

	stack.push("/"); //root

	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "..")
		{
			stack.pop();
		}
		else if (tokens[i] != ".")
		{
			char *cstr = new char[tokens[i].length() + 1];
			strcpy(cstr, tokens[i].c_str());
			stack.push(cstr);
		}
	}



	/*
	int pos=-1, prevPos = -1;
	while ((pos = strPath.find("/", pos + 1)) != string::npos)
	{

		if (pos >= 1 && strPath[pos - 1] == '.') 
		{
			if (pos >= 2 && strPath[pos - 2] == '.') //상위 디렉토리를 가리킬 경우
			{
				stack.pop();
			}
			else //현재 디렉토리를 가리킬 경우
			{
				// 무시
			}
		}
		else
		{
			if (prevPos != -1) {
				string tmp = strPath.substr(prevPos + 1, pos - prevPos - 1);
				char *cstr = new char[tmp.length() + 1];
				strcpy(cstr, tmp.c_str());
				stack.push(cstr);
			}
		}
		prevPos = pos;
	}
	*/
	return getPathStackTrace(stack);
}

char* PathManager::getCurrentPath()
{
	if (ps.isEmpty())
		return NULL;
	else
	{
		// 복사본 만들기
		PathStack tmp = ps;
		char* dir;
		string fullPath = "";
		

		while ((dir = tmp.pop()))
		{
			string s = dir;
			reverse(begin(s), end(s));
			
			if (dir != "/" && !tmp.isEmpty()) {
				fullPath += s;
				fullPath += "/";
			}
		}
		reverse(begin(fullPath), end(fullPath));
		char *cstr = new char[fullPath.length() + 1];
		strcpy(cstr, fullPath.c_str());

		return cstr;
	}
}

char * PathManager::getPathStackTrace(PathStack stack)
{
	if (stack.isEmpty())
		return NULL;
	else
	{
		// 복사본 만들기
		PathStack tmp = stack;
		char* dir;
		string fullPath = "";


		while ((dir = tmp.pop()))
		{
			fullPath += dir;
			fullPath += "/";
		}
		reverse(begin(fullPath), end(fullPath));
		char *cstr = new char[fullPath.length() + 1];
		strcpy(cstr, fullPath.c_str());

		return cstr;
	}
}
void PathManager::setAbsPathInPathStack(char * path)
{
	initPathStack();
	vector<string> arr = doAnalyzeFolder(path);
	
	for (int i = 0; i < arr.size(); i++)
	{
		ps.push(stringToCharArr(arr[i]));
	}
}
PathManager* PathManager::instance = NULL;