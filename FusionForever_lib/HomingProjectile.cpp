#include "StdAfx.h"
#include "HomingProjectile.h"
#include "Section.h"
#include "TurningRoutines.h"

HomingProjectile::HomingProjectile(BaseEntity* _target) 
	: Projectile()
{
	target_ = _target;
	turn_rate_ = 10;
	wave_sum_ = Random::RandomFactor() * 2 * M_PI;
	if(target_!=NULL)
		target_->AddSubscriber(this);
}

HomingProjectile::~HomingProjectile(void)
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void HomingProjectile::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	wave_sum_ += _timespan * 8.0f;
	float turn_rate = 0;
	if(target_ != NULL)
	{//Turn to face target
		Vector3f target_position = target_->GetGlobalPosition() -= this->position_;
		TurnData turn_data = GetTurnDirection(this->angle_, target_position);
		turn_rate = turn_data.turn_factor;
	}

	turn_rate += 0.5f * sinf(wave_sum_);

	angle_ += turn_rate * _timespan * turn_rate_;
	velocity_  = Vector3f(scalar_speed_ * sin(angle_ * M_PI / 180.0f), scalar_speed_ * cos(angle_ * M_PI / 180.0f),0);
	Projectile::Tick(_timespan, _spawn_dec, _transform);
}

void HomingProjectile::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}
