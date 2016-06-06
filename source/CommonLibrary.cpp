#include "CommonLibrary.h"

/*   FileSystem   */
char* getData(char* data, int size)
{
	char *buffer = new char[size];
	strncpy(buffer, data, size - 1);
	buffer[size - 1] = '\0';

	return buffer;
}
/*   FileSystem   */

/*   Path   */
vector<string>* tokenize(const string& str, const string& delimiters)
{
	vector<string>* tokens = new vector<string>;
	// 맨 첫 글자가 구분자인 경우 무시
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// 구분자가 아닌 첫 글자를 찾는다
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// token을 찾았으니 vector에 추가한다
		tokens->push_back(str.substr(lastPos, pos - lastPos));
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of(delimiters, pos);
		// 다음 구분자가 아닌 글자를 찾는다
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
void getCurrentTime(char* currTime)
{
	time_t timer;
	struct tm t;

	timer = time(NULL);         //  현재 시각을 초 단위로 얻기
	localtime_s(&t, &timer);      // 초 단위의 시간을 분라하여 구조체에 넣기

	string str = to_string(t.tm_year + 1900);
	if (t.tm_mon < 10)
		str += "0" + to_string(t.tm_mon);
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

	currTime = (char*)str.c_str();
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