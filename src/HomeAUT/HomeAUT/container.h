#pragma once
#include <Arduino.h>
template<class T>
class container
{
	T** items;
	int number_of_items;
public:
	void add(T* item) {
		T** temp = new T * [number_of_items + 1];
		memcpy(temp, items, number_of_items * sizeof(T*));
		temp[number_of_items] = item;
		delete items;
		items = temp;
		number_of_items++;
	}

	void del(T* item) {
		for (int k = 0; k < number_of_items; k++) {
			if (items[k] == item) {
				delByIndex(k);

				break;
			}
		}
	}
	void delByIndex(int index) {
		number_of_items--;

		if (number_of_items == 0) {
			items = NULL;
			return;
		}

		T** temp = new T * [number_of_items];

		memcpy(temp, items, index * sizeof(T*));
		memcpy(&temp[index], &items[index + 1], (number_of_items - index + 1) * sizeof(T*));

		delete items;
		items = temp;
	}

	T* get(int index){
		return items[index];
	}
	T** getAll() {
		return items;
	}
	int getSize() {
		return number_of_items;
	}
};

