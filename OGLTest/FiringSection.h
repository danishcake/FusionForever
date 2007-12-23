#pragma once
#include "Section.h"
#include "Projectile.h"

class FiringSection :
	public Section
{
protected:
	float cooldown_;
	float cooldown_time_;

	void fire_projectile(Projectile * _projectile, std::list<boost::shared_ptr<Projectile>>& _spawn_prj);
public:
	FiringSection(void);
	virtual ~FiringSection(void);
};
