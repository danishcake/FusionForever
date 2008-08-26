#include "StdAfx.h"
#include "HeatBeam.h"

HeatBeam::HeatBeam(void)
: BeamSection()
{
	this->max_distance_ = 300.0f;
	this->damage_per_second_ = 1600.0f;
}

HeatBeam::~HeatBeam(void)
{
}

void HeatBeam::DrawSelf()
{
	float inner_width = Random::RandomRange(0.5f, 2.5f);
	float outer_width = 3;

	glPushMatrix();
	glLoadMatrixf(ltv_transform_);

	glColor3f(1.0f, 0.0f, 0.0f);

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


	glColor3f(1.0f, 1.0f, 1.0f);

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