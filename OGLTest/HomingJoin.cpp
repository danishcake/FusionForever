#include "stdafx.h"
#include "HomingJoin.h"

#include "Section.h"
#include "HomingProjectile.h"

HomingJoin::HomingJoin(HomingProjectile* _a, Section* _b)
{
	a_ = _a;
	b_ = _b;
}

HomingJoin::~HomingJoin()
{
}

void HomingJoin::UnregisterProjectile()
{
	//This should be called by the destructor of A. It unregisters B and then delete the join.
	b_->Unregister();
	delete this;
}
void HomingJoin::UnregisterSection()
{
	//This should be called by the destructor of B. It unregisters A and then delete the join.
	a_->Unregister();
	delete this;
}

HomingProjectile* HomingJoin::GetProjectile()
{
	return a_;
}

Section* HomingJoin::GetSection()
{
	return b_;
}