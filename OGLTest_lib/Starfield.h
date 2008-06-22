#pragma once

class Starfield
{
private: 
	static int display_list_;
	static bool initialised_;
public:
	Starfield();
	~Starfield(void);
	void DrawStarfield(Vector3f position_);
};
