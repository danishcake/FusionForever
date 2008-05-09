#include "StdAfx.h"
#include "Decoration.h"

Decoration::Decoration(void)
{
	lifetime_ = 1.0f;
}

Decoration::~Decoration(void)
{
}

void Decoration::Tick(float _timespan, Matrix4f _transform)
{
	lifetime_-=_timespan;
	BaseEntity::Tick(_timespan, _transform);
}



void Decoration::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	Filled::DrawFillDisplayList();
	glPopMatrix();
} 