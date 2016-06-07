#pragma once
#include <iostream>

using namespace std;

class Exception
{
public:
	static enum ExceptionType {
		E1_INODE_ERROR, E99_IDONT_KNOW
	};
	static void processError(ExceptionType e);
	void process(ExceptionType e);
private:

};