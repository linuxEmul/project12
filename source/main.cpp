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

	//파일1 오픈
	int file1_handle = tt.fileOpenEvent(file1_inode, block);
	cout << file1_handle << endl;
	//파일1 파일 포인터 변경
	tt.updateFilePointer(file1_handle, 5);

	//파일2 오픈
	int file2_handle = tt.fileOpenEvent(file2_inode, block);
	cout << file2_handle << endl;
	//파일1 close
	if (!tt.fileCloseEvent(file1_handle))
		cout << "file1 Close Error" << endl;

	//파일3 오픈
	int file3_handle = tt.fileOpenEvent(file3_inode, block);
	cout << file3_handle << endl;
	//파일4 오픈
	int file4_handle = tt.fileOpenEvent(file4_inode, block);
	cout << file4_handle << endl;




	//모두 close
	if (!tt.fileCloseEvent(file2_handle))
		cout << "file2 Close Error" << endl;
	if (!tt.fileCloseEvent(file3_handle))
		cout << "file3 Close Error" << endl;
	if (!tt.fileCloseEvent(file4_handle))
		cout << "file4 Close Error" << endl;
	return 0;
}