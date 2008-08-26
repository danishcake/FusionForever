#include "StdAfx.h"
#include "SmallBullet.h"
#include "Puff.h"
#include "Ricochet.h"

bool SmallBullet::initialised_ = false;
int SmallBullet::outline_dl_ = 0;
int SmallBullet::outline_verts_index_ = 0;

SmallBullet::SmallBullet(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
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
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));
	temp_outline->push_back(Vector3f(0,2,0));

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);
}

void SmallBullet::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Decoration_ptr spark = Decoration_ptr(new Ricochet(angle_, false));
	spark->SetPosition(position_);
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(this->position_);

	_spawn.push_back(puff);
	_spawn.push_back(spark);
}
