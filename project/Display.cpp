#include "Display.h"

using namespace std ;

void Display::displayFileDiscriptorTable()
{
	TableManager tb = *TableManager::getInstance();

	cout << "<< File Descriptor Table>> " << endl ;

	cout << "Index			SF Index" << endl ;

	cout << "0" << "			" << "-" << endl;
	cout << "1" << "			" << "-" << endl;
	cout << "2" << "			" << "-" << endl;
	
	cout << tb.getCount(FDT) << endl;

	if (tb.getCount(FDT) == 3) {
		//cout << "추가 정보가 없습니다." << endl;
		return;
	}

	int* idxList = NULL;
	vector<string> sftIndex ;
	tb.getTableInfo(FDT, idxList, &sftIndex);

	for ( int i = 0; i < tb.getCount(FDT)-3; i++ )
	{
		cout << idxList[i] << "		" << sftIndex[i] << endl;
	} //  File Descriptor Table 출력

}

void Display::displaySystemFileTable()
{
	TableManager tb = *TableManager::getInstance();
	cout << "<< System File Table >> " << endl ;

	int* idxList = NULL;
	vector<SFTElement> sft;
	if (tb.getCount(SFT) == 0) {
		cout << "정보가 없습니다." << endl;
		return;
	}
	cout << tb.getCount(SFT) << endl;
	tb.getTableInfo(SFT, idxList, &sft);
	cout << "Index			Inode Index			File Pointer" << endl ;

	

	for ( int i = 0; i < tb.getCount(SFT); i ++ )
	{
		cout << idxList[i] << "		" << sft[i].inodeTableIdx << "		" << sft[i].file_pointer << endl;
	}
}

void Display::displayInodeTable()
{
	TableManager tb = *TableManager::getInstance();
	cout << "<< Inode Table >> " << endl ;

	if (tb.getCount(INODET) == 0) {
		cout << "정보가 없습니다." << endl;
		return;
	}
	cout << "Index			Inode Number" << endl ;
	int* idxList = NULL;
	vector<InodeElement> inodeE;
	tb.getTableInfo(INODET, idxList, &inodeE);

	for ( int i = 0; i < tb.getCount(INODET); i ++ )
	{
		cout << idxList[i] << "		" << inodeE[i].inode_number << endl;
	}
}

void Display::displaySuperBlock()
{
	FileSystem &fs = *FileSystem::getInstance();
	
	SuperBlock s = fs.superBlock ;

	cout << fs.superBlock.getMagicNum() << endl;

	char* test = s.getMagicNum();
	cout << "Magic Number: " << s.getMagicNum() << endl ;
	cout << "Block Size: " << s.getBlockSize() << endl ;
	cout << "Blocks Per Group: " << s.getBlockPerGroup() << endl ;
	cout << "Number of Inodes: " << s.getInodesNum() << endl ;
	cout << "Inode Size: " << s.getInodeSize() << endl ;
	cout << "Finish Time: " << s.getFinishTime() << endl ; 
	cout << "First Data Block number: " << s.getFirstDataBlockNum() << endl ;
	cout << "First Inode number: " << s.getRootInodeNum() << endl ;
	
}

void Display::displayBlockDescriptorTable()
{
	FileSystem &fs = *FileSystem::getInstance();
	
	BlockDescriptorTable bdt = fs.blockDescriptorTable;

	cout << "Block Bitmap: " << bdt.getBlockBitmapBlockNum() << endl ;
	cout << "Inode Bitmap: " << bdt.getInodeBitmapBlockNum() << endl;
	cout << "first Inode table block number: " << bdt.getFirstInodeTableBlockNum() << endl ;
	cout << "Unassign block number: " << bdt.getUnassignedBlockNum() << endl ;
	cout << "Unassigned Inode number: " << bdt.getUnassignedInodeNum() << endl ;

}

void Display::displayInodeBlock()
{
	FileSystem &fs = *FileSystem::getInstance();
	
	InodeBlock ib1 = fs.inodeBlock[1];
	InodeBlock ib2 = fs.inodeBlock[2];

	cout << " < 사용중인 inode 수: " << fs.getAssignedInodeNum() << " >" << endl ;
	int* inodeList = fs.getAssignedInodeIndex();

	for ( int i = 0 ; i < fs.getAssignedInodeNum() ; i ++ )
	{
		if ( inodeList[i] < 32 )
		{
			cout << "inode: " << inodeList[i] << endl ;
			cout << "mode: " << ib1.getMode(inodeList[i]) << endl ;
			cout << "time: " << ib1.getTime(inodeList[i]) << endl ;
			cout << "ctime: " << ib1.getCtime(inodeList[i]) << endl ;
			cout << "mtime: " << ib1.getMtime(inodeList[i]) << endl ;
			cout << "links_count: " << ib1.getLinksCount(inodeList[i]) << endl ;
			cout << "blocks: " << ib1.getBlocks(inodeList[i]) << endl ;
			cout << "block[]: " << ib1.getDataBlockList(inodeList[i]) << endl ;
		}
		else
		{
			cout << "inode: " << inodeList[i] << endl ;
			cout << "mode: " << ib2.getMode(inodeList[i]) << endl ;
			cout << "time: " << ib2.getTime(inodeList[i]) << endl ;
			cout << "ctime: " << ib2.getCtime(inodeList[i]) << endl ;
			cout << "mtime: " << ib2.getMtime(inodeList[i]) << endl ;
			cout << "links_count: " << ib2.getLinksCount(inodeList[i]) << endl ;
			cout << "blocks: " << ib2.getBlocks(inodeList[i]) << endl ;
			cout << "block[]: " << ib2.getDataBlockList(inodeList[i]) << endl ;
		}

	}

}

void Display::printBlockNum(int bNum)
{
	FileSystem &fs = *FileSystem::getInstance();

	if ( bNum == 0 )
	{
		displaySuperBlock();
	}
	else if ( bNum == 1 )
	{
		displayBlockDescriptorTable();
	}
	else if ( bNum == 2 )
	{
		fs.displayBlockBitmap();
	}
	else if ( bNum == 3 )
	{
		fs.displayInodeBitmap();
	}
	else if ( bNum == 4 )
	{
		// inode 정보에 맞게 내용 출력. 사용중인 inode에 대해서만 출력. 
		// block5에 있는 정보도 같이 출력. 
		displayInodeBlock();
	}
	else //데이터 블록 출력
	{

	}

}