#include "StdAfx.h"
#include "SmallBullet.h"
#include "Puff.h"

bool SmallBullet::initialised_ = false;
int SmallBullet::outline_dl_ = 0;
int SmallBullet::outline_verts_index_ = 0;

SmallBullet::SmallBullet(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		 initialise_outline();
		 initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	damage_ = 25.0;
	lifetime_ = 4.0;
	velocity_.y = 320;
	position_ = _position;
}

SmallBullet::~SmallBullet(void)
{
}

void SmallBullet::initialise_outline()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));
	temp_outline->push_back(Vector3f(0,2,0));

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);
}

void SmallBullet::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(this->position_);
	_spawn.push_back(puff);
}