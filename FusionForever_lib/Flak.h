#pragma once
#include "projectile.h"

class Flak :
	public Projectile
{
protected:
	static bool initialised_;
	static int fill_dl_;
	static int fill_verts_index_;
	virtual void InitialiseGraphics();
public:
	Flak(Vector3f _position);
	virtual ~Flak(void);
	virtual void Hit(std::vector<Decoration_ptr>& _spawn);
};
