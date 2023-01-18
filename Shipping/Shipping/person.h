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

	person(std::string name, std::string title, std::string faction) {
		this->name = name;
		this->title = title;
		this->faction = faction;
		sprite = olc::Sprite("person.png");
		initialText = "Lorem ipsum dolor sit amet, consectetur\nadipiscing elit, sed do eiusmod tempor\nincididunt ut labore et dolore magna\naliqua. Ut enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi\n";
	}
};

