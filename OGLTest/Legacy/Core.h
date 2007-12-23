#pragma once
#include "section.h"

class Core :
	public Section
{
public:
	Core(void);
	virtual ~Core(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform);
};
