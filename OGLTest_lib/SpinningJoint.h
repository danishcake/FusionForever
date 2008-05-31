#pragma once
#include "Section.h"

class SpinningJoint :
	public Section
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

	float degrees_per_second_;

public:
	SpinningJoint(float _degrees_per_second);
	virtual ~SpinningJoint(void);
	virtual void Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies);
};
