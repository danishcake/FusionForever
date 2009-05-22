#include "StdAfx.h"
#include "Flak.h"
#include "Puff.h"

bool Flak::initialised_ = false;
int Flak::fill_dl_ = 0;
int Flak::fill_verts_index_ = 0;

Flak::Flak(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255,127,0));
	damage_ = 15;
	lifetime_ = 2.0;
	velocity_.y = 320;
	position_ = _position;
	mass_ = 10;
	angle_ = Random::RandomRange(0, 6.28);
}

Flak::~Flak(void)
{
}

void Flak::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());


	temp_fill->push_back(Vector3f( -1, 0, 0));
	temp_fill->push_back(Vector3f( 0, 2, 0));
	temp_fill->push_back(Vector3f( 1, 0, 0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void Flak::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn)
{
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(position_);

	_spawn.push_back(puff);
}
