#include "CommonLibrary.h"

/*   FileSystem   */
char* getData(char* data, int size)
{
	char *buffer = new char[size];
	memcpy(buffer, data, size - 1);
	buffer[size - 1] = '\0';

	return buffer;
}

/*   FileSystem   */

/*   Path   */
vector<string>* tokenize(const string& str, const string& delimiters)
{
	vector<string>* tokens = new vector<string>;
	// �� ù ���ڰ� �������� ��� ����
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// �����ڰ� �ƴ� ù ���ڸ� ã�´�
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// token�� ã������ vector�� �߰��Ѵ�
		tokens->push_back(str.substr(lastPos, pos - lastPos));
		// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
		lastPos = str.find_first_not_of(delimiters, pos);
		// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}

char* stringToCharArr(string arr)
{
	char *cstr = new char[arr.length() + 1];
	strcpy(cstr, arr.c_str());
	return cstr;
}
/*   Path   */

/*   File   */
char* getCurrentTime()
{
	char* currTime = new char[13];
	time_t timer;
	struct tm t;

	timer = time(NULL);         //  ���� �ð��� �� ������ ���
	localtime_s(&t, &timer);      // �� ������ �ð��� �ж��Ͽ� ����ü�� �ֱ�

	string str = to_string(t.tm_year + 1900);
	if (t.tm_mon < 10)
		str += "0" + to_string(t.tm_mon+1);
	else str += to_string(t.tm_mon);

	if (t.tm_mday < 10)
		str += "0" + to_string(t.tm_mday);
	else str += to_string(t.tm_mday);

	if (t.tm_hour < 10)
		str += "0" + to_string(t.tm_hour);
	else str += to_string(t.tm_hour);

	if (t.tm_min < 10)
		str += "0" + to_string(t.tm_min);
	else str += to_string(t.tm_min);

	for (int i = 0; i < 12; i++)
	{
		currTime[i] = str.at(i);
	}
	currTime[12] = '\0';
	return currTime;
	//currTime = (char*)str.c_str();
}

void getCurrentTime(char* currTime)
{
	time_t timer;
	struct tm t;

	timer = time(NULL);         //  ���� �ð��� �� ������ ���
	localtime_s(&t, &timer);      // �� ������ �ð��� �ж��Ͽ� ����ü�� �ֱ�

	string str = to_string(t.tm_year + 1900);
	if (t.tm_mon < 10)
		str += "0" + to_string(t.tm_mon+1);
	else str += to_string(t.tm_mon);

	if (t.tm_mday < 10)
		str += "0" + to_string(t.tm_mday);
	else str += to_string(t.tm_mday);

	if (t.tm_hour < 10)
		str += "0" + to_string(t.tm_hour);
	else str += to_string(t.tm_hour);

	if (t.tm_min < 10)
		str += "0" + to_string(t.tm_min);
	else str += to_string(t.tm_min);

	for ( int i = 0 ; i < 12; i++ )
	{
		currTime[i] = str.at(i);
	}
	//currTime = (char*)str.c_str();
}

void translateCharArrToIntArr(char* charArr, int* intArr, int count)
{
	char* pCharArr = charArr;
	for (int i = 0; i < count; i++)
	{
		*(pCharArr + 2) = '\0';
		intArr[i] = atoi(pCharArr);
		pCharArr += 3;
	}
}// 

void translateIntArrToCharArr(int* intArr, char* charArr, int count)
{
	char* pCharArr = charArr;
	for (int i = 0; i < count; i++)
		itoa(intArr[i], pCharArr + i * 3, 10);
}

void itoa(int num, char *pStr)
{
	int radix = 10;
	int deg = 1;
	int cnt = 0;
	int i;

	if (pStr == NULL) return;

	if (num < 0) {
		*pStr = '-';
		num *= -1;
		pStr++;
	}

	while (1) {
		if ((num / deg) > 0)
			cnt++;
		else
			break;

		deg *= radix;
	}
	deg /= radix;

	for (i = 0; i < cnt; i++, pStr++) {
		*pStr = (num / deg) + '0';
		num -= (num / deg) * deg;
		deg /= radix;
	}
}

/*   File   */