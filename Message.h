#pragma once
#include <iostream>

struct Message {
	int x, y;
	int objectID;


	Message(int id, int xO, int yO) {
		objectID = id;
		x = xO;
		y = yO;
	}

	void print() {
		std::cout << "Object ID: " << objectID << "x: " << x << "y: " << y << std::endl;
	}
};