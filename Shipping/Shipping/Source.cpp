#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "tile.h"
#include <map>
#include <fstream>
#include "nlohmann/json.hpp"
#include "boat.h"
#include "port.h"
#include "person.h"
#include "constants.h"
#include "PerlinNoise.hpp"
#include "UI.h";

using json = nlohmann::json;

#define MAP_TOTAL_SIZE_X 18950
#define MAP_TOTAL_SIZE_Y 18950

int SEED = 34413;

typedef unsigned char byte;
typedef unsigned short word;

std::map<std::tuple<int,int>, port> portMap;

std::map<int, person> personMap;

std::map<std::string, tile> tileTypeMap;

std::map<std::string, UI_Container> UIMap;

std::map<std::tuple<int, int>, tile*> screenView;
const siv::PerlinNoise::seed_type seed = 123456u;

const siv::PerlinNoise perlin{ seed };

std::list<std::tuple<int, int>> islands;

std::map<std::tuple<int, int>, tile*> generateIsland(int x, int y, int px, int py) {

	std::map<std::tuple<int, int>, tile*> islandMap;

	srand(x+y);

	struct blob {
		int x;
		int y;
		int radius;
		bool subtract;
	};

	std::list<blob> blobs;

	for (int i = 0; i < 6; i++) {
		blob b;
		b.x = -6 + rand() % 6;
		b.x += x;
		b.y = -6 + rand() % 6;
		b.y += y;
		b.radius = rand() % 8;
		bool s = (rand()) % 3==0;
		if (b.radius < 2 && s)
			b.subtract == true;
		else
			b.subtract == false;
		blobs.push_back(b);
	}

	for (int i = x-28; i < x+28; i++) {
		for (int j = y-28; j < y+28; j++) {
			bool found = false;
			for (blob b : blobs) {
				int xs = pow((i - b.x),2);
				int ys = pow((j - b.y),2);

				double d = sqrt(xs + ys);
				
				if (ceil(d) == b.radius) {
					if(b.subtract)
						islandMap[{i, j}] = &tileTypeMap["defaultL"];
					else
						islandMap[{i, j}] = &tileTypeMap["defaultS"];
				}
				if (d < b.radius) {
					if (b.subtract)
						islandMap[{i, j}] = &tileTypeMap["defaultL"];
					else
						islandMap[{i, j}] = &tileTypeMap["defaultS"];
					found = true;
					break;
				}
			}
			if(!found)
				islandMap[{i, j}] = &tileTypeMap["defaultS"];
		}
	}
	
	for (int i = x-27; i < x+27; i++) {
		for (int j = y-27; j < y+27; j++) {
			if (islandMap[{i, j}]->type == "defaultl") {
				byte v = 0;
				
				v += 0b0001 * (islandMap[{i, j-1}]->type != "defaults");
				v += 0b0010 * (islandMap[{i+1, j}]->type != "defaults");
				v += 0b0100 * (islandMap[{i, j+1}]->type != "defaults");
				v += 0b1000 * (islandMap[{i-1, j}]->type != "defaults");
				

				int xp = i+1;
				int xm = i - 1;
				int yp = j + 1;
				int ym = j - 1;

				islandMap[{i, j}] = &tileTypeMap["tile" + std::to_string((int)v)];
			}
		}
	}
	

	return islandMap;
}

std::map<std::tuple<int, int>, tile*> islandMap;

