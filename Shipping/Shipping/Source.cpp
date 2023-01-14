#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "tile.h"
#include <map>
#include "boat.h"

#define MAP_TOTAL_SIZE_X 18950
#define MAP_TOTAL_SIZE_Y 18950




std::map<std::string, tile> tileTypeMap;
std::map<std::tuple<int, int>, tile*> screenView;

boat player(525, 525);

tile* generateTile(int x, int y) {
	return &tileTypeMap["default"];
}

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Shipping Game very cool wooooooooow";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		tileTypeMap["default"] = tile(0, 0, "default", "default.png");

		for (int i = -6; i < 6; i++) {
			for (int j = -6; j < 6; j++) {
				screenView[{i, j}] = generateTile(i, j);
			}
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		
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

		if (GetKey(olc::UP).bHeld) { player.y -= (100 * fElapsedTime); };
		if (GetKey(olc::DOWN).bHeld) { 
			player.y += (100 * fElapsedTime); };
		if (GetKey(olc::RIGHT).bHeld) {
			player.x += (100 * fElapsedTime); };
		if (GetKey(olc::LEFT).bHeld) { player.x -= (100 * fElapsedTime); };

		DrawString(10,10,std::to_string(player.x));
		DrawString(10,30, std::to_string(player.y));

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

