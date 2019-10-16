#pragma once
#include <iostream>

//Should be in its own header but lazyness
struct GameObject {
	int id;
	int x;
	int y;
	int radius = 100;

	GameObject(int objID, int objX, int objY) {
		id = objID;
		x = objX;
		y = objY;
	}

	GameObject() {
		id = 0;
		x = 0;
		y = 0;
	}

	void moveCircle(int t) {
		x += radius * t;
		y += radius * t;
	}

	void randMove(int t) {
		x += (rand() % (t + 1)) * 10;
		y += (rand() % (t + 1)) * 10;
	}

	void update(int objX, int objY) {
		x = objX;
		y = objY;
	}

	void print() {
		std::cout << "ID: " << id << " x: " << x << " y: " << y << std::endl;
	}
};

