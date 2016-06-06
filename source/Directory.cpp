
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

	entryCnt = 2;

	//entryList[0].mode = 1; // 종류가 디렉토리라는 것을 지정.
}

void Directory::setInodeNum(int cur, int top) 
{
	entryList[0].inodeNum = cur; // 현재 디렉토리의 inode 번호 (.)
	entryList[1].inodeNum = top; // 상위 디렉토리의 inode 번호 (..)
}

void Directory::addDirectory(Entry entry, int inodeNum)
{
	//새로 추가된 entry를 데이터 블록에 기록
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

	//memcpy(dB.data, (void *)content.c_str(), sizeof(content)); //데이터블록에 데이터 추가
	
	//int fd = tmpTm.fileOpenEvent(inodeNum, fs.inodeBlock->getInodeData(inodeNum));
	//InodeElement* inodeE = ((InodeElement*)tmpTm.getElement(INODET, fd));
	Inode inodeData = fs.inodeBlock->getInodeData(inodeNum);
	int blocks = atoi(inodeData.blocks);
	int* dataIdx = new int[blocks];
	translateCharArrToIntArr(inodeData.dataBlockList, dataIdx, blocks);

	// (FT)   fd를 타고가서 inode에서 data block idx받아오기
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
	(FS)   data block[idx]에 저장된 데이터 읽어오기
	하나의 data에다 각 데이터블럭에 분산됬던 것들을 합침
	*/
	string filedata = data + content;   // 기존데이터블럭의 내용과 새로운 내용을 합친 것

	fs.resetDataBlock(dataIdx, blocks);// writeFile에서 writeFS를 부르기 전에 BlockBitmap의 파일에 할당됬던 indxe 초기화
									   /* 참고 BlockBitmap이 1인 경우만 데이터블럭이 저장되어있는 것이므로 getDatablock은 blockbitmap이 1인지 검사 후 저장된 data return */

									   /*
									   합쳐진 data를 블럭사이즈에 맞게 쪼개서 (반복문)
									   writeFS에 넘겨서 저장해준다
									   (FS)   block idx와 blockdata를 넘겨서  writeFS에서는 해당 데이터 블럭에 blockdata를 쓴다
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
		fs.writeFS 에서는
		block descriptor Table 미할당된 블록 수, -> 초기화할때랑 다시 써줄 때 갱신해줘야 함
		Block Bitmap에 idx에 해당하는 비트 1로 set -> write에서 데이터블럭에 쓴 후 바로 바로 해주는 걸로
		write에서  Datablock idx찾아서 DataBlock에 저장 후 blockBitmap 설정해줘야함 그리고 idx 반환해주는 걸로
		*/
		translateIntArrToCharArr(&returnIdx, inodeData.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inodeData.blocks, 1);
	/* 아마 translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);와 같은 역할, 안되면 바꿔서 해보기

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block 의 size, time(파일 마지막 접근시간), mtime(파일이 마지막으로 수정된 시간) blocks, block[] 갱신 -> FT, FS 모두 갱신 */
	char* currTime;
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

	// (FT)   fd를 타고가서 inode에서 data block idx받아오기
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
	(FS)   data block[idx]에 저장된 데이터 읽어오기
	하나의 data에다 각 데이터블럭에 분산됬던 것들을 합침
	*/
	string filedata = data;   // 기존데이터블럭의 내용과 새로운 내용을 합친 것

	fs.resetDataBlock(dataIdx, blocks);// writeFile에서 writeFS를 부르기 전에 BlockBitmap의 파일에 할당됬던 indxe 초기화
									   /* 참고 BlockBitmap이 1인 경우만 데이터블럭이 저장되어있는 것이므로 getDatablock은 blockbitmap이 1인지 검사 후 저장된 data return */

									   /*
									   합쳐진 data를 블럭사이즈에 맞게 쪼개서 (반복문)
									   writeFS에 넘겨서 저장해준다
									   (FS)   block idx와 blockdata를 넘겨서  writeFS에서는 해당 데이터 블럭에 blockdata를 쓴다
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
		fs.writeFS 에서는
		block descriptor Table 미할당된 블록 수, -> 초기화할때랑 다시 써줄 때 갱신해줘야 함
		Block Bitmap에 idx에 해당하는 비트 1로 set -> write에서 데이터블럭에 쓴 후 바로 바로 해주는 걸로
		write에서  Datablock idx찾아서 DataBlock에 저장 후 blockBitmap 설정해줘야함 그리고 idx 반환해주는 걸로
		*/
		translateIntArrToCharArr(&returnIdx, inodeData.dataBlockList + new_Blocks * 3, 1);

		new_Blocks++;
	}

	translateIntArrToCharArr(&new_Blocks, inodeData.blocks, 1);
	/* 아마 translateIntArrToCharArr( &new_Blocks, inode.blocks, 1);와 같은 역할, 안되면 바꿔서 해보기

	char c_blocks[4];
	memcpy( c_blocks, &new_Blocks, sizeof(int) );
	string s_blocks = c_blocks;
	inode.blocks = new char[3];
	strcpy(inode.blocks, (s_blocks.substr(1, 3)).c_str());
	*/

	/* (FS)   Inode Block 의 size, time(파일 마지막 접근시간), mtime(파일이 마지막으로 수정된 시간) blocks, block[] 갱신 -> FT, FS 모두 갱신 */
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