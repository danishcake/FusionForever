#include "StdAfx.h"
#include "BeamFiringSection.h"

BeamFiringSection::BeamFiringSection(void)
: Section()
{
	beam_ = NULL;
}

BeamFiringSection::~BeamFiringSection(void)
{
	delete beam_;
}

void BeamFiringSection::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies);
	if(firing_)
	{
		beam_->Tick(_timespan, _spawn_dec, ltv_transform_, _enemies);
	}
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