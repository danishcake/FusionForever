#include "StdAfx.h"
#include "JointAngles.h"
#include "SmallBullet.h"

JointAngles::JointAngles(float _first_angle, float _second_angle, float _transition_time, float _pause_time)
{
	first_angle_ = _first_angle;
	second_angle_ = _second_angle;
	transition_time_ = _transition_time;
	pause_time_ = _pause_time;

	Vector3f rotate_point_ = Vector3f(2.5f,2.5f,0);

	outline_verts_.push_back(Vector3f(2.5f,0,0));		//0
	outline_verts_.push_back(Vector3f(0,2.5f,0));		//1
	outline_verts_.push_back(Vector3f(2.5f,5.0f,0));	//2
	outline_verts_.push_back(Vector3f(5.0f,2.5f,0));	//3
	findRadius();

	for(int i = 0; i < outline_verts_.size(); i++)
	{
		outline_verts_[i] -= rotate_point_;
	}

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[1]);
	fill_verts_.push_back(outline_verts_[2]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[2]);
	fill_verts_.push_back(outline_verts_[3]);
	Health = 2000;
}

JointAngles::~JointAngles(void)
{
}

void JointAngles::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	static float tot_time = 0;
	tot_time += _timespan;
	tot_time = fmodf(tot_time, 2 * transition_time_ + 2 * pause_time_);
	if(tot_time < pause_time_)
	{
		Angle = first_angle_;
	}
	else if(tot_time < pause_time_ + transition_time_)
	{
		Angle = first_angle_ + (second_angle_ - first_angle_) * (tot_time-pause_time_)/transition_time_;
	}
	else if(tot_time < 2 * pause_time_ + transition_time_)
	{
		Angle = second_angle_;
	}
	else
	{
		Angle = second_angle_ - (second_angle_ - first_angle_) * (tot_time - 2 * pause_time_ - transition_time_)/transition_time_;
	}

	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
}