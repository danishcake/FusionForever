#pragma once
#include "projectile.h"

class SmallBullet :
	public Projectile
{
protected:
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	virtual void InitialiseGraphics();
public:
	SmallBullet(Vector3f _position);
	virtual ~SmallBullet(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn);
};
