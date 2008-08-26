#include "StdAfx.h"
#include "SwarmMissile.h"
#include "Puff.h"

bool SwarmMissile::initialised_ = false;
int SwarmMissile::outline_dl_ = 0;
int SwarmMissile::outline_verts_index_ = 0;

SwarmMissile::SwarmMissile(Vector3f _position, BaseEntity* _target)
: HomingProjectile(_target)
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
	lifetime_ = 5;
	damage_ = 70;
	turn_rate_ = 150;
	scalar_speed_ = 200;
	velocity_.y = scalar_speed_;
	position_ = _position;
	mass_ = 25;
}

SwarmMissile::~SwarmMissile(void)
{

}

void SwarmMissile::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Puff* p = new Puff();
	p->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(p));
}

void SwarmMissile::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0,0,0));
	temp_outline->push_back(Vector3f(0,10,0));

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);
}

void SwarmMissile::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	HomingProjectile::Tick(_timespan, _spawn_dec, _transform);
}
