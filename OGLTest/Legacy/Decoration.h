#pragma once
#include "vmath.h"
#include <vector>

class Decoration
{
public:
	Decoration(Vector3f _position, Vector3f _velocity, float _angle);
	virtual ~Decoration(void);
	
	virtual void Tick(float _timespan);
	virtual void DrawSelf() = 0;
	float LifeTime;
	Vector3f Position;
	Vector3f Velocity;
protected:
	float angle_;
};