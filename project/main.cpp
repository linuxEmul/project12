#include "Shell.h"

using namespace std;

int main()
{

	FileSystem& fs = *FileSystem::getInstance();
	Shell sh;
	fs.loadFS("test.bin");
	sh.run();
	fs.saveFS("test.bin");
	// FileSystem fs;
	PathManager& pm = *PathManager::getInstance();
	PathStack ps;
	
	ps.push("a");
	ps.push("b");
	ps.push("c");
	ps.push("d");

	pm.setPathStack(&ps);
	cout << pm.getCurrentPath() << endl;

	cout << pm.getAbsolutePath("./../d/aa/dkfk") << endl;
	

	TableManager* t = TableManager::getInstance();
	TableManager& tt = *t;
	Inode block;
	block.blocks = (char*)""+2;
	block.ctime = "2";
	block.dataBlockList = "1";
	block.linksCount = "3";
	block.mode = "2";
	block.mtime = "12";
	block.size = "122";
	block.time = "123";

	int file1_inode = 5;
	int file2_inode = 3;
	int file3_inode = 4;
	int file4_inode = 0;

	//����1 ����
	int file1_handle = tt.fileOpenEvent(file1_inode, block);
	cout << file1_handle << endl;
	//����1 ���� ������ ����
	tt.updateFilePointer(file1_handle, 5);

	//����2 ����
	int file2_handle = tt.fileOpenEvent(file2_inode, block);
	cout << file2_handle << endl;
	//����1 close
	if (!tt.fileCloseEvent(file1_handle))
		cout << "file1 Close Error" << endl;

	//����3 ����
	int file3_handle = tt.fileOpenEvent(file3_inode, block);
	cout << file3_handle << endl;
	//����4 ����
	int file4_handle = tt.fileOpenEvent(file4_inode, block);
	cout << file4_handle << endl;




	//��� close
	if (!tt.fileCloseEvent(file2_handle))
		cout << "file2 Close Error" << endl;
	if (!tt.fileCloseEvent(file3_handle))
		cout << "file3 Close Error" << endl;
	if (!tt.fileCloseEvent(file4_handle))
		cout << "file4 Close Error" << endl;
	return 0;
}