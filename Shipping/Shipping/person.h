#pragma once
class person
{
public:
	std::string name;
	std::string title;
	std::string faction;
	olc::Sprite sprite;
	std::string initialText = "";

	person() {
		name = "default";
		title = "";
		faction = "Independant";
		sprite = olc::Sprite("person.png");
	}

	person(std::string name, std::string title, std::string faction, std::string sprite, std::string initialText) {
		this->name = name;
		this->title = title;
		this->faction = faction;
		this->sprite = olc::Sprite(sprite);
		this->initialText = initialText;
	}
};

