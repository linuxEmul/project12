#ifndef PATHSTACK_H
#define PATHSTACK_H

#include <cstring>
class PathStack
{
private:
	char* stack[100];
	int top;
public:
	PathStack()
	{
		top = -1;
	}

	void push(char* data)
	{
		stack[++top] = data;
	}
	char* pop() {
		if (top != -1) {
			char* tmp = stack[top];
			stack[top] = NULL;
			top--;
			return tmp;
		}
		return NULL;
	}
	char* popFirst()
	{
		if (top != -1)
		{
			char* tmp = stack[0];
			

			for (int i = 0; i < top; i++)
				stack[i] = stack[i + 1];
			stack[top] = NULL;
			top--;
			return tmp;
		}
		return NULL;
	}
	bool isEmpty()
	{
		return top == -1;
	}
	void initialize()
	{
		for (int i = 0; i < 100; i++)
		{
			stack[i] = NULL;
		}
	}
};
#endif