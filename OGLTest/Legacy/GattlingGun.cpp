#include "StdAfx.h"
#include "GattlingGun.h"
#include "SmallBullet.h"
#include "Bang.h"

GattlingGun::GattlingGun(void)
{	
	outline_verts_.push_back(Vector3f(0, -2.5f, 0));	//0
	outline_verts_.push_back(Vector3f(2.5f, 0, 0));	//1
	outline_verts_.push_back(Vector3f(1, 5, 0));		//2
	outline_verts_.push_back(Vector3f(-1, 5, 0));//3
	outline_verts_.push_back(Vector3f(-2.5f, 0, 0));		//4
	findRadius();

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[1]);
	fill_verts_.push_back(outline_verts_[2]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[2]);
	fill_verts_.push_back(outline_verts_[3]);

	fill_verts_.push_back(outline_verts_[0]);
	fill_verts_.push_back(outline_verts_[3]);
	fill_verts_.push_back(outline_verts_[4]);

	Health = 500;
}

GattlingGun::~GattlingGun(void)
{
}

void GattlingGun::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	static float sumTime = 0;
	sumTime += _timespan;
	if(sumTime>0.2f)
	{
		fireProjectile(new SmallBullet(Vector3f(0,5,0)), _spawn_prj);
		//_spawn_dec.push_back(boost::shared_ptr<Decoration>(new Bang(ltv_position_)));
		sumTime = 0;
	}
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
}