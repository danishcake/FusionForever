#pragma once
#include "HomingProjectile.h"

class SwarmMissile :
	public HomingProjectile
{
protected:
	virtual void initialise_outline();

	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;

public:
	SwarmMissile(Vector3f _position, BaseEntity* _target);
	virtual ~SwarmMissile(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn);
	virtual void Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
};
