#pragma once

#define SF_MAX_POINTS 1000


class Starfield
{
private: 
	static bool initialised_;

	Vector3f stars_[SF_MAX_POINTS];
public:
	Starfield();
	~Starfield(void);
	void DrawStarfield(Vector3f position_);
};
