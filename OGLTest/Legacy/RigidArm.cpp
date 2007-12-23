#include "StdAfx.h"
#include "RigidArm.h"


RigidArm::RigidArm(void) 
	: Section()
{
	Vector3f rotate_point_ = Vector3f(2.5f,2.5f,0);

	outline_verts_.push_back(Vector3f(2.5f,0,0));	//0
	outline_verts_.push_back(Vector3f(0,2.5f,0));	//1
	outline_verts_.push_back(Vector3f(0,10,0));		//2
	outline_verts_.push_back(Vector3f(2.5f,7.5f,0));//3
	outline_verts_.push_back(Vector3f(5,10,0));		//4
	outline_verts_.push_back(Vector3f(5,2.5,0));	//5
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
	Health = 800;
	
}

RigidArm::~RigidArm(void)
{

}

void RigidArm::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
}