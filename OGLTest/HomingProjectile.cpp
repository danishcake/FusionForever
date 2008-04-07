#include "StdAfx.h"
#include "HomingProjectile.h"
#include "Section.h"
#include "TurningRoutines.h"
#include "vmath.h"

HomingProjectile::HomingProjectile() 
	: Projectile()
{
	homing_join_ = NULL;
	turn_rate_ = 10;
}

HomingProjectile::~HomingProjectile(void)
{
	if(homing_join_!=NULL)
		homing_join_->UnregisterProjectile();
	homing_join_ = NULL;
}

void HomingProjectile::UnregisterHomingJoin()
{
	homing_join_= NULL;
}

void HomingProjectile::Tick(float _timespan, Matrix4f _transform)
{
	if(homing_join_ != NULL)
	{//Turn to face target
		Vector3f target_position = homing_join_->GetSection()->GetGlobalPosition();
		target_position -= this->position_;
		//I'm not entirely sure why I have to do this negation, 
		//I guess it has something to do with the turning algorithm 
		//being tuned to mouse position coordinate system 
		TurnData turn_data = GetTurnDirection(this->angle_, target_position);
		float turn_rate = turn_data.turn_factor;
		angle_ += turn_rate * _timespan * turn_rate_;
		velocity_  = Vector3f(scalar_speed_ * sin(angle_ * M_PI / 180.0f), scalar_speed_ * cos(angle_ * M_PI / 180.0f),0);
	}
	Projectile::Tick(_timespan, _transform);
}

void HomingProjectile::RegisterHomingJoin(HomingJoin *_homing_join)
{
	homing_join_ = _homing_join;
}