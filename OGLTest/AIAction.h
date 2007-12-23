#pragma once

class AIAction
{
public:
	AIAction();
	AIAction(float _dx, float _dy, float _dtheta, bool _firing);

	float dx_;
	float dy_;
	float dtheta_;
	float firing_;
};