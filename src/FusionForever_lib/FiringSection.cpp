#include "StdAfx.h"
#include "FiringSection.h"
#include "Core.h"

FiringSection::FiringSection(void)
: Section()
{
	cooldown_ = 0;
	cooldown_time_ = 0.5f;
}

FiringSection::~FiringSection(void)
{
}

void FiringSection::fire_projectile(Projectile_ptr _projectile, std::vector<Projectile_ptr>& _spawn_prj)
{
	_projectile->SetPosition(ltv_transform_ * _projectile->GetPosition());
	_projectile->SetVelocity(ltv_transform_ * _projectile->GetVelocity() - ltv_position_);
	_projectile->SetAngle(atan2f(_projectile->GetVelocity().x, _projectile->GetVelocity().y) * 180 / M_PI);
	
	_spawn_prj.push_back(Projectile_ptr(_projectile));
	_projectile->SetFirer_ID(root_->GetSectionID()); //This assumes that a Core will never be cajoled into firing
}

void FiringSection::RegisterMetadata()
{
	Section::RegisterMetadata();
	SectionMetadata::RegisterSectionTag(section_type_, "Weapon");
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Cooldown", cooldown_time_);
}
