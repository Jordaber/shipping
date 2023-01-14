#pragma once
#include <string>

#define WIDTH 50
#define HEIGHT 50

/*

	List of Tile Types (For referencing in sprites map)

	default - "default.bmp"

	deepOcean
	shallowOcean
	reefOcean
	rocksOcean

*/


class tile
{
public:
	std::string type = "default";
	float fTemp = 8.0f;
	olc::Sprite sprite;

	bool isAnimated = false;
	int x;
	int y;

	tile() {
		this->x = 0;
		this->y = 0;
		this->sprite = olc::Sprite("default.png");
	}

	tile(int x, int y, std::string type, std::string sprite) {
		this->x = x;
		this->y = y;
		this->type = type;
		this->sprite = olc::Sprite(sprite);
	}
};

