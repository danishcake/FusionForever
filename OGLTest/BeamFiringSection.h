#pragma once
#include "Section.h"
#include "BeamSection.h"

class BeamFiringSection :
	public Section
{
protected:
	float cooldown_;
	float cooldown_time_;
	BeamSection_ptr beam_;
	bool ltv_is_firing_;
public:
	BeamFiringSection(void);
	virtual ~BeamFiringSection(void);

	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
	virtual void DrawSelf();
};
