#include "pch.h"

CMyStack<int> g_stack;

void WorkerThreadMain_Consume()
{
	for (int i = 0; i < 3; ++i)
	{
		g_stack.pop();
	}
}

void WorkerThreadMain_Produce()
{
	for (int i = 0; i < 3; ++i)
	{
		g_stack.push(rand() % 100);
	}
}

int main(void)
{
	vector<thread> vecThreads;
	for (int32 i = 0; i < 4; ++i)
	{
		if(i % 2)
			vecThreads.emplace_back(thread(WorkerThreadMain_Consume));
		else
			vecThreads.emplace_back(thread(WorkerThreadMain_Produce));
	}

	for (int32 i = 0; i < 4; ++i)
	{
		vecThreads[i].join();
	}

	return 0;
}