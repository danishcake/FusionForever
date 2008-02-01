#include "StdAfx.h"
#include "BeamSection.h"
#include <boost/foreach.hpp>
#include "Puff.h"
#include "Sparks.h"

static const float beam_accuracy = 5.0f;
static const float beam_high_accuracy = 1.0f;

BeamSection::BeamSection(void)
{
	max_distance_ = 100.0f;
	distance_ = 0.0f;
}

BeamSection::~BeamSection(void)
{
}

void BeamSection::Tick(float _timespan, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	BaseEntity::Tick(_timespan, _transform);

	deco_cooldown_ -= _timespan;
	Section* closest_hit_section;
	bool has_hit = false;
	float max_dist = max_distance_;

	BOOST_FOREACH(Core_ptr core, _enemies)
	{
		Section* hit_section;
		for(float dist = 0; dist <= max_dist; dist += beam_accuracy)
		{
			if(core->CheckCollisions(ltv_transform_ * Vector3f(0,dist,0), hit_section))
			{
				closest_hit_section = hit_section;
				max_dist = dist - beam_accuracy;
				has_hit = true;
				break;
			}
		}
	}
	if(has_hit)
	{
		Section * throwaway;
		for(float dist = max_dist + beam_accuracy;  dist >= max_dist; dist-=beam_high_accuracy)
		{
			if(!closest_hit_section->CheckCollisions(ltv_transform_ * Vector3f(0,dist,0), throwaway))
			{
				max_dist = dist-beam_accuracy;
				break;
			}
		}

		closest_hit_section->TakeDamage(_timespan * damage_per_second_);
		if(deco_cooldown_<=0)
		{
			deco_cooldown_ = 0.025f;

			Decoration_ptr spark =Decoration_ptr(new Sparks());
			spark->SetPosition(ltv_transform_ * Vector3f(0,max_dist+beam_accuracy,0));
			spark->SetAngle(GetGlobalAngle());
			_spawn_dec.push_back(spark);
		}
			
		distance_ = max_dist+5.0f;
	}
	else
	{
		distance_ = max_distance_;
	}
}