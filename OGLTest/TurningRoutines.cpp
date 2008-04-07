#include "stdafx.h"
#include "vmath.h"
#include "TurningRoutines.h"


TurnData GetTurnDirection(Vector3f _point_faced, Vector3f _point_to_face)
{
	TurnData d;
	_point_to_face.normalize();
	_point_faced.normalize();
	Vector3f right_vector = Vector3f(_point_faced.y, -_point_faced.x, 0);
	d.turn_factor = right_vector.dotProduct(_point_to_face);
	float adotb = _point_to_face.dotProduct(_point_faced);
	if(adotb != 0)
	{
		d.angle_difference = asinf(adotb);
	} else
	{
		Vector3f acrossb = _point_to_face.crossProduct(_point_faced);
		
		d.angle_difference = 180.0f * acosf(acrossb.length()) / M_PI;
	}
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
	float adotb = _point_to_face.dotProduct(point_faced);
	if(adotb != 0)
	{
		d.angle_difference = 180.0f * asinf(adotb) / M_PI;
	} else
	{
		Vector3f acrossb = _point_to_face.crossProduct(point_faced);
		d.angle_difference = 180.0f * acosf(acrossb.length()) / M_PI;
	}
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
	d.angle_difference = _angle_to_face - _angle;
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