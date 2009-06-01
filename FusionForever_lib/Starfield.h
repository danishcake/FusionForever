#pragma once
#include "vmath.h"

static const int SF_MAX_POINTS = 1000;

class Starfield
{
private: 
	Vector3f stars_[SF_MAX_POINTS];
public:
	Starfield();
	~Starfield(void);
	void DrawStarfield(Vector3f _position);
};
