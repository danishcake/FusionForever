#pragma once
#include "HomingProjectile.h"
#include "LineTrace.h"

class HomingBomb :
	public HomingProjectile
{
protected:
	virtual void InitialiseGraphics();

	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
public:
	HomingBomb(Vector3f _position, BaseEntity* _target);
	virtual ~HomingBomb(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn);
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
};
