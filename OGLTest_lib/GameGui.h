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
	GameGUI()
	{
		ship_health_fraction = 0;
		render_bars = false;
		core_health_fraction = 0;
		energy_fraction = 0;
		counter_A_max = 0;
		counter_A_value = -1;
		counter_A_visible = false;
		counter_B_max = 0;
		counter_B_value = -1;
		counter_B_visible = false;
		counter_C_max = 0;
		counter_C_value = -1;
		counter_C_visible = false;
	}
	bool render_bars;
	float ship_health_fraction;
	float core_health_fraction;
	float energy_fraction;
	std::vector<ScreenText> new_messages_;

	bool counter_A_visible;
	int counter_A_value;
	int counter_A_max; //If max == -1 then do not render

	bool counter_B_visible;
	int counter_B_value;
	int counter_B_max; //If max == -1 then do not render

	bool counter_C_visible;
	int counter_C_value;
	int counter_C_max; //If max == -1 then do not render

};

