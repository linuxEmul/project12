#ifndef COMMON
#define COMMON

/*   FileSystem   */
#include <iostream>
#include <string.h>
//#include <sys/io.h>
#include <io.h>
#include <fcntl.h>

/*   Path   */
#include <vector>
#include <string>

/*   File   */
#include <time.h>

using namespace std;

/*   FileSystem   */
const int NUM_BLOCKS = 100;
const int BLOCK_SIZE = 4096;

/* SuperBlock 상수 */
const int magicNumLength = 9;
const int blockSizeLength = 5;
const int blockPerGroupLength = 4;
const int inodesNumLength = 3;
const int inodeSizeLength = 4;
const int finishTimeLength = 13;
const int firstDataBlockNumLength = 2;
const int rootInodeNumLength = 2;

/* BlockDescriptorTable 상수 */
const int blockBitmapBlockNumLength = 2;
const int inodeBitmapBlockNumLength = 2;
const int firstInodeTableBlockNumLength = 2;
const int unassignedBlockNumLength = 3;
const int unassignedInodeNumLength = 3;

/* Inode Blocks 상수 */
const int modeLength = 5;
const int sizeLength = 7; // 최대 94 * 4096
const int timeLength = 13;
const int ctimeLength = 13;
const int mtimeLength = 13;
const int linksCountLength = 2;
const int blocksLength = 3;
const int dataBlockList = 72;

char* getData(char* data, int size);
/*   FileSystem   */

/*   Directory   */
/*   Directory   */

/*   Path   */
vector<string>* tokenize(const string& str, const string& delimiters = " ");
char* stringToCharArr(string arr);
/*   Path   */

/*   File   */
void getCurrentTime(char* currTime);
void translateCharArrToIntArr(char* charArr, int* intArr, int count);
void translateIntArrToCharArr(int* intArr, char* charArr, int count);
void itoa(int num, char *pStr);

/*   File   */
#endif // !COMMON