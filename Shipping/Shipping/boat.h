#pragma once
#include <string>
#include "constants.h"
#include "port.h"

class boat
{
public:
	double x;
	double y;
	double angle = 0;
	float speed = 177.46; //5.2 = 8knots
	float ballance = 500.0;
	std::map<std::string, float> inventory;

	std::string spriteaddr = "boat.png";
	olc::Sprite sprite;
	olc::Decal decal = olc::Decal(&sprite);
	std::string name = "POLARIS NST-B03-RTF";
	std::string allegiance = "INDEPENDANT";
	port* currentPort = nullptr;

	boat() {
		this->x = x;
		this->y = y;
		this->sprite = olc::Sprite("default.png");
		decal = olc::Decal(&sprite);
	}

	boat(int x, int y) {
		this->x = x;
		this->y = y;
		this->sprite = olc::Sprite("boat.png");
		decal = olc::Decal(&sprite);
	}
};

