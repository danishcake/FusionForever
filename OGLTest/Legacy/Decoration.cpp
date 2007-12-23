#include "StdAfx.h"
#include "Decoration.h"

Decoration::Decoration(Vector3f _position, Vector3f _velocity, float _angle)
{
	Position = _position;
	Velocity  = _velocity;
	angle_ = _angle;
	LifeTime = 5;
}

Decoration::~Decoration(void)
{
	
}

void Decoration::Tick(float _timespan)
{
	Position += Velocity * _timespan;
	LifeTime -= _timespan;
}
