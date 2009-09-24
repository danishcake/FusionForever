#include "StdAfx.h"
#include "HomingBomb.h"
#include "Puff.h"
#include "SmallBullet.h"

bool HomingBomb::initialised_ = false;
int HomingBomb::fill_dl_ = 0;
int HomingBomb::fill_verts_index_ = 0;

HomingBomb::HomingBomb(Vector3f _position, BaseEntity* _target)
: HomingProjectile(_target)
{
	if(!initialised_)
	{
		 InitialiseGraphics();
		 initialised_ = true;
	}
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);

	lifetime_ = 10;
	damage_ = 200;
	turn_rate_ = 360;
	scalar_speed_ = 50;
	velocity_.y = scalar_speed_;
	position_ = _position;
	mass_ = 300;
}

HomingBomb::~HomingBomb(void)
{

}

void HomingBomb::Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn)
{
	for(int i = 0; i < 10; i++)
	{
		Puff* p = new Puff();
		p->SetPosition(ltv_position_ + Vector3f(Random::RandomRange(-25, 25), Random::RandomRange(-25, 25), 0));
		_spawn.push_back(Decoration_ptr(p));
	}

	for(int i = 0; i < 20; i++)
	{
		SmallBullet* sb = new SmallBullet(ltv_position_ + Vector3f(Random::RandomRange(-25, 25), Random::RandomRange(-25, 25), 0));
		sb->SetAngle(Random::RandomRange(0, 360));
		float sb_speed = sb->GetVelocity().length();
		sb->SetVelocity(Vector3f(sb_speed * sin(sb->GetAngle() * M_PI / 180.0f), sb_speed * cos(sb->GetAngle() * M_PI / 180.0f), 0));
		_projectile_spawn.push_back(sb);
	}
}

void HomingBomb::InitialiseGraphics()
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(-2,  1, 0));
	temp_fill->push_back(Vector3f(-1,  2, 0));

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(-1,  2, 0));
	temp_fill->push_back(Vector3f(1,   2, 0));

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(1,   2, 0));
	temp_fill->push_back(Vector3f(2,   1, 0));

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(2,   1, 0));
	temp_fill->push_back(Vector3f(2,  -1, 0));

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(2,  -1, 0));
	temp_fill->push_back(Vector3f(1,  -1, 0));

	temp_fill->push_back(Vector3f(-2, -1, 0));
	temp_fill->push_back(Vector3f(1,  -1, 0));
	temp_fill->push_back(Vector3f(-1, -1, 0));

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void HomingBomb::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform)
{
	if(lifetime_ > 9)
	{
		scalar_speed_ = 50.0f + (lifetime_ - 9.0f) * 450.0f;
		velocity_  = Vector3f(scalar_speed_ * sin(angle_ * M_PI / 180.0f), scalar_speed_ * cos(angle_ * M_PI / 180.0f),0);
	} else
	{
		scalar_speed_ = 400;

		if(target_ != NULL)
		{
			Vector3f target_position = target_->GetGlobalPosition() - this->position_;
			target_position.normalize();
			velocity_ += target_position * _timespan * 12;
			if(velocity_.length() > 400)
			{
				velocity_.normalize();
				velocity_ *= scalar_speed_;
			}
		}
	}

	Projectile::Tick(_timespan, _spawn_dec, _transform);
}
