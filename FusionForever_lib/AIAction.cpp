#include "StdAfx.h"
#include "AIAction.h"

AIAction::AIAction()
{
	dx_ = 0;
	dy_ = 0;
	dtheta_ = 0;
	firing_ = false;
	thrust_ = false;
	target_= NULL;
}

AIAction::AIAction(float _dx, float _dy, float _dtheta, bool _firing, bool _thrust, Core* _target)
{
	dx_ = _dx;
	//Clamp the x-axis acceleration
	if(dx_ < -1)
		dx_ = -1;
	if(dx_ > 1)
		dx_ = 1;
	//Clamp the y-axis acceleration
	dy_ = _dy;
	if(dy_ < -1)
		dy_ = -1;
	if(dy_ > 1)
		dy_ = 1;
	//Clamp the angular acceleration
	dtheta_ = _dtheta;
	if(dtheta_< -1)
		dtheta_ = -1;
	if(dtheta_ > 1)
		dtheta_ = 1;	

	firing_ = _firing;
	target_= _target;
	thrust_ = _thrust;
}
