#pragma once
#include "FiringSection.h"
#include "SmallBullet.h"

class Blaster :
	public FiringSection
{
protected:
	virtual void initialise_outline();
	virtual void initialise_fill();
	
	static bool initialised_;
	static int outline_dl_;
	static int outline_verts_index_;
	static int fill_dl_;
	static int fill_verts_index_;

public:
	Blaster(void);
	virtual ~Blaster(void);

	virtual void Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies);
};
