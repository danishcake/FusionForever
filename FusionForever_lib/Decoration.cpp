#include "StdAfx.h"
#include "Decoration.h"

int Decoration::deco_spawn_count_ = 0;
int Decoration::deco_free_count_ = 0;

Decoration::Decoration(void)
{
	lifetime_ = 1.0f;
	deco_spawn_count_++;
}

Decoration::~Decoration(void)
{
	deco_free_count_++;
}

void Decoration::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	lifetime_-=_timespan;
	BaseEntity::Tick(_timespan, _transform);
}

void Decoration::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	fill_.DrawFillDisplayList();
	glPopMatrix();
} 