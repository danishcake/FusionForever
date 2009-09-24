#include "StdAfx.h"
#include "BeamSection.h"
#include "Puff.h"
#include "Sparks.h"
#include <cfloat>

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
	Section_ptr closest_hit_section = NULL;

	float max_dist = 0;						//The maximum distance including the collision
	float low_dist = max_distance_;			//The minimum distance excluding any collisions

	std::vector<Section_ptr> filtered_sections; 

	BOOST_FOREACH(Core_ptr core, _enemies)
	{
		core->RayCollisionFilter(ltv_position_, ltv_transform_ * Vector3f(0, max_distance_, 0), filtered_sections, low_dist, max_dist);
	}

	Vector3f low_dist_v = ltv_transform_ * Vector3f(0,low_dist,0);
	Vector3f high_dist_v = ltv_transform_ * Vector3f(0,max_dist,0);

	float lengthSq = FLT_MAX;
	Vector3f best_collision_point;
	// filtered_sections contains all the sections that are near the beam
	BOOST_FOREACH(Section_ptr section, filtered_sections)
	{
		Vector3f collision_point;
		if(section->CheckCollisions(low_dist_v, high_dist_v, collision_point))
		{
			float found_lengthSq = (collision_point - ltv_transform_ * Vector3f() ).lengthSq();
			if(!closest_hit_section || found_lengthSq < lengthSq)
			{
				lengthSq = found_lengthSq;
				best_collision_point = collision_point;
				closest_hit_section = section;
			}
		}
	}

	if(closest_hit_section)
	{
		closest_hit_section->TakeDamage(_timespan * damage_per_second_, firer_id_);
		if(deco_cooldown_<=0)
		{
			deco_cooldown_ = 0.025f;

			Decoration_ptr spark = Decoration_ptr(new Sparks());
			spark->SetPosition(best_collision_point);
			spark->SetVelocity(closest_hit_section->GetRoot()->GetVelocity());
			spark->SetAngle(GetGlobalAngle());
			_spawn_dec.push_back(spark);
		}
			
		distance_ = (best_collision_point -ltv_transform_ * Vector3f()).length();
	}
	else
	{
		distance_ = max_distance_;
	}
}
