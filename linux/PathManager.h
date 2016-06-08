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

	PathManager() {}
public:
	static bool allowDelete;

	~PathManager()
	{
		if (allowDelete)
		{
			if (instance)
				delete instance;
		}
	}

	// ���� �м� �Լ�
	void doAnalyzeFolder(char* direc, vector<string>& arr);
	vector<string> doAnalyzeFolder(char* direc) {
		vector<string> arr;
		doAnalyzeFolder(direc, arr);
		return arr;
	}

	// ��� ��� �Ǵ� �Լ�
	bool isRelativePath(char* path);

	// ���� ��� �� ��� ���� ��� ��ȯ �Լ�
	vector<string>* getAllAbsPath(char* direc);

	//����� -> ������ ��ȯ �Լ�
	char* getAbsolutePath(char* path);

	//���� ��θ� ���� ��η� ��ȯ
	char* getCurrentPath();

	//PathStack�� ���ؼ� ��� ����
	char* getPathStackTrace(PathStack ps);

	//�����θ� PathStack�� �ݿ��ϴ� �Լ�
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