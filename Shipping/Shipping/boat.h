#pragma once
#include <string>

class boat
{
public:
	int x;
	int y;
	float angle = 0;

	std::string spriteaddr = "boat.png";
	olc::Sprite sprite;
	olc::Decal decal = olc::Decal(&sprite);
	std::string name = "POLARIS NST-B03-RTF";
	std::string allegiance = "INDEPENDANT";

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

