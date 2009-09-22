#include "StdAfx.h"
#include "SwarmMissile.h"
#include "BillboardDeco.h"
#include "Puff.h"

bool SwarmMissile::initialised_ = false;
int SwarmMissile::fill_dl_ = 0;
int SwarmMissile::fill_verts_index_ = 0;

SwarmMissile::SwarmMissile(Vector3f _position, BaseEntity* _target)
: HomingProjectile(_target)
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255,255,0));
	fill_.GetFillColor().a = 127;
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

void SwarmMissile::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn)
{
	Puff* p = new Puff();
	p->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(p));

	BillboardDeco* d = BillboardDeco::RandomExplosion();
	d->SetPosition(ltv_position_);
	_spawn.push_back(Decoration_ptr(d));
}

void SwarmMissile::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f(-1,-2,0));
	temp_fill->push_back(Vector3f(-1, 2,0));
	temp_fill->push_back(Vector3f(0 , 2,0));

	temp_fill->push_back(Vector3f(0 , 2,0));
	temp_fill->push_back(Vector3f(1 ,-2,0));
	temp_fill->push_back(Vector3f(-1,-2,0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void SwarmMissile::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	HomingProjectile::Tick(_timespan, _spawn_dec, _transform);
}
