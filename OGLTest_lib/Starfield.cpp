#include "StdAfx.h"
#include "Starfield.h"
#include "Camera.h"
#include "Datastore.h"

bool Starfield::initialised_ = false;
int Starfield::display_list_ = 0;
#define SF_PARALAX1 0.7f
#define SF_GRIDSPACING 35.0f
#define SF_GRIDSIZE 10

Starfield::Starfield()
{
	if(!initialised_)
	{
		initialised_ = true;
		display_list_ = Datastore::Instance().BeginDisplayList();
		glColor3f(1,1,1);
		glPointSize(2);
		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);
		/*
		for(int x = -SF_GRIDSIZE; x < SF_GRIDSIZE; x++)
		{
			for(int y = -SF_GRIDSIZE; y < SF_GRIDSIZE; y++)
			{
				glVertex3f(x * SF_GRIDSPACING, y * SF_GRIDSPACING, 0);
			}
		}*/
		for(int i = 0; i< 200; i++)
		{
			glVertex3f(SF_GRIDSPACING * SF_GRIDSIZE * Random::RandomFactorPM(),
				       SF_GRIDSPACING * SF_GRIDSIZE * Random::RandomFactorPM(),
					   0);
		}
		glEnd();
		Datastore::Instance().EndDisplayList();
		glPointSize(1);
	}
}

Starfield::~Starfield(void)
{
	
}

void Starfield::DrawStarfield(Vector3f position_)
{
	glPushMatrix();

	glTranslatef(position_.x, position_.y, 0);
	Vector3f offset = Vector3f(-fmodf(SF_PARALAX1 * position_.x, SF_GRIDSIZE * SF_GRIDSPACING*2),
				      -fmodf(SF_PARALAX1 * position_.y, SF_GRIDSIZE * SF_GRIDSPACING*2),
					  0);
	glTranslatef(offset.x, offset.y,0);
	
	glCallList(display_list_);

	glPushMatrix();
	glTranslatef(2 * SF_GRIDSIZE * SF_GRIDSPACING,0,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2 * SF_GRIDSIZE * SF_GRIDSPACING,0,0);
	glCallList(display_list_);
	glPopMatrix();

	
	glPushMatrix();
	glTranslatef(2 * SF_GRIDSIZE * SF_GRIDSPACING,2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2 * SF_GRIDSIZE * SF_GRIDSPACING,2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2 * SF_GRIDSIZE * SF_GRIDSPACING, -2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2 * SF_GRIDSIZE * SF_GRIDSPACING, -2 * SF_GRIDSIZE * SF_GRIDSPACING,0);
	glCallList(display_list_);
	glPopMatrix();



	glPopMatrix();
}
