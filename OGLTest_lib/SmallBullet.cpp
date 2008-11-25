#include "StdAfx.h"
#include "SmallBullet.h"
#include "Puff.h"
#include "Ricochet.h"

bool SmallBullet::initialised_ = false;
int SmallBullet::fill_dl_ = 0;
int SmallBullet::fill_verts_index_ = 0;

SmallBullet::SmallBullet(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(210,210,255));
	damage_ = 25.0;
	lifetime_ = 4.0;
	velocity_.y = 320;
	position_ = _position;
	mass_ = 10;
}

SmallBullet::~SmallBullet(void)
{
}

void SmallBullet::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f( 1,-1,0));
	temp_fill->push_back(Vector3f( 1, 3,0));
	temp_fill->push_back(Vector3f(-1, 3,0));

	temp_fill->push_back(Vector3f( 1, 3,0));
	temp_fill->push_back(Vector3f(-1, 3,0));

	temp_fill->push_back(Vector3f(-1,-1,0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void SmallBullet::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Decoration_ptr spark = Decoration_ptr(new Ricochet(angle_, false));
	spark->SetPosition(position_);
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(position_);

	_spawn.push_back(puff);
	_spawn.push_back(spark);
}
