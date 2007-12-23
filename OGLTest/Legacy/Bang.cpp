#include "StdAfx.h"
#include "Bang.h"
#define BANGLIFETIME 0.3f
#define BANGPARTS 12
#define BANGSIZE 20

Bang::Bang(Vector3f _position)
	: Decoration(_position, Vector3f(0,0,0), M_PI * (float)rand() /(float)RAND_MAX)
{
	LifeTime = BANGLIFETIME;
	bang_color_ = GLColor(255,255*((float)rand()/(float)RAND_MAX),0);
	for(int i = 0; i < BANGPARTS; i++)
		bang_verts_.push_back(Vector3f(cos(((float)i)* M_PI /6),
									   sin(((float)i)* M_PI /6),
									   0));
}

Bang::~Bang(void)
{
}

void Bang::DrawSelf()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(Position.x, Position.y, 0);
	glRotatef(angle_, 0, 0, 1);
	float scale = BANGSIZE *(BANGLIFETIME - LifeTime);
	glScalef(scale,scale,1);
	glColor3ub(bang_color_.r, bang_color_.g, bang_color_.b);
	glBegin(GL_POLYGON);
	for(unsigned int i = 0; i < bang_verts_.size(); i++)
	{
		glVertex3fv(bang_verts_[i]);
	}
	glEnd();

	glPopMatrix();
}