tile* generateTile(int x, int y) {
	/*
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
	*/

	
	/*
	const double noise = perlin.octave2D_01((x), (y), 2);
	double nnoise = noise * 1000;
	if (x < 0 || y < 0) {
		return &tileTypeMap["edge"];
	}
	if (nnoise > 600) {
		return &tileTypeMap["default5"];
	}
	if (nnoise > 580) {
		return &tileTypeMap["default4"];
	}
	if (nnoise < 560) {
		return &tileTypeMap["default3"];
	}
	if (nnoise < 540) {
		return &tileTypeMap["default2"];
	}
	if (nnoise < 520) {
		return &tileTypeMap["default1"];
	}
	if (nnoise < 500) {
		return &tileTypeMap["default0"];
	}*/

	//Roughly O(1) complexity
	if (x < 0 || y < 0 || x > MAP_TOTAL_SIZE_X || y > MAP_TOTAL_SIZE_Y) {
		return &tileTypeMap["edge"];
	}

	if (!islandMap.empty()) {
		return(islandMap[{x, y}]);
	}
	else{
		return &tileTypeMap["defaultS"];
	}


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

	olc::Sprite portSprite;

	byte menueOpen = PORT; //ENUM(map,port,)
	std::tuple<int, int> CurrentIslandCoord;

	bool drawsells = false;

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		tileTypeMap["default"] = tile(0, 0, "default", "default.png");
		tileTypeMap["defaultL"] = tile(0, 0, "defaultl", "default.png");
		tileTypeMap["defaultS"] = tile(0, 0, "defaults", "tile0s.png");
		tileTypeMap["tile0"] = tile(0, 0, "default0", "tile00.png");
		tileTypeMap["tile1"] = tile(0, 0, "default1", "tile1.png");
		tileTypeMap["tile2"] = tile(0, 0, "default2", "tile2.png");
		tileTypeMap["tile3"] = tile(0, 0, "default3", "tile03.png");
		tileTypeMap["tile4"] = tile(0, 0, "default4", "tile4.png");
		tileTypeMap["tile5"] = tile(0, 0, "default5", "tile5.png");
		tileTypeMap["tile6"] = tile(0, 0, "default6", "tile06.png");
		tileTypeMap["tile7"] = tile(0, 0, "default7", "tile07.png");
		tileTypeMap["tile8"] = tile(0, 0, "default8", "tile8.png");
		tileTypeMap["tile9"] = tile(0, 0, "default9", "tile09.png");
		tileTypeMap["tile10"] = tile(0, 0, "default10", "tile10.png");
		tileTypeMap["tile11"] = tile(0, 0, "default11", "tile011.png");
		tileTypeMap["tile12"] = tile(0, 0, "default12", "tile012.png");
		tileTypeMap["tile13"] = tile(0, 0, "default13", "tile013.png");
		tileTypeMap["tile14"] = tile(0, 0, "default14", "tile014.png");
		tileTypeMap["tile15"] = tile(0, 0, "default15", "tile015.png");
		tileTypeMap["edge"] = tile(0, 0, "default", "edge.png");

		player = boat(40000, 45000);
		portSprite = olc::Sprite("port.png");
		
		UIMap["portUI"] = UI_Container("portUI");

		UIMap["portUI"].add(UI_Element(0, 0, ElementType::IMAGE, "", &portSprite));
		UIMap["portUI"].add(UI_Element(0, 0, ElementType::IMAGE, "", &portSprite));


		//                                  READ PEOPLE AND ISALNDS FROM FILE
		//==========================================================================================================
		std::fstream fpeople("people.json");
		std::fstream fislands("islands.json");

		json peopleData = json::parse(fpeople);
		for(auto it : peopleData)
			personMap[it["id"]] = person(it["name"], it["title"], it["faction"],it["sprite"],it["initialText"]);

		json islandsData = json::parse(fislands);
		for (auto it : islandsData) {
			portMap[{it["x"], it["y"]}] = port(it["x"], it["y"], it["name"], &personMap[it["leader"]]);
			if (it.count("sells"))
				portMap[{it["x"], it["y"]}].sells = it["sells"];
		}
		//==========================================================================================================

		//player.currentPort = &portMap[{302, 108}];
		//islandMap = generateIsland(player.currentPort->x, player.currentPort->y);

		islandMap.clear();

		//Create initial 11x11 tile map
		for (int i = -5; i <= 5; i++) {
			for (int j = -5; j <= 5; j++) {
				screenView[{i, j}] = generateTile(ceil(player.x/50) + i, ceil(player.y/50) + j);
			}
		}

		boatsprite = olc::Sprite("boat.png");
		boatdecal.Update();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		

		if (menueOpen == MAP) {
			SetPixelMode(olc::Pixel::MASK);
			int cornerx = player.x - 250;
			int cornery = player.y - 250;

			int boatTilex = ceil(player.x / 50) - 1;
			int boatTiley = ceil(player.y / 50) - 1;


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
				float angleDeg = player.angle * (180 / PI);

				float o = sin(player.angle - (PI / 2));
				o *= player.speed * fElapsedTime;
				float a = cos(player.angle - (PI / 2));
				a *= player.speed * fElapsedTime;

				player.x += a;
				player.y += o;
			};
			if (GetKey(olc::DOWN).bHeld) {
				float angleDeg = player.angle * (180 / PI);

				float o = sin(player.angle - (PI / 2));
				o *= player.speed * fElapsedTime;
				float a = cos(player.angle - (PI / 2));
				a *= player.speed * fElapsedTime;

				player.x -= a;
				player.y -= o;
			};
			if (GetKey(olc::RIGHT).bHeld) { player.angle += (4 * (double)fElapsedTime); };
			if (GetKey(olc::LEFT).bHeld) { player.angle -= (4 * (double)fElapsedTime); };

			boatTilex = ceil(player.x / 50) - 1;
			boatTiley = ceil(player.y / 50) - 1;


			//ENTER NEW TILE
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

				std::map<std::tuple<int, int>, port>::iterator it;

				for (it = portMap.begin(); it != portMap.end(); it++) {
					if (boatTilex >= it->second.x - 20 && boatTilex <= it->second.x + 20 && boatTiley >= it->second.y - 20 && boatTiley <= it->second.y + 20) {
						if (player.currentPort == nullptr) {
							player.currentPort = &it->second;
							islandMap = generateIsland(player.currentPort->x, player.currentPort->y,boatTilex,boatTiley);
						}
						else {
							if (boatTilex == it->second.x - 20 || boatTilex == it->second.x + 20 || boatTiley == it->second.y - 20 || boatTiley == it->second.y + 20) {
								player.currentPort = nullptr;
								islandMap.clear();
							}
						}
						break;
					}
				}

				for (int i = -5; i <= 5; i++) {
					for (int j = -5; j <= 5; j++) {
						newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
					}
				}
				screenView = newView;

				/*
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
					//Move tiles up and right
					for (int i = -5; i <= 5; i++) {
						for (int j = -5; j <= 5; j++) {
							if (j < 5) {
								newView[{i, j}] = screenView[{i, j + 1}];
							}
							if (j == 5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
							if (i > -5) {
								newView[{i, j}] = screenView[{i - 1, j}];
							}
							if (i == -5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
						}
					}
					break;
				case 0xFF01:
					//Move tiles up and left
					for (int i = -5; i <= 5; i++) {
						for (int j = -5; j <= 5; j++) {
							if (j < 5) {
								newView[{i, j}] = screenView[{i, j + 1}];
							}
							if (j == 5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
							if (i < 5) {
								newView[{i, j}] = screenView[{i + 1, j}];
							}
							if (i == 5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
						}
					}
					break;
				case 0x01FF:
					//Move tiles down and left
					for (int i = -5; i <= 5; i++) {
						for (int j = -5; j <= 5; j++) {
							if (j > -5) {
								newView[{i, j}] = screenView[{i, j - 1}];
							}
							if (j == -5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
							if (i < 5) {
								newView[{i, j}] = screenView[{i + 1, j}];
							}
							if (i == 5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
						}
					}
					break;
				case 0xFFFF:
					//Move tiles down and right
					for (int i = -5; i <= 5; i++) {
						for (int j = -5; j <= 5; j++) {
							if (j > -5) {
								newView[{i, j}] = screenView[{i, j - 1}];
							}
							if (j == -5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
							if (i > -5) {
								newView[{i, j}] = screenView[{i - 1, j}];
							}
							if (i == -5) {
								newView[{i, j}] = generateTile(boatTilex + i, boatTiley + j);
							}
						}
					}
					break;
				}
				*/
			}


			DrawString(10, 10, std::to_string(player.x) + " (" + std::to_string(boatTilex) + "," + std::to_string(boatTiley) + ")");
			DrawString(10, 30, std::to_string(player.y));
			DrawString(10, 50, screenView[{0,0}]->type);
			if(player.currentPort != nullptr)
				DrawString(10, 70, player.currentPort->name);
			double lat = MAX_LAT - (player.y * COORD_INC);
			double lon = (player.x * COORD_INC) + MIN_LON;

			DrawString(10, 90, std::to_string(lat) + " , " + std::to_string(lon));

			//DrawRect(245,240,10,20);

			//DrawSprite(245, 240, &player.sprite);
			DrawRotatedDecal({ 250,250 }, &boatdecal, player.angle, { 5,10 }, { 1,1 });

			if (GetKey(olc::P).bPressed) {
				menueOpen = PORT;
			}
		}
		if(menueOpen==PORT){
			if (player.currentPort != nullptr) {
				SetPixelMode(olc::Pixel::ALPHA);

				DrawSprite(0, 0, &portSprite);
				FillRect(10, 10, 480, 380, { 128,128,128,160 });
				if (!drawsells) {
					FillRect(25, 50, 30, 20, { 128,128,128,200 });
					DrawString(25, 50, "Buy");
				}
				else {
					std::map<std::string, float>::iterator it;
					int yoffset = 50;
					for (it = player.currentPort->sells.begin(); it != player.currentPort->sells.end(); it++) {
						DrawString(25,yoffset,it->first + " - $"+std::to_string(it->second));
						yoffset += 20;
					}
				}


				DrawString(25, 25, player.currentPort->name, olc::WHITE, 2);

				DrawSprite(0, 400, &player.currentPort->leader->sprite);
				DrawString(110, 410, player.currentPort->leader->name);
				DrawString(110, 430, player.currentPort->leader->initialText, olc::WHITE);

				

				if (GetMouse(0).bPressed) {
					if ((GetMouseX() >= 25 && GetMouseX() <= 105) && (GetMouseY() >= 50 && GetMouseY() <= 90)) {
						drawsells = true;
					}
				}
			}
			if (GetKey(olc::ESCAPE).bPressed) {
				menueOpen = MAP;
				drawsells = false;
			}
			if (GetKey(olc::BACK).bPressed) {
				drawsells = false;
			}
		}

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

