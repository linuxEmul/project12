
#include <memory.h>

#include "Directory.h"
#include "DataBlock.h"
#include "TableManager.h"

Directory::Directory()
{
	char currentDir[255] = ".";
	char preDir[255] = "..";

	memcpy(entryList[0].name, currentDir, 255);
	memcpy(entryList[1].name, preDir, 255);

	for (int i = 2; i < 20; i++) {
		memcpy(entryList[i].name, " ", 2);
		entryList[i].inodeNum=-1;
	}

	entryCnt = 2;

	//entryList[0].mode = 1; // ������ ���丮��� ���� ����.
}

void Directory::setInodeNum(int cur, int top) 
{
	entryList[0].inodeNum = cur; // ���� ���丮�� inode ��ȣ (.)
	entryList[1].inodeNum = top; // ���� ���丮�� inode ��ȣ (..)
}

void Directory::addDirectory(Entry entry, int inodeNum)
{
	//���� �߰��� entry�� ������ ��Ͽ� ���
	memcpy(&entryList[entryCnt], &entry, sizeof(Entry));
	entryCnt ++ ;

	//DataBlock dB;
	string content = entry.name;
	content += "," + entry.inodeNum;
	content += ";";
	

	/*
	for ( int i = 0 ; i < entryCnt; i ++ )
	{
		content +=  entryList[i].name;
		content += "," + entryList[i].inodeNum;
		content +=  ";";
	}
	*/
	
	FileSystem& fs = *FileSystem::getInstance();

	//memcpy(dB.data, (void *)content.c_str(), sizeof(content)); //�����ͺ�Ͽ� ������ �߰�
	
	//int fd = tmpTm.fileOpenEvent(inodeNum, fs.inodeBlock->getInodeData(inodeNum));
	//InodeElement* inodeE = ((InodeElement*)tmpTm.getElement(INODET, fd));
	Inode inodeData = fs.inodeBlock->getInodeData(inodeNum);
	int blocks = atoi(inodeData.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	// (FT)   fd�� Ÿ���� inode���� data block idx�޾ƿ���
	//InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	//Inode inodeData = inodeE->inode;
	//int blocks = atoi(inode.blocks);
	//int* dataIdx = new int[blocks];
	//translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char* blockData;
		fs.readFS(dataIdx[i], blockData);
		data += blockData;
	}
	/*
	(FS)   data block[idx]�� ����� ������ �о����
	�ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	*/
	string filedata = data + content;   // ���������ͺ��� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ��� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ���� blockdata�� ����
									   */
	int length = filedata.length();
	char fileSize[7];
	for (int i = 0; i <7; i++)
		fileSize[i] = ' ';
	itoa(length, fileSize);

	inodeData.size = fileSize;

	char* blockData;
	int new_Blocks = 0;
	while (length != 0)
	{
		if (length > BLOCK_SIZE)
		{
			blockData = new char[BLOCK_SIZE];
			strcpy(blockData, (filedata.substr(0, BLOCK_SIZE)).c_str());

			length -= BLOCK_SIZE;
		}

		else
		{
			blockData = new char[length];
			strcpy(blockData, (filedata.substr(0, length)).c_str());

			length = 0;
		}

		int returnIdx = fs.writeFS(blockData);
		/*
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ��� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ��� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
		write����  Datablock idxã�Ƽ� DataBlock�� ���� �� blockBitmap ����������� �׸��� idx ��ȯ���ִ� �ɷ�
		*/
		translateIntArrToCharArr(&returnIdx, inodeData.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inodeData.blocks, 1);
	/* �Ƹ� translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);�� ���� ����, �ȵǸ� �ٲ㼭 �غ���

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
	char currTime[100];
	getCurrentTime(currTime);
	inodeData.time = currTime;
	inodeData.mtime = currTime;

	fs.updateInode_writeFile(inodeNum, inodeData);
}

Entry* Directory::findName(char* dName)
{
	for ( int i = 0; i < entryCnt; i ++ )
	{
		if ( dName == entryList[i].name )
			return &entryList[i];
	}
	return NULL;
}

void Directory::rmDirectory(int inodeNum)
{
	FileSystem& fs = *FileSystem::getInstance();

	for ( int i = 0; i < entryCnt; i ++ )
	{
		if ( inodeNum == entryList[i].inodeNum )
		{
			for ( int j = i ; j < entryCnt; j ++)
				entryList[i] = entryList[i+1];
			break;
		}
	}
	Inode inodeData = fs.inodeBlock->getInodeData(inodeNum);
	int blocks = atoi(inodeData.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	// (FT)   fd�� Ÿ���� inode���� data block idx�޾ƿ���
	//InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	//Inode inodeData = inodeE->inode;
	//int blocks = atoi(inode.blocks);
	//int* dataIdx = new int[blocks];
	//translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	string data = "";
	//for (int i = 0; i < blocks; i++)
	//{
	//	char* blockdata;
	//	fs.readfs(dataidx[i], blockdata);
	//	data += blockdata;
	//}
	for (int i = 0; i < entryCnt; i++)
	{
		data += entryList[i].name;
		data += "," + entryList[i].inodeNum;
		data += ";";
	}
	/*
	(FS)   data block[idx]�� ����� ������ �о����
	�ϳ��� data���� �� �����ͺ��� �л��� �͵��� ��ħ
	*/
	string filedata = data;   // ���������ͺ��� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ��� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ���� blockdata�� ����
									   */
	int length = filedata.length();
	char fileSize[7];
	for (int i = 0; i <7; i++)
		fileSize[i] = ' ';
	itoa(length, fileSize);

	inodeData.size = fileSize;

	char* blockData;
	int new_Blocks = 0;
	while (length != 0)
	{
		if (length > BLOCK_SIZE)
		{
			blockData = new char[BLOCK_SIZE];
			strcpy(blockData, (filedata.substr(0, BLOCK_SIZE)).c_str());

			length -= BLOCK_SIZE;
		}

		else
		{
			blockData = new char[length];
			strcpy(blockData, (filedata.substr(0, length)).c_str());

			length = 0;
		}

		int returnIdx = fs.writeFS(blockData);
		/*
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ��� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ��� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
		write����  Datablock idxã�Ƽ� DataBlock�� ���� �� blockBitmap ����������� �׸��� idx ��ȯ���ִ� �ɷ�
		*/
		translateIntArrToCharArr(&returnIdx, inodeData.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inodeData.blocks, 1);
	/* �Ƹ� translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);�� ���� ����, �ȵǸ� �ٲ㼭 �غ���

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block �� size, time(���� ������ ���ٽð�), mtime(������ ���������� ������ �ð�) blocks, block[] ���� -> FT, FS ��� ���� */
	char* currTime;
	getCurrentTime(currTime);
	inodeData.time = currTime;
	inodeData.mtime = currTime;

	fs.updateInode_writeFile(inodeNum, inodeData);
}

bool Directory::isExist(char *dName)
{
	for ( int i = 0 ; i < entryCnt; i ++ )
	{
		if ( entryList[i].name == dName)
		{
			return true;
		}
	}
	return false;
}