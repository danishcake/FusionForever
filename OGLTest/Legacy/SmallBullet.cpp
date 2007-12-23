#include "StdAfx.h"
#include "SmallBullet.h"

SmallBullet::SmallBullet(Vector3f _position)
: Projectile(_position, Vector3f(0,100,0))
{
	outline_verts_.push_back(Vector3f(0,0,0));
	outline_verts_.push_back(Vector3f(0,4,0));

	outline_color_ = GLColor(255, 255, 255);
	LifeTime = 3.0;
}

SmallBullet::~SmallBullet(void)
{
}

void SmallBullet::DrawSelf()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(Position.x, Position.y, 0);
	glRotatef(angle_, 0, 0,1);

	glColor3ub(outline_color_.r, outline_color_.g, outline_color_.b);
	glBegin(GL_LINES);
	for(unsigned int i = 0; i < outline_verts_.size(); i++)
	{
		glVertex3fv(outline_verts_[i]);
	}
	glEnd();

	glPopMatrix();
}