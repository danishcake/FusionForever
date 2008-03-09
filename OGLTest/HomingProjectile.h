#pragma once
#include "Projectile.h"
#include "HomingJoin.h"

class HomingProjectile :
	public Projectile
{
public:
	HomingProjectile();
	virtual ~HomingProjectile(void);
	virtual void Tick(float _timespan, Matrix4f _transform);
	void UnregisterHomingJoin();
	void RegisterHomingJoin(HomingJoin* _homing_join);
protected:
	HomingJoin* homing_join_;
	float turn_rate_;
	float scalar_speed_;
};

