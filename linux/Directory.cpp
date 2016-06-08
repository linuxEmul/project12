
#include <memory.h>
#include <string>
#include "Directory.h"
#include "DataBlock.h"
#include "TableManager.h"

Directory::Directory()
{
	char currentDir[255] = ".";
	char preDir[255] = "..";

	//memcpy(entryList[0].name, currentDir, 255);
	//memcpy(entryList[1].name, preDir, 255);

	for (int i = 2; i < 20; i++) {
		memcpy(entryList[i].name, " ", 2);
		entryList[i].inodeNum=-1;
	}

	entryCnt = 0;

	//entryList[0].mode = 1; // ������ ���丮��� ���� ����.
}

void Directory::setInodeNum(int cur, int top) 
{
	/*
	entryList[0].inodeNum = cur; // ���� ���丮�� inode ��ȣ (.)
	entryList[1].inodeNum = top; // ���� ���丮�� inode ��ȣ (..)
	strcpy(entryList[0].name, ".");
	strcpy(entryList[1].name, "..");
	*/
	Entry curE, topE;
	curE.inodeNum = cur;
	strcpy(curE.name, ".");
	topE.inodeNum = top;
	strcpy(topE.name, "..");

	//�ʱ� .�� ..�� �߰��� ���� addDirectory �Լ��� �̿��Ѵ�.
	//LinkCount ������ ����
	addDirectory(curE, cur);
	addDirectory(topE, cur);
}

void Directory::addDirectory(Entry entry)
{
	if (isExist(entry.name))
		return;

	//���� �߰��� entry�� ������ ���Ͽ� ���
	memcpy(&entryList[entryCnt], &entry, sizeof(Entry));
	entryCnt++;
}

void Directory::addDirectory(Entry entry, int inodeNum)
{
	if (isExist(entry.name))
		return;

	//���� �߰��� entry�� ������ ���Ͽ� ���
	memcpy(&entryList[entryCnt], &entry, sizeof(Entry));
	entryCnt ++ ;

	//DataBlock dB;
	string content = entry.name;
	content.append(",");
	content.append(to_string(entry.inodeNum));
	content.append(";");

	/*
	for ( int i = 0 ; i < entryCnt; i ++ )
	{
		content +=  entryList[i].name;
		content += "," + entryList[i].inodeNum;
		content +=  ";";
	}
	*/
	
	FileSystem& fs = *FileSystem::getInstance();

	//memcpy(dB.data, (void *)content.c_str(), sizeof(content)); //�����ͺ��Ͽ� ������ �߰�
	
	//int fd = tmpTm.fileOpenEvent(inodeNum, fs.inodeBlock->getInodeData(inodeNum));
	//InodeElement* inodeE = ((InodeElement*)tmpTm.getElement(INODET, fd));
	Inode inodeData = fs.inodeBlock->getInodeData(inodeNum);
	int blocks = atoi(inodeData.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	// (FT)   fd�� Ÿ������ inode���� data block idx�޾ƿ���
	//InodeElement* inodeE = ((InodeElement*)tm.getElement(INODET, fd));
	//Inode inodeData = inodeE->inode;
	//int blocks = atoi(inode.blocks);
	//int* dataIdx = new int[blocks];
	//translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	string data = "";
	for (int i = 0; i < blocks; i++)
	{
		char blockData[BLOCK_SIZE];
		fs.readFS(dataIdx[i], blockData);
		data +=blockData;
	}
	/*
	(FS)   data block[idx]�� ����� ������ �о����
	�ϳ��� data���� �� �����ͺ����� �л��� �͵��� ��ħ
	*/
	string filedata = data + content;   // ���������ͺ����� ����� ���ο� ������ ��ģ ��
	filedata = trim(filedata);
	cout << "dataIdx : "<< dataIdx << endl;
	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ����� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ��������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ������ blockdata�� ����
									   */

	int length = filedata.length();
	char fileSize[7];
	for (int i = 0; i <7; i++)
		fileSize[i] = ' ';
	_itoa(length, fileSize);

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
		//fs.resetDataBlock(dataIdx, blocks);
		int returnIdx = fs.writeFS(blockData);
		cout << "returnIdx : " << returnIdx << endl;
		/*
		fs.writeFS ������
		block descriptor Table ���Ҵ�� ���� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ����� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
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

	//�ش� ��Ʈ���� ���ΰ��� linkCount�� �÷��ְ� �����Ѵ�.
	Inode tmp = fs.inodeBlock->getInodeData(entry.inodeNum);
	_itoa(atoi(tmp.linksCount) + 1, tmp.linksCount);
	fs.updateInode_writeFile(entry.inodeNum, tmp);
	
	if(entry.inodeNum != inodeNum)
		fs.updateInode_writeFile(inodeNum, inodeData);
}

Entry* Directory::findName(char* dName)
{
	for ( int i = 0; i < entryCnt; i ++ )
	{
		if (strcmp(dName, entryList[i].name) == 0)
			return &entryList[i];
	}
	return NULL;
}

void Directory::rmDirectory(int inodeNum, int myInodeNum)
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
	entryCnt--;

	//�� inode�� entry���� ���� �����
	Inode inodeData = fs.inodeBlock->getInodeData(myInodeNum);
	int blocks = atoi(inodeData.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	// (FT)   fd�� Ÿ������ inode���� data block idx�޾ƿ���
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
		data += "," + to_string(entryList[i].inodeNum);
		data += ";";
	}
	/*
	(FS)   data block[idx]�� ����� ������ �о����
	�ϳ��� data���� �� �����ͺ����� �л��� �͵��� ��ħ
	*/
	string filedata = data;   // ���������ͺ����� ����� ���ο� ������ ��ģ ��

	fs.resetDataBlock(dataIdx, blocks);// writeFile���� writeFS�� �θ��� ���� BlockBitmap�� ���Ͽ� �Ҵ��� indxe �ʱ�ȭ
									   /* ���� BlockBitmap�� 1�� ��츸 �����ͺ����� ����Ǿ��ִ� ���̹Ƿ� getDatablock�� blockbitmap�� 1���� �˻� �� ����� data return */

									   /*
									   ������ data�� ��������� �°� �ɰ��� (�ݺ���)
									   writeFS�� �Ѱܼ� �������ش�
									   (FS)   block idx�� blockdata�� �Ѱܼ�  writeFS������ �ش� ������ ������ blockdata�� ����
									   */
	int length = filedata.length();
	char fileSize[7];
	for (int i = 0; i <7; i++)
		fileSize[i] = ' ';
	_itoa(length, fileSize);

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
		block descriptor Table ���Ҵ�� ���� ��, -> �ʱ�ȭ�Ҷ��� �ٽ� ���� �� ��������� ��
		Block Bitmap�� idx�� �ش��ϴ� ��Ʈ 1�� set -> write���� �����ͺ����� �� �� �ٷ� �ٷ� ���ִ� �ɷ�
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
	char currTime[13];
	getCurrentTime(currTime);
	inodeData.time = currTime;
	inodeData.mtime = currTime;
	_itoa(atoi(inodeData.linksCount) - 1, inodeData.linksCount);

	fs.updateInode_writeFile(myInodeNum, inodeData);

	//myInodeNum ��, ������ ������ ���� ���丮���� ��Ʈ�� ���� �� ��ũ�� ����
	
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