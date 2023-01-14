#pragma once
#include <string>
class boat
{
public:
	int x;
	int y;

	std::string name = "POLARIS NST-B03-RTF";
	std::string allegiance = "INDEPENDANT";

	boat(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

