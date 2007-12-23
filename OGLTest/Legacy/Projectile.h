#pragma once
#include "vmath.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include "Decoration.h"

class Projectile
{
public:
	Projectile(Vector3f _position, Vector3f _velocity);
	virtual ~Projectile(void);
	virtual void Tick(float _timespan, std::list<boost::shared_ptr<Decoration>>& _spawn_dec);
	virtual void Hit(std::list<boost::shared_ptr<Decoration>>& _spawn_dec);
	virtual void DrawSelf() = 0;
	float LifeTime;
	float Radius;
	int Damage;
	Vector3f Position;
	void SetVelocity(Vector3f _velocity);
	Vector3f Velocity;
protected:
	float angle_;
};
