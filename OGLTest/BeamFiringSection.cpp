#include "StdAfx.h"
#include "BeamFiringSection.h"

BeamFiringSection::BeamFiringSection(void)
: Section()
{
	ltv_is_firing_ = false;
}

BeamFiringSection::~BeamFiringSection(void)
{
}

void BeamFiringSection::Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies);
	if(firing_)
	{
		beam_->Tick(_timespan, _spawn_dec, ltv_transform_, _enemies);
	}
	ltv_is_firing_ = firing_;
}

void BeamFiringSection::DrawSelf()
{
	Section::DrawSelf();
	//Now draw beam if firing
	if(firing_)
	{
		beam_->DrawSelf();
	}
}