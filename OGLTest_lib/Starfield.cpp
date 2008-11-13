#include "StdAfx.h"
#include "Starfield.h"
#include "Camera.h"
#include "Datastore.h"

bool Starfield::initialised_ = false;

#define SF_PARALAX1 0.7f
#define SF_GRIDSIZE 750.0f


Starfield::Starfield()
{
	if(!initialised_)
	{
		initialised_ = true;
		for(int i = 0; i < SF_MAX_POINTS; i++) 
		{
			stars_[i] = Vector3f(Random::RandomRange(0, SF_GRIDSIZE),
								 Random::RandomRange(0, SF_GRIDSIZE), 0);
		}
	}
}

Starfield::~Starfield(void)
{
	
}

void Starfield::DrawStarfield(Vector3f position_)
{
	int max_index = static_cast<int>(SF_MAX_POINTS / pow(Camera::Instance().GetWidth() / SF_GRIDSIZE,2.0f));
	if(max_index > SF_MAX_POINTS)
		max_index = SF_MAX_POINTS;
	if(max_index < 50)
		max_index = 50;

	glPushMatrix();
	glColor3f(0.5f,0.5f,0.5f);
	glPointSize(1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, stars_);

	glTranslatef(position_.x, position_.y, 0);
	Vector3f offset = Vector3f(-fmodf(SF_PARALAX1 * position_.x, SF_GRIDSIZE),
				      -fmodf(SF_PARALAX1 * position_.y, SF_GRIDSIZE),
					  0);
	glTranslatef(offset.x, offset.y,0);

	int x_minus_times = static_cast<int>(((Camera::Instance().GetWidth() / 2.0f) / SF_GRIDSIZE) + 2);
	int y_minus_times = static_cast<int>(((Camera::Instance().GetHeight() / 2.0f) / SF_GRIDSIZE) + 2);
	float left = -SF_GRIDSIZE * x_minus_times;
	for(int x = 0; x < x_minus_times * 2; x++)
	{
		float top = -SF_GRIDSIZE * y_minus_times;
		for(int y = 0; y < y_minus_times * 2; y++)
		{
			glPushMatrix();
			glTranslatef(left, top, 0);
			glDrawArrays(GL_POINTS, 0, max_index);
			glPopMatrix();
			top += SF_GRIDSIZE;
		}
		left += SF_GRIDSIZE;
	}

	glPointSize(1);
	glPopMatrix();
}
