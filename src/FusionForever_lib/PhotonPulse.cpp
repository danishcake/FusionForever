#include "StdAfx.h"
#include "PhotonPulse.h"
#include "Ricochet.h"

bool PhotonPulse::initialised_ = false;
int PhotonPulse::fill_dl_ = 0;
int PhotonPulse::fill_verts_index_ = 0;

PhotonPulse::PhotonPulse(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(150, 159, 255));
	damage_ = 80;
	lifetime_ = 3.0;
	velocity_.y = 750;
	position_ = _position;
	mass_ = 0;
}

PhotonPulse::~PhotonPulse(void)
{
}

void PhotonPulse::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	const float length = 4.0f;
	const float width = 1.0f;
	temp_fill->push_back(Vector3f(-width,  length, 0));
	temp_fill->push_back(Vector3f( width,  length, 0));
	temp_fill->push_back(Vector3f( -width, -length, 0));

	temp_fill->push_back(Vector3f( width,  length, 0));
	temp_fill->push_back(Vector3f( width, -length, 0));
	temp_fill->push_back(Vector3f(-width, -length, 0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PhotonPulse::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& /*_projectile_spawn*/)
{
	Decoration_ptr dec = new Ricochet( angle_ + Random::RandomRange(-15, 15), false);
	dec->SetPosition(position_);

	_spawn.push_back(dec);
}
