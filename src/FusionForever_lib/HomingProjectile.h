#pragma once
#include "Projectile.h"

class HomingProjectile :
	public Projectile
{
public:
	HomingProjectile(BaseEntity* _target);
	virtual ~HomingProjectile(void);
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
protected:
	BaseEntity* target_;
	float turn_rate_;
	float scalar_speed_;
	float wave_sum_;
	float wave_magnitude_;
	virtual void EndSubscription(Subscriber* _source);
};

