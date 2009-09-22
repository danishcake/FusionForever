#pragma once
#include "Section.h"
#include "Projectile.h"

class FiringSection :
	public Section
{
protected:
	float cooldown_;
	float cooldown_time_;

	void fire_projectile(Projectile_ptr _projectile, std::vector<Projectile_ptr>& _spawn_prj);
public:
	FiringSection(void);
	virtual ~FiringSection(void);

	/*
	 * Register section metadata - section name associated with a number of 
	 * tags, values and coordinates
	 */
	virtual void RegisterMetadata();
};
