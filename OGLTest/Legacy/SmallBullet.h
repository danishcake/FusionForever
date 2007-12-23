#pragma once
#include "projectile.h"
#include "vmath.h"
#include "GLColor.h"

class SmallBullet :
	public Projectile
{
protected:
	std::vector<Vector3f> outline_verts_;
	GLColor outline_color_;
public:
	SmallBullet(Vector3f _position);
	virtual ~SmallBullet(void);
	virtual void DrawSelf();
};
