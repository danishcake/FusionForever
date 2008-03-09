#pragma once
#include "Projectile.h"
#include "HomingJoin.h"

class HomingProjectile :
	public Projectile
{
public:
	HomingProjectile(void);
	virtual ~HomingProjectile(void);
	virtual void Tick(float _timespan, Matrix4f _transform);
	void Unregister();
protected:
	HomingJoin* homing_join_;
};

