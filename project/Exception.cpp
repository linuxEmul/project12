#include "Exception.h"

void Exception::processError(ExceptionType e)
{
	switch (e)
	{
	case Exception::E1_INODE_ERROR:
		cerr << "" << endl;
		break;
	case Exception::E99_IDONT_KNOW:
		break;
	default:
		break;
	}
}
