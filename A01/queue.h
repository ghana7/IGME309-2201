#pragma once
#include <iostream>
template <class T>
class Queue
{
public:
	//basic constructor
	Queue() {
		size = 0;
		arrLength = 1;
		array = new T[arrLength];
	}

	//copy constructor
	//  creates a copy of another Queue
	Queue(const Queue& other) {
		size = other.size;
		arrLength = other.arrLength;
		array = new T[arrLength];
		for (int i = 0; i < other.size; i++) {
			array[i] = other.array[i];
		}
	};

	//copy assignment operator
	//  sets a Queue to be a copy of another Queue
	Queue& operator=(const Queue& other) {
		if (this == &other) {
			return *this;
		}
		delete[] array;

		size = other.size;
		arrLength = other.arrLength;
		array = new T[arrLength];
		for (int i = 0; i < other.size; i++) {
			array[i] = other.array[i];
		}
		return *this;
	};

	//destructor
	//  deletes a Queue
	~Queue() {
		delete[] array;
		array = nullptr;

		std::cout << "destructor" << std::endl;
	};

	//pushes an item to the top of the Queue
	void Push(T item) {
		array[size] = item;

		size++;
		if (size >= arrLength) {
			GrowArray();
		}

		Sort();

	};

	//pops the top item of the Queue off and returns it
	//returns null if no items in the Queue
	T Pop() {
		if (size == 0) {
			return NULL;
		}
		else {
			size--;
			T returnValue = array[0];
			//shift items down the queue
			for (int i = 0; i < size + 1; i++) {
				if (i == size) {
					array[i] = NULL;
				}
				array[i] = array[i + 1];
			}
			return returnValue;
		}
	};
	//prints the Queue
	void Print() {
		for (int i = 0; i < size; i++) {
			std::cout << array[i] << ", ";
		}
		std::cout << std::endl;
	};
	//gets the size of the Queue
	int GetSize() {
		return size;
	};
	//returns if the Queue is empty or not
	bool IsEmpty() {
		return size == 0;
	};
private:
	int size;
	int arrLength;
	T* array;
	//a helper method that doubles the size of the internal array and copies all elements over
	void GrowArray() {
		T* newArray = new T[arrLength * 2];
		for (int i = 0; i < arrLength; i++) {
			newArray[i] = array[i];
		}
		delete[] array;
		array = newArray;
		newArray = nullptr;
		arrLength = arrLength * 2;
	}

	//a helper method for selection sort, finds the index of the lowest member of the array
	int FindMinIndex(int start) {
		int minIndex = start;
		int i = start + 1;

		while (i < size) {
			if (array[i] < array[minIndex]) {
				minIndex = i;
			}
			i++;
		}

		return minIndex;
	}
	//a simple selection sort
	void Sort() {
		for (int i = 0; i < size; i++) {
			int minIndex = FindMinIndex(i);
			if (i != minIndex) {
				std::swap(array[i], array[minIndex]);
			}
		}
	}
};

