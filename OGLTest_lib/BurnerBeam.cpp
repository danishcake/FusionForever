#include "StdAfx.h"
#include "BurnerBeam.h"

BurnerBeam::BurnerBeam(void)
: BeamSection()
{
	this->max_distance_ = 80.0f;
	this->damage_per_second_ = 3000.0f;
}

BurnerBeam::~BurnerBeam(void)
{
}

void BurnerBeam::DrawSelf()
{
	float inner_width = Random::RandomRange(0.5f, 4.0f);
	float outer_width = 5;

	glPushMatrix();
	glLoadMatrixf(ltv_transform_);

	glColor3f(0.7f, 0.1f, 0.2f);

	glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 0);
		glVertex3f(outer_width, outer_width, 0);
		glVertex3f(-outer_width, outer_width, 0);
		
		glVertex3f(outer_width, distance_, 0);
		glVertex3f(0, distance_ + outer_width, 0);
		glVertex3f(-outer_width, distance_, 0);

		glVertex3f(outer_width, outer_width, 0);
		glVertex3f(outer_width, distance_, 0);
		glVertex3f(-outer_width, outer_width, 0);

		glVertex3f(-outer_width, outer_width, 0);
		glVertex3f(outer_width, distance_, 0);
		glVertex3f(-outer_width, distance_, 0);
	glEnd();


	glColor3f(1.0f, 0.75f, 0.75f);

	glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 0);
		glVertex3f(inner_width, inner_width, 0);
		glVertex3f(-inner_width, inner_width, 0);
		
		glVertex3f(inner_width, distance_, 0);
		glVertex3f(0, distance_ + inner_width, 0);
		glVertex3f(-inner_width, distance_, 0);

		glVertex3f(inner_width, inner_width, 0);
		glVertex3f(inner_width, distance_, 0);
		glVertex3f(-inner_width, inner_width, 0);

		glVertex3f(-inner_width, inner_width, 0);
		glVertex3f(inner_width, distance_, 0);
		glVertex3f(-inner_width, distance_, 0);
	glEnd();

	glPopMatrix();
}