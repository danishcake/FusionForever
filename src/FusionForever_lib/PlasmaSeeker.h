#pragma once
#include "HomingProjectile.h"

/* Represents a homing projectile that looks much like trail */
class PlasmaSeeker :
	public HomingProjectile
{
protected:
	virtual void InitialiseGraphics();

	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	GLColor trail_color_;
	bool first_run_;
	float scale_;
 
public:
	PlasmaSeeker(Vector3f _position, BaseEntity* _target, GLColor _trail_color, float _scale, float _damage_scale);
	virtual ~PlasmaSeeker(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn);
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
};
