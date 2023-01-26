#pragma once
#include "person.h"

class port
{
public:
	std::string name;
	int x;
	int y;
	olc::Sprite sprite;
	person* leader;
	std::map<std::string, float> sells;

	port() {
		name = "default";
		x = 0;
		y = 0;
		sprite = olc::Sprite("port.png");
	}

	port(int x, int y, std::string name, person* leader) {
		this->name = name;
		this->x = x;
		this->y = y;
		this->sprite = olc::Sprite("port.png");
		this->leader = leader;
	}
};

