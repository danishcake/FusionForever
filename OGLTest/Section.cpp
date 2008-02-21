#include "StdAfx.h"
#include "Section.h"
#include <boost/foreach.hpp>
#include "Puff.h"

Section::Section(void) 
: BaseEntity(), Outlined(), Filled()
{
	health_ = 800;
	max_health_ = 800;
	damage_timer_ = 0;
	firing_ = false;
	outline_color_base_ = GLColor(255, 255, 255);
	default_sub_section_position_ = Vector3f(0,0,0);
}

Section::~Section(void)
{
}

void Section::DrawSelf(void)
{
	//Draw self
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	Filled::DrawFillDisplayList();
	Outlined::DrawOutlinedDisplayList();
	//Draw children
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->DrawSelf();
	}

	glPopMatrix();
}
void Section::AddChild(Section *child)
{
	if(!child->GetPositionSpecified())
	{
		child->SetPosition(default_sub_section_position_);
	}
	this->sub_sections_.push_back(Section_ptr(child));
	child->SetColor(fill_color_);
}

void Section::findRadius(void)
{
	for(std::vector<Vector3f>::iterator it = outline_verts_->begin(); it!=outline_verts_->end(); it++)
	{
		if(it->length() > radius_)
			radius_ = it->length();
	}
}

void Section::GetDeathSpawn(std::list<Decoration_ptr>& _spawn_dec)
{
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->GetDeathSpawn(_spawn_dec);
	}
	BOOST_FOREACH(Vector3f vert, *outline_verts_)
	{
		Puff* puff = new Puff();
		puff->SetPosition(ltv_transform_ * vert);
		_spawn_dec.push_back(Decoration_ptr(puff));
	}
}

void Section::Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	BaseEntity::Tick(_timespan, _transform); // Use ltv_transform after this as _transform is passed by value
	
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		if(section->GetHealth() <=0)
		{
			section->GetDeathSpawn(_spawn_dec);
		}
		else
		{
			section->SetFiring(firing_);
			section->Tick(_timespan, _spawn_prj, _spawn_dec, ltv_transform_, _enemies);
		}
	}

	sub_sections_.erase(std::remove_if(sub_sections_.begin(), sub_sections_.end(), 
		                Section::IsRemovable), sub_sections_.end());
	damage_timer_ -= _timespan;

	if(fmodf(damage_timer_,health_/max_health_) > health_/max_health_ / 2.0f)
		outline_color_ = outline_color_base_.GetFaded(0);
	else
		outline_color_ = outline_color_base_;
}
bool Section::CheckCollisions(Projectile_ptr _projectile)
{
	bool hasCollided = false;
	if(Collisions2f::CirclesIntersect(_projectile->GetPosition(), _projectile->GetRadius(), ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		for(int vert = 0; vert < fill_verts_->size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(ltv_transform_ * (*fill_verts_)[vert], ltv_transform_ * (*fill_verts_)[vert+1], ltv_transform_ * (*fill_verts_)[vert+2], _projectile->GetPosition()))
			{
				hasCollided = true; //This is where we decided that the collision has taken place				
				TakeDamage(_projectile->GetDamage());
				_projectile->SetLifetime(0);
				break;
			}
		}
	}
	if(!hasCollided)
	{
		BOOST_FOREACH(Section_ptr section, sub_sections_)
		{
			hasCollided |= section->CheckCollisions(_projectile);
			if(hasCollided)
				break;
		}
	}
	return hasCollided;
}

void Section::RayCollisionFilter(Vector3f P1, Vector3f P2, std::list<Section*>& _valid_sections, float& _min_distance, float& _max_distance)
{
	bool hasCollided = false;
	
	if(Collisions2f::LineInCircle(P1, P2, ltv_position_, radius_))
	{
		_valid_sections.push_back(this);
		float distance = Collisions2f::Distance(ltv_position_, P1);
		if(distance - radius_ < _min_distance)
			_min_distance = distance - radius_;
		if(distance + radius_ > _max_distance)
			_max_distance = distance + radius_;

	}
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->RayCollisionFilter(P1, P2, _valid_sections, _min_distance, _max_distance);
	}
}

bool Section::CheckCollisions(Vector3f _location, Section*& _section)
{
	bool hasCollided = false;
	
	if(Collisions2f::PointInCircle(_location, ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		for(int vert = 0; vert < fill_verts_->size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(ltv_transform_ * (*fill_verts_)[vert], ltv_transform_ * (*fill_verts_)[vert+1], ltv_transform_ * (*fill_verts_)[vert+2], _location))
			{
				hasCollided = true; //This is where we decided that the collision has taken place
				_section = this;
				break;
			}
		}
	}
	if(!hasCollided)
	{
		BOOST_FOREACH(Section_ptr section, sub_sections_)
		{
			hasCollided |= section->CheckCollisions(_location, _section);
			if(hasCollided)
				break;
		}
	}
	return hasCollided;
}

bool Section::IsRemovable(Section_ptr section)
{
	bool dead = (section->health_ <= 0);
	if(dead)
		Camera::Instance().Shake();
	return dead;
}
void Section::SetColor(GLColor _color)
{
	this->fill_color_= _color;
	for(int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->SetColor(_color);
	}
}


void Section::ScaleHealth(float _factor)
{
	SetMaxHealth(max_health_ * _factor);
	BOOST_FOREACH(Section_ptr sub_section, sub_sections_)
	{
		sub_section->ScaleHealth(_factor);
	}
}