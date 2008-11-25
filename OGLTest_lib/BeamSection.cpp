#include "StdAfx.h"
#include "BeamSection.h"
#include "Puff.h"
#include "Sparks.h"

static const float beam_accuracy = 1.0f;

BeamSection::BeamSection(void)
{
	max_distance_ = 100.0f;
	distance_ = 0.0f;
	firer_id_ = -1;
}

BeamSection::~BeamSection(void)
{
}

void BeamSection::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies)
{
	BaseEntity::Tick(_timespan, _transform);

	deco_cooldown_ -= _timespan;
	Section_ptr closest_hit_section;
	bool quick_hit_check = false;

	bool has_hit = false;
	float max_dist = 0;	//The maximum distance including the collision
	float low_dist = max_distance_;			//The minimum distance excluding any collisions

	std::vector<Section_ptr> filtered_sections; 

	BOOST_FOREACH(Core_ptr core, _enemies)
	{
		core->RayCollisionFilter(ltv_position_, ltv_transform_ * Vector3f(0, max_distance_, 0), filtered_sections, low_dist, max_dist);
	}

	Vector3f low_dist_v = ltv_transform_ * Vector3f(0,low_dist,0);
	Vector3f delta_dist_v = ltv_transform_ * Vector3f(0, beam_accuracy,0) - ltv_transform_ * Vector3f();
	Vector3f test_dist_v = low_dist_v;
	BOOST_FOREACH(Section_ptr section, filtered_sections)
	{
		Section_ptr hit_section;
		test_dist_v = low_dist_v;
		for(float dist = low_dist; dist <= max_dist; dist += beam_accuracy)
		{
			if(section->CheckCollisions(test_dist_v, hit_section))
			{
				closest_hit_section = hit_section;
				max_dist = dist;
				has_hit = true;
				break;
			}
			test_dist_v += delta_dist_v;
		}
	}

	if(has_hit)
	{
		closest_hit_section->TakeDamage(_timespan * damage_per_second_);
		if(deco_cooldown_<=0)
		{
			deco_cooldown_ = 0.025f;

			Decoration_ptr spark = Decoration_ptr(new Sparks());
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
