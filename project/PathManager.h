#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <algorithm>
#include <iterator>
#include "PathStack.h"
#include "CommonLibrary.h"

using namespace std;

class PathManager
{
private:
	PathStack ps;
	static PathManager* instance;

	PathManager(){}
public:
	// 폴더 분석 함수
	void doAnalyzeFolder(char* direc, vector<string>& arr);
	vector<string> doAnalyzeFolder(char* direc) {
		vector<string> arr;
		doAnalyzeFolder(direc, arr);
		return arr;
	}

	// 상대 경로 판단 함수
	bool isRelativePath(char* path);

	// 절대 경로 속 모든 절대 경로 반환 함수
	vector<string>* getAllAbsPath(char* direc);

	//상대경로 -> 절대경로 변환 함수
	char* getAbsolutePath(char* path);

	//현재 경로를 절대 경로로 반환
	char* getCurrentPath();

	//PathStack을 통해서 경로 추출
	char* getPathStackTrace(PathStack ps);

	//절대경로를 PathStack에 반영하는 함수
	void setAbsPathInPathStack(char* path);

	//PathStack Getter & Setter
	PathStack* getPathStack() { return &ps; }
	void setPathStack(PathStack* ps) { this->ps = *ps; }
	void setPathStack(PathStack ps) { this->ps = ps; }

	void initPathStack() { ps.initialize(); }


	static PathManager* getInstance()
	{
		if (!instance)
			instance = new PathManager();
		return instance;
	}
};

#endif