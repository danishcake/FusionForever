#include "StdAfx.h"
#include "Projectile.h"

Projectile::Projectile(void)
: BaseEntity()
{
	damage_ = 10;
	lifetime_ = 5;
	mass_ = 100;
	moment_ = 1;
	firer_id_ = -1;

}

Projectile::~Projectile(void)
{
}

void Projectile::Tick(float _timespan, std::vector<Decoration_ptr>& _spawn_dec,  Matrix4f _transform)
{
	lifetime_ -= _timespan;
	BaseEntity::Tick(_timespan, _transform);
}

void Projectile::DrawSelf()
{
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	fill_.DrawFillDisplayList();
	glPopMatrix();
}
