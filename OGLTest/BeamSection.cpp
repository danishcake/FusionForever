#include "StdAfx.h"
#include "BeamSection.h"
#include <boost/foreach.hpp>
#include "Puff.h"
#include "Sparks.h"

static const int beam_accuracy = 5;
static const float linear_search_steps = 3.0f;
static const int binary_search_depth = 10; //Provides sufficient accuracy for a range of up to 5000 (Typically I use 300)

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
	bool quick_hit_check = false;

	bool has_hit = false;
	float max_dist = max_distance_;	//The maximum distance including the collision
	float max_dist_bs = max_distance_;	//The maxiumum distance being considered by the bs (may not include collision)
	float low_dist = 0.0f;			//The minimum distance excluding any collisions

	//Do a binary search to narrow down the range
	for(int i = 0; i < 10; i++)
	{
		bool bs_hit = false; 
		BOOST_FOREACH(Core_ptr core, _enemies)
		{
			bs_hit |= core->QuickRayCheck(ltv_position_, ltv_transform_ * Vector3f(0, max_dist_bs, 0));
			if(bs_hit)
				break;
		}
		if(i==0 && !bs_hit)
			break;
		if(bs_hit)
		{
			quick_hit_check = true;
			max_dist = max_dist_bs;
			max_dist_bs -= max_distance_ / pow(2.0f, i+1);
		}
		else
		{
			low_dist = max_dist_bs;
			max_dist_bs += max_distance_ / pow(2.0f, i+1);
		}
	}

	if(quick_hit_check)
	{
		BOOST_FOREACH(Core_ptr core, _enemies)
		{
			Section* hit_section;
			for(float dist = low_dist; dist <= max_dist; dist += (max_dist - low_dist) / linear_search_steps)
			{
				if(core->CheckCollisions(ltv_transform_ * Vector3f(0,dist,0), hit_section))
				{
					closest_hit_section = hit_section;
					max_dist = dist;
					has_hit = true;
					break;
				}
			}
		}
	}
	if(!has_hit) //Earlier estimates of max_dist were too low, as bounding circle too loose. Do complete check (Expensive)
	{
		max_dist = max_distance_;
		BOOST_FOREACH(Core_ptr core, _enemies)
		{
			Section* hit_section;
			for(float dist = low_dist; dist <= max_dist; dist += beam_accuracy)
			{
				if(core->CheckCollisions(ltv_transform_ * Vector3f(0,dist,0), hit_section))
				{
					closest_hit_section = hit_section;
					max_dist = dist;
					has_hit = true;
					break;
				}
			}
		}
	}

	if(has_hit)
	{
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