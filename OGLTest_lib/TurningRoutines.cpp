#include "stdafx.h"
#include "vmath.h"
#include "TurningRoutines.h"
#include <sstream>

TurnData GetTurnDirection(Vector3f _point_faced, Vector3f _point_to_face)
{
	TurnData d;
	_point_to_face.normalize();
	_point_faced.normalize();
	Vector3f right_vector = Vector3f(_point_faced.y, -_point_faced.x, 0);
	d.turn_factor = right_vector.dotProduct(_point_to_face);
	return d;
}

TurnData GetTurnDirection(float _angle, Vector3f _point_to_face)
{
	TurnData d;
	_point_to_face.normalize();
	Vector3f right_vector = Vector3f(sinf((_angle+90) * M_PI / 180.0f),
									  cosf((_angle+90) * M_PI / 180.0f), 0);
	d.turn_factor = right_vector.dotProduct(_point_to_face);

	Vector3f point_faced = Vector3f(sinf((_angle) * M_PI / 180.0f),
									  cosf((_angle) * M_PI / 180.0f), 0);
	return d;
}

TurnData GetTurnDirection(float _angle, float _angle_to_face)
{
	TurnData d;
	Vector3f _point_to_face = Vector3f(sinf(_angle_to_face * M_PI / 180.0f),
									    cosf(_angle_to_face * M_PI / 180.0f), 0);
	Vector3f right_vector = Vector3f(sinf((_angle+90) * M_PI / 180.0f),
									  cosf((_angle+90) * M_PI / 180.0f), 0);
	d.turn_factor = right_vector.dotProduct(_point_to_face);
	return d;
}

float ClampTurnDirection(float _turn_rate, float _centre_width)
{
	assert(_centre_width >= 0);
	assert(_centre_width <= 1);
	
	if(fabsf(_turn_rate) >= _centre_width)
	{
		return _turn_rate < 0 ? -1.0f : 1.0f;
	} else
	{
		return _turn_rate / _centre_width;
	}
}