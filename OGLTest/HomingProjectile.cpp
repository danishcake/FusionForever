#include "StdAfx.h"
#include "HomingProjectile.h"
#include "Section.h"
#include "TurningRoutines.h"

HomingProjectile::HomingProjectile(HomingJoin* _homing_join) 
	: Projectile()
{
	homing_join_ = _homing_join;
	turn_rate_ = 10;
}

HomingProjectile::~HomingProjectile(void)
{
	if(homing_join_!=NULL)
		homing_join_->UnregisterProjectile();
	homing_join_ = NULL;
}

void HomingProjectile::Unregister()
{
	homing_join_= NULL;
}

void HomingProjectile::Tick(float _timespan, Matrix4f _transform)
{
	if(homing_join_ != NULL)
	{//Turn to face target
		Vector3f target_position = homing_join_->GetSection()->GetGlobalPosition();
		target_position -= this->position_;
		float turn_rate = GetTurnDirection(this->angle_, target_position);
		angle_+= turn_rate * _timespan * turn_rate_;
	}
	Projectile::Tick(_timespan, _transform);
}