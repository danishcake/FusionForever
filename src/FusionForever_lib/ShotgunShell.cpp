#include "StdAfx.h"
#include "ShotgunShell.h"
#include "Puff.h"

bool ShotgunShell::initialised_ = false;
int ShotgunShell::fill_dl_ = 0;
int ShotgunShell::fill_verts_index_ = 0;

ShotgunShell::ShotgunShell(Vector3f _position, int _generation)
:Projectile()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	fill_.SetFillColor(GLColor(200, 200, 250));
	damage_ = 30;
	lifetime_ = 3.0f / (float)(_generation + 4.0f);
	velocity_.y = 400;
	position_ = _position;
	mass_ = 0;
	generation_ = _generation;
}

ShotgunShell::~ShotgunShell(void)
{
}

void ShotgunShell::InitialiseGraphics()
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

void ShotgunShell::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn)
{
	Decoration_ptr dec = new Puff();
	dec->SetPosition(position_);	

	if(generation_ < 3 && lifetime_ > -100)
	{
		float ss_speed = GetVelocity().length();
		for(int i = 0; i < 4; i++)
		{
			ShotgunShell* ss = new ShotgunShell(position_, generation_+1);
			ss->SetAngle(angle_ + Random::RandomRange(-15, 15));
			ss->SetVelocity(Vector3f(ss_speed * sin(ss->GetAngle() * M_PI / 180.0f), ss_speed * cos(ss->GetAngle() * M_PI / 180.0f), 0));
			_projectile_spawn.push_back(ss);
		}
	}

	_spawn.push_back(dec);
}
