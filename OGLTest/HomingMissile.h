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
public:
	HomingMissile(Vector3f _position);
	virtual ~HomingMissile(void);
	virtual void Hit(std::list<Decoration_ptr>& _spawn);
	virtual void Tick(float _timespan, Matrix4f _transform);
};
