#include "StdAfx.h"
#include "HomingProjectile.h"

HomingProjectile::HomingProjectile(void) 
	: Projectile()
{
	homing_join_ = NULL;
}

HomingProjectile::~HomingProjectile(void)
{
	if(homing_join_!=NULL)
		homing_join_->UnregisterProjectile();
	homing_join_ = NULL;
}

void HomingProjectile::Unregister()
{
	homing_join_= NULL;
}

void HomingProjectile::Tick(float _timespan, Matrix4f _transform)
{
	Projectile::Tick(_timespan, _transform);
	if(homing_join_ == NULL)
	{//Acquire target

	}
}