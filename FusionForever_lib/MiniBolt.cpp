#include "StdAfx.h"
#include "MiniBolt.h"
#include "Puff.h"

bool MiniBolt::initialised_ = false;
int MiniBolt::fill_dl_ = 0;
int MiniBolt::fill_verts_index_ = 0;

MiniBolt::MiniBolt(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255, 0, 0));
	damage_ = 75;
	lifetime_ = 6.0;
	velocity_.y = 320;
	position_ = _position;
	mass_ = 25;
}

MiniBolt::~MiniBolt(void)
{
}

void MiniBolt::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());


	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( 0, 2, 0));
	temp_fill->push_back(Vector3f( 1.414, 1.414, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( 1.414, 1.414, 0));
	temp_fill->push_back(Vector3f( 2, 0, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( 2, 0, 0));
	temp_fill->push_back(Vector3f( 1.414, -1.414, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( 1.414, -1.414, 0));
	temp_fill->push_back(Vector3f( 0, 2, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( 0, 2, 0));
	temp_fill->push_back(Vector3f( -1.414, -1.414, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( -1.414, -1.414, 0));
	temp_fill->push_back(Vector3f( -2, 0, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( -2, 0, 0));
	temp_fill->push_back(Vector3f( -1.414, 1.414, 0));

	temp_fill->push_back(Vector3f( 0, 0, 0));
	temp_fill->push_back(Vector3f( -1.414, 1.414, 0));
	temp_fill->push_back(Vector3f( 0, 2, 0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void MiniBolt::Hit(std::vector<Decoration_ptr>& _spawn)
{
	Decoration_ptr puff = Decoration_ptr(new Puff());
	puff->SetPosition(position_);

	_spawn.push_back(puff);
}
