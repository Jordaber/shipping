#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "tile.h"
#include <map>
#include "boat.h"

#define MAP_TOTAL_SIZE_X 18950
#define MAP_TOTAL_SIZE_Y 18950

# define PI 3.141592653589793238462643383279502884L

typedef unsigned char byte;
typedef unsigned short word;



std::map<std::string, tile> tileTypeMap;
std::map<std::tuple<int, int>, tile*> screenView;

tile* generateTile(int x, int y) {
	if (x < 0 || y < 0) {
		return &tileTypeMap["edge"];
	}
	if (x == 6 || x == 16 || y == 6 || y == 16) {
		return &tileTypeMap["default5"];
	}
	if (x == 7 || x == 15 || y == 7 || y == 15) {
		return &tileTypeMap["default4"];
	}
	if (x == 8 || x == 14 || y == 8 || y == 14) {
		return &tileTypeMap["default3"];
	}
	if (x == 9|| x == 13 || y == 9 || y == 13) {
		return &tileTypeMap["default2"];
	}
	if (x == 10 || x == 12 || y == 10 || y == 12) {
		return &tileTypeMap["default1"];
	}
	if (x == 11 && y == 11) {
		return &tileTypeMap["default0"];
	}

	return &tileTypeMap["default"];
}

class Example : public olc::PixelGameEngine
{
private:

public:
	Example()
	{
		sAppName = "Shipping Game very cool wooooooooow";
	}

	boat player;
	olc::Sprite boatsprite;

	olc::Decal boatdecal = olc::Decal(&boatsprite);

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		player = boat(525, 525);

		tileTypeMap["default"] = tile(0, 0, "default", "default.png");
		tileTypeMap["default5"] = tile(0, 0, "default", "default5.png");
		tileTypeMap["default4"] = tile(0, 0, "default", "default4.png");
		tileTypeMap["default3"] = tile(0, 0, "default", "default3.png");
		tileTypeMap["default2"] = tile(0, 0, "default", "default2.png");
		tileTypeMap["default1"] = tile(0, 0, "default", "default1.png");
		tileTypeMap["default0"] = tile(0, 0, "default", "default0.png");
		tileTypeMap["edge"] = tile(0, 0, "default", "edge.png");

		//Create initial 11x11 tile map
		for (int i = -5; i <= 5; i++) {
			for (int j = -5; j <= 5; j++) {
				screenView[{i, j}] = generateTile(ceil(player.x/50) + i, ceil(player.y/50) + j);
			}
		}

		boatsprite = olc::Sprite("boat.png");
		boatdecal = olc::Decal(&player.sprite);
		boatdecal.UpdateSprite();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		SetPixelMode(olc::Pixel::MASK);

		int cornerx = player.x - 250;
		int cornery = player.y - 250;

		int boatTilex = ceil(player.x / 50);
		int boatTiley = ceil(player.y / 50);

		std::map<std::tuple<int, int>, tile*>::iterator it;

		for (it = screenView.begin(); it != screenView.end(); it++) {
			int absolute_tile_positionx = boatTilex + std::get<0>(it->first);
			int absolute_tile_positiony = boatTiley + std::get<1>(it->first);

			int absolute_pixel_positionx = absolute_tile_positionx * 50;
			int absolute_pixel_positiony = absolute_tile_positiony * 50;

			int draw_positionx = absolute_pixel_positionx - cornerx;
			int draw_positiony = absolute_pixel_positiony - cornery;

			DrawSprite(draw_positionx, draw_positiony, &it->second->sprite);
		}

		int premoveTilex = boatTilex;
		int premoveTiley = boatTiley;

		if (GetKey(olc::UP).bHeld) { //player.y -= (10 * fElapsedTime * (player.y > 10));
			float angleDeg = player.angle * (180/PI);

			float o = sin(angleDeg) * 50 * fElapsedTime;
			float a = cos(angleDeg) * 50 * fElapsedTime;

			player.x += o;
			player.y += a;

		};
		if (GetKey(olc::DOWN).bHeld) { player.y += (50 * fElapsedTime * (player.y < 947500)); };
		if (GetKey(olc::RIGHT).bHeld) { player.x += (50 * fElapsedTime * (player.x < 947500)); };
		if (GetKey(olc::LEFT).bHeld) { player.x -= (10 * fElapsedTime * (player.x > 5)); };
		if (GetKey(olc::Q).bHeld) { player.angle -= (10 * fElapsedTime); };
		if (GetKey(olc::E).bHeld) { player.angle += (10 * fElapsedTime); };

		boatTilex = ceil(player.x / 50);
		boatTiley = ceil(player.y / 50);

		if ((premoveTilex != boatTilex) || (premoveTiley != boatTiley)) {
			byte xdiff = boatTilex - premoveTilex;
			byte ydiff = boatTiley - premoveTiley;

			unsigned short diff = ((unsigned short)xdiff << 8) + (unsigned short)ydiff;
			/*
				diff chart

				0001 moved down one tile (SOUTH)
				00FF moved up one tile (NORTH)
				0100 moved right one tile (EAST)
				FF00 moved left one tile (WEST)

				0101 moved down and right (SOUTH-EAST)
				FF01 moved down and left (SOUTH-WEST)
				01FF moved up and right (NORTH-EAST)
				FFFF moved up and left (NORTH-WEST)
			*/

			std::map<std::tuple<int, int>, tile*> newView;

			switch (diff) {
			case 0x0001:
				//Move all tiles up relative
				for (int i = -5; i <= 5; i++) {
					for (int j = -5; j <= 5; j++) {
						if (j < 5) {
							newView[{i, j}] = screenView[{i, j + 1}];
						}
						if (j == 5) {
							newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
						}
					}
				}
				screenView = newView;
				break;
			case 0x00FF:
				//Move all tiles down relative
				for (int i = -5; i <= 5; i++) {
					for (int j = -5; j <= 5; j++) {
						if (j > -5) {
							newView[{i, j}] = screenView[{i, j - 1}];
						}
						if (j == -5) {
							newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
						}
					}
				}
				screenView = newView;
				break;
			case 0x0100:
				//Move all tiles left relative
				for (int i = -5; i <= 5; i++) {
					for (int j = -5; j <= 5; j++) {
						if (i < 5) {
							newView[{i, j}] = screenView[{i + 1, j}];
						}
						if (i == 5) {
							newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
						}
					}
				}
				screenView = newView;
				break;
			case 0xFF00:
				//Move all tiles right relative
				for (int i = -5; i <= 5; i++) {
					for (int j = -5; j <= 5; j++) {
						if (i > -5) {
							newView[{i, j}] = screenView[{i - 1, j}];
						}
						if (i == -5) {
							newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
						}
					}
				}
				screenView = newView;
				screenView = newView;
				break;
			case 0x0101:
				break;
			case 0xFF01:
				break;
			case 0x01FF:
				break;
			case 0xFFFF:
				break;
			}
		}

		DrawString(10, 10, std::to_string(player.x) + " (" + std::to_string(boatTilex) + "," + std::to_string(boatTiley) + ")");
		DrawString(10, 30, std::to_string(player.y));

		//DrawRect(245,240,10,20);

		//DrawSprite(245, 240, &player.sprite);
		DrawRotatedDecal({ 245,240 }, &boatdecal, player.angle, { 5,10 }, {1,1});

		return true;
	}
};
 

int main()
{
	Example demo;
	if (demo.Construct(500, 500, 1, 1))
		demo.Start();

	return 0;
}

