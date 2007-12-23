#pragma once
#include "section.h"

class RigidArm :
	public Section
{
public:
	RigidArm(void);
	virtual ~RigidArm(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
};
