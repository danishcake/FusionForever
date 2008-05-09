#include "StdAfx.h"
#include "JointSpin.h"
#include "SmallBullet.h"

JointSpin::JointSpin(JointSpinStyle _style)
{
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

	style_ = _style;
	Health = 2000;
}

JointSpin::~JointSpin(void)
{
}

void JointSpin::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	static float spin_time = 0;
	spin_time += _timespan;
	float delta_angle = 0;


	switch(style_)
	{
	case FastCCW:
		Angle += _timespan * FAST_SPIN_SPEED;
		break;
	case FastCW:
		Angle -= _timespan * FAST_SPIN_SPEED;
		break;
	case SlowCCW:
		Angle += _timespan * SLOW_SPIN_SPEED;
		break;
	case SlowCW:
		Angle -= _timespan * SLOW_SPIN_SPEED;
		break;
	case FastReverse:
		delta_angle = sin(spin_time);
		Angle += FAST_SPIN_SPEED * delta_angle * _timespan*3;
		break;
	case SlowReverse:
		delta_angle = sin(spin_time);
		Angle += SLOW_SPIN_SPEED * delta_angle * _timespan*3;
		break;
	}

	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
}
