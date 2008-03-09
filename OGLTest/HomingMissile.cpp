#include "StdAfx.h"
#include "HomingMissile.h"
#include "Puff.h"

bool HomingMissile::initialised_ = false;
int HomingMissile::outline_dl_ = 0;
int HomingMissile::outline_verts_index_ = 0;

HomingMissile::HomingMissile(HomingJoin* _homing_join, Vector3f _position)
:HomingProjectile(_homing_join)
{
	if(!initialised_)
	{
		 initialise_outline();
		 initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	lifetime_ = 8;
	damage_ = 500;
	turn_rate_ = 10;
	velocity_.y = 200;
	position_ = _position;
}

HomingMissile::~HomingMissile(void)
{

}

void HomingMissile::Hit(std::list<Decoration_ptr>& _spawn)
{
	Puff* p = new Puff();
	p->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(p));
}

void HomingMissile::initialise_outline()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));
	temp_outline->push_back(Vector3f(-2,5,0));
	temp_outline->push_back(Vector3f(2,5,0));

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);
}
