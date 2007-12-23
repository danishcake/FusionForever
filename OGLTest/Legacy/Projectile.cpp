#include "StdAfx.h"
#include "Projectile.h"
#include "Bang.h"

Projectile::Projectile(Vector3f _position, Vector3f _velocity)
{
	Position = _position;
	Velocity = _velocity;
	Radius = 2.0;
	LifeTime = 5.0;
	Damage = 100;
}

Projectile::~Projectile(void)
{
}

void Projectile::Tick(float _timespan, std::list<boost::shared_ptr<Decoration>>& _spawn_prj)
{
	Position += Velocity * _timespan;
	LifeTime -= _timespan;
}

void Projectile::DrawSelf()
{
	glLoadIdentity();
	glTranslatef(Position.x, Position.y, 0.0f);
}
void Projectile::SetVelocity(Vector3f _velocity)
{
	Velocity = _velocity;
	angle_ = atan2f(Velocity.x, -Velocity.y) * 180 / M_PI;
}

void Projectile::Hit(std::list<boost::shared_ptr<Decoration>>& _spawn_dec)
{
	_spawn_dec.push_back(boost::shared_ptr<Decoration>(new Bang(Position)));
}