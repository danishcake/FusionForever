#pragma once
#include <vector>
#include <string>

struct ScreenText
{
public:
	std::string text;
	float time;
};


struct GameGUI
{
public:
	bool render_bars;
	float ship_health_fraction;
	float core_health_fraction;
	float energy_fraction;
	std::vector<ScreenText> new_messages_;
};

