#pragma once
#include "Section.h"

class SpinningJoint :
	public Section
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int spinningjoint_outline_display_list_;
	static int outline_verts_index_;
	static int spinningjoint_fill_display_list_;
	static int fill_verts_index_;

public:
	SpinningJoint(void);
	virtual ~SpinningJoint(void);
	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform);
};
