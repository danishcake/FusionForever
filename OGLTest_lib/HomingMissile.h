#pragma once
#include "HomingProjectile.h"
#include "LineTrace.h"

class HomingMissile :
	public HomingProjectile
{
protected:
	virtual void initialise_outline();

	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	LineTrace* line_trace_;
	bool first_run_;
  GLColor trail_color_;
public:
	HomingMissile(Vector3f _position, BaseEntity* _target, GLColor _trail_color);
	virtual ~HomingMissile(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn);
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
};
