#pragma once
#include "section.h"

class GattlingGun :
	public Section
{
public:
	GattlingGun(void);
	virtual ~GattlingGun(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
};
