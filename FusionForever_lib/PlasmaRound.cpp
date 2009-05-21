#include "StdAfx.h"
#include "PlasmaRound.h"
#include "Puff.h"
#include "Ricochet.h"

bool PlasmaRound::initialised_ = false;
int PlasmaRound::fill_dl_ = 0;
int PlasmaRound::fill_verts_index_ = 0;

PlasmaRound::PlasmaRound(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255, 255, 255));
	damage_ = 45.0;
	lifetime_ = 4.0;
	velocity_.y = 450;
	position_ = _position;
	mass_ = 25;
}

PlasmaRound::~PlasmaRound(void)
{
}

void PlasmaRound::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f( 1.5f, -1.5f, 0));
	temp_fill->push_back(Vector3f( 1.5f, 4,     0));
	temp_fill->push_back(Vector3f(-1.5f, 4,     0));

	temp_fill->push_back(Vector3f( 1.5f, 4,     0));
	temp_fill->push_back(Vector3f(-1.5f, 4,     0));
	temp_fill->push_back(Vector3f(-1.5f, -1.5f, 0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PlasmaRound::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Decoration_ptr spark = Decoration_ptr(new Ricochet(angle_ , false));
	spark->SetPosition(position_);
	Decoration_ptr spark2 = Decoration_ptr(new Ricochet(angle_ , false));
	spark2->SetPosition(position_);
	
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(position_);

	_spawn.push_back(puff);
	_spawn.push_back(spark);
	_spawn.push_back(spark2);
}
