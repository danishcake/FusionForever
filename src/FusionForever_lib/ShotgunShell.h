#pragma once
#include "projectile.h"

class ShotgunShell :
	public Projectile
{
protected:
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	virtual void InitialiseGraphics();
	int generation_;
public:
	ShotgunShell(Vector3f _position, int _generation);
	virtual ~ShotgunShell(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn, std::vector<Projectile_ptr>& _projectile_spawn);
};
