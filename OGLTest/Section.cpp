#include "StdAfx.h"
#include "Section.h"

Section::Section(void) 
: BaseEntity(), Outlined(), Filled()
{
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
	for(int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->DrawSelf();
	}

	glPopMatrix();
}
void Section::AddChild(Section *child)
{
	this->sub_sections_.push_back(child);
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

void Section::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{

	BaseEntity::Tick(_timespan, _transform); // Use ltv_transform after this as _transform is passed by value
	
	sub_sections_.erase(std::remove_if(sub_sections_.begin(), sub_sections_.end(), 
		                Section::IsSectionDead), sub_sections_.end());
	for(int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->SetFiring(firing_);
		sub_sections_[i]->Tick(_timespan, _spawn_prj, _spawn_dec, ltv_transform_);
	}
}
bool Section::CheckCollisions(boost::shared_ptr<Projectile> _projectile)
{
	bool hasCollided = false;
	if(Collisions2f::CirclesIntersect(_projectile->GetPosition(), _projectile->GetRadius(), ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		for(int vert = 0; vert < fill_verts_->size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(ltv_transform_ * (*fill_verts_)[vert], ltv_transform_ * (*fill_verts_)[vert+1], ltv_transform_ * (*fill_verts_)[vert+2], _projectile->GetPosition()))
			{
				hasCollided = true; //This is where we decided that the collision has taken place				
				health_ -= _projectile->GetDamage();
				_projectile->SetLifetime(0);
				break;
			}
		}
	}
	if(!hasCollided)
	{
		for(std::vector<Section*>::iterator it = sub_sections_.begin(); it < sub_sections_.end(); it++)
		{
			hasCollided |= (*it)->CheckCollisions(_projectile);
			if(hasCollided)
				break;
		}
	}
	return hasCollided;
}

bool Section::IsSectionDead(Section* section)
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

