#pragma once
#include "projectile.h"

class SmallBullet :
	public Projectile
{
protected:
	virtual void initialise_outline();

	static bool initialised_;
	static int smallbullet_outline_display_list_;
	static int outline_verts_index_;
public:
	SmallBullet(Vector3f _position);
	virtual ~SmallBullet(void);
	virtual void Hit(std::list<boost::shared_ptr<Decoration>> _spawn);
};
