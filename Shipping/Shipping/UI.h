#pragma once
#include <string>
#include <map>
#include "olcPixelGameEngine.h"

enum ElementType{ LABEL,PANEL,BUTTON,BLANK,IMAGE };

class UI_Element
{
public:
	int x;
	int w = 0;
	int y;
	int h = 0;
	int size=1;
	ElementType type;
	std::string text;
	std::tuple<int,int,int,int> colour;
	olc::Sprite* sprite = nullptr;

	bool hidden = false;
	void (*action)() = nullptr;

	UI_Element() {
		this->x = 0;
		this->y = 0;
		this->type = ElementType::BLANK;
		this->colour = { 0,0,0,0 };
		hidden = true;
	}

	UI_Element(int x, int y, ElementType type, olc::Sprite* sprite) {
		this->x = x;
		this->y = y;
		this->type = type;
		this->sprite = sprite;
	}

	UI_Element(int x, int y, int w, int h, ElementType type, std::tuple<int,int,int,int> colour) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->colour = colour;
	}

	UI_Element(int x, int y, ElementType type, std::string text, int size) {
		this->x = x;
		this->y = y;
		this->type = type;
		this->size = size;
	}

	UI_Element(int x, int y, ElementType type, std::string text, std::tuple<int,int,int,int> colour) {
		this->x = x;
		this->y = y;
		this->type = type;
		this->colour = colour;
	}

	

	UI_Element(int x, int y, ElementType type, std::string text, std::tuple<int, int, int, int> colour, void(*action)()) {
		this->x = x;
		this->y = y;
		this->type = type;
		this->colour = colour;
		this->action = action;
	}

	bool clicked(int mouseX, int mouseY) {
		if ((mouseX >= x && mouseX <= x+w) && (mouseY >= y && mouseY <= y+h)) {
			action();
		}
	}
};

class UI_Container
{
public:
	std::string tag;
	std::list<UI_Element> elements;

	UI_Container() {
		tag = "none";
	}

	UI_Container(std::string tag) {
		this->tag = tag;
	}

	void add(UI_Element element) {
		elements.push_back(element);
	}
};