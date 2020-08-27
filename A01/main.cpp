//James Harkins
//HW4
//4/19/2020

#include <iostream>
#include "queue.h"
#include "Alberto.h"
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

void wrapper() {
	std::cout << "int queue" << std::endl;
	Queue<int> intQueue = Queue<int>();
	intQueue.Push(3);
	intQueue.Push(7);
	intQueue.Push(9);
	intQueue.Push(11);
	intQueue.Push(23);
	intQueue.Print();

	intQueue.Push(8);
	intQueue.Print();

	intQueue.Pop();
	intQueue.Print();
	std::cout << std::endl;

	/* not sure if you have the alberto class in testing, so commenting out to be safe
	std::cout << "alberto queue" << std::endl;
	Queue<AlbertoClass> myAlbertoQueue;
	for (int i = 40; i > 30; --i)
	{
		AlbertoClass alberto(i);
		myAlbertoQueue.Push(alberto);
	}
	myAlbertoQueue.Print();
	std::cout << std::endl;
	*/
	std::cout << "bool queue" << std::endl;
	Queue<bool> boolQueue = Queue<bool>();
	boolQueue.Push(true);
	boolQueue.Push(false);
	boolQueue.Push(false);
	boolQueue.Push(false);
	boolQueue.Print();
	boolQueue.Pop();
	boolQueue.Pop();
	boolQueue.Pop();
	boolQueue.Pop();
	boolQueue.Pop();
	boolQueue.Print();
	std::cout << std::endl;

	std::cout << "char queue" << std::endl;
	Queue<char> charQueue = Queue<char>();
	charQueue.Push('q');
	charQueue.Push('d');
	charQueue.Push('f');
	charQueue.Push('r');
	charQueue.Print();
	charQueue.Pop();
	charQueue.Pop();
	charQueue.Print();
	std::cout << std::endl;

	std::cout << "copying int queue" << std::endl;
	Queue<int> copy = Queue<int>(intQueue);
	copy.Print();
	copy.Pop();
	copy.Print();
	copy = intQueue;
	copy.Print();

	std::getchar();
}
int main()
{
	wrapper();
	_CrtDumpMemoryLeaks();
}