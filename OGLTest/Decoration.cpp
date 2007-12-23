#include "StdAfx.h"
#include "Decoration.h"

Decoration::Decoration(void)
{
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
	return dec->GetLifetime()<=0;
}

void Decoration::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	Filled::DrawFillDisplayList();
	glPopMatrix();
}