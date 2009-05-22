#pragma once
#include "projectile.h"

class SmallBullet :
	public Projectile
{
protected:
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	virtual void InitialiseGraphics();
public:
	SmallBullet(Vector3f _position);
	virtual ~SmallBullet(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn);
};
