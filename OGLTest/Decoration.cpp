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


bool Decoration::IsRemovable(boost::shared_ptr<Decoration> dec)
{
	if(dec->GetLifetime()<=0)
		return true;
	return false;
}

void Decoration::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	Filled::DrawFillDisplayList();
	glPopMatrix();
}