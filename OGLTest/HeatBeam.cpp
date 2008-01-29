#include "StdAfx.h"
#include "HeatBeam.h"

HeatBeam::HeatBeam(void)
: BeamSection()
{
	this->max_distance_ = 300.0f;
	this->damage_per_second_ = 800.0f;
}

HeatBeam::~HeatBeam(void)
{
}

void HeatBeam::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_LINES);

		glVertex3f(0, 0, 0);
		glVertex3f(0, distance_, 0);

	glEnd();

	glPopMatrix();
}