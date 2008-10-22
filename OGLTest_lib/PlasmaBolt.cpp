#include "StdAfx.h"
#include "PlasmaBolt.h"
#include "Puff.h"
#include "Ricochet.h"
#include "Explosion.h"

bool PlasmaBolt::initialised_ = false;
int PlasmaBolt::fill_dl_ = 0;
int PlasmaBolt::fill_verts_index_ = 0;

PlasmaBolt::PlasmaBolt(Vector3f _position)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(255,255,255));
	damage_ = 400.0;
	lifetime_ = 4.0;
	velocity_.y = 600;
	position_ = _position;
	mass_ = 250;
}

PlasmaBolt::~PlasmaBolt(void)
{
}

void PlasmaBolt::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f(-2,-5,0));
	temp_fill->push_back(Vector3f(-3,10,0));
	temp_fill->push_back(Vector3f(3,10,0));

	temp_fill->push_back(Vector3f(3,10,0));
	temp_fill->push_back(Vector3f(2,-5,0));
	temp_fill->push_back(Vector3f(-2,-5,0));


	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PlasmaBolt::Hit(std::vector<Decoration_ptr>& _spawn)
{
	for(int i = 0; i < 25; i++)
	{
		Decoration_ptr ricochet = Decoration_ptr(new Ricochet(angle_ + Random::RandomRange(-15,15), false));
		ricochet->SetPosition(position_);
		_spawn.push_back(ricochet);

		Decoration_ptr puff = Decoration_ptr(new Puff());
		Vector3f puff_pos = this->position_;
		puff_pos.x += Random::RandomRange(-10, 10);
		puff_pos.y += Random::RandomRange(-10, 10);
		puff->SetPosition(puff_pos);
	}
	Decoration_ptr p_explosion = new Explosion();
	p_explosion->SetPosition(position_);
	p_explosion->SetAngle(angle_ + 180.0f);
	_spawn.push_back(p_explosion);
}
