#include "StdAfx.h"
#include "Core.h"
#include "SmallBullet.h"

Core::Core(void)
: Section()
{
	Vector3f rotate_point_ = Vector3f(10,10,0);

	outline_verts_.push_back(Vector3f(0,5,0));		//0
	outline_verts_.push_back(Vector3f(5,0,0));		//1
	outline_verts_.push_back(Vector3f(15,0,0));		//2
	outline_verts_.push_back(Vector3f(20,5,0));		//3
	outline_verts_.push_back(Vector3f(20,15,0));	//4
	outline_verts_.push_back(Vector3f(15,20,0));	//5
	outline_verts_.push_back(Vector3f(5,20,0));		//6
	outline_verts_.push_back(Vector3f(0,15,0));		//7
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

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[3]);
	fill_verts_.push_back(outline_verts_[4]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[4]);
	fill_verts_.push_back(outline_verts_[5]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[5]);
	fill_verts_.push_back(outline_verts_[6]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[6]);
	fill_verts_.push_back(outline_verts_[7]);
	Health = 2000;
}

Core::~Core(void)
{
}

void Core::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	Angle += _timespan * 90.0f; //90'/s
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
	//this->fireProjectile(new SmallBullet(
}