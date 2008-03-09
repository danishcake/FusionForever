#pragma once
#include "projectile.h"

class SmallBullet :
	public Projectile
{
protected:
	virtual void initialise_outline();

	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
public:
	SmallBullet(Vector3f _position);
	virtual ~SmallBullet(void);
	virtual void Hit(std::list<Decoration_ptr>& _spawn);
};
