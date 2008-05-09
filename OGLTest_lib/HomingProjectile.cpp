#include "StdAfx.h"
#include "HomingProjectile.h"
#include "Section.h"
#include "TurningRoutines.h"
#include "vmath.h"

HomingProjectile::HomingProjectile(BaseEntity* _target) 
	: Projectile()
{
	target_ = _target;
	turn_rate_ = 10;
	if(target_!=NULL)
		target_->AddSubscriber(this);
}

HomingProjectile::~HomingProjectile(void)
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void HomingProjectile::Tick(float _timespan, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	if(target_ != NULL)
	{//Turn to face target
		Vector3f target_position = target_->GetGlobalPosition();
		target_position -= this->position_;
		//I'm not entirely sure why I have to do this negation, 
		//I guess it has something to do with the turning algorithm 
		//being tuned to mouse position coordinate system 
		TurnData turn_data = GetTurnDirection(this->angle_, target_position);
		float turn_rate = turn_data.turn_factor;
		angle_ += turn_rate * _timespan * turn_rate_;
		velocity_  = Vector3f(scalar_speed_ * sin(angle_ * M_PI / 180.0f), scalar_speed_ * cos(angle_ * M_PI / 180.0f),0);
	}
	Projectile::Tick(_timespan, _spawn_dec, _transform);
}

void HomingProjectile::EndSubscription(BaseEntity* _source)
{
	if(target_ == _source)
		target_ = NULL;
//	BaseEntity::EndSubscription(_source); 
}
