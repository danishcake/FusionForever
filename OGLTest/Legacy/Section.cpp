#include "StdAfx.h"
#include "Section.h"

Section::Section(void)
{
	fill_color_ = GLColor(255,50,50);
	outline_color_ = GLColor(255,255,255);
	Position = Vector3f(0,0,0);
	Angle = 0;
	Radius = 0;
	Health = 500;
}

Section::~Section(void)
{
	for(int i = 0; i < sub_sections_.size(); i++)
	{
		delete sub_sections_[i];
		sub_sections_[i] = NULL;
	}
	sub_sections_.clear();
}


void Section::findRadius()
{
	for(std::vector<Vector3f>::iterator it = outline_verts_.begin(); it!=outline_verts_.end(); it++)
	{
		if(it->length() > Radius)
			Radius = it->length();
	}
}
void Section::DrawSelf()
{
	//Draw self
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);

	glColor3ub(fill_color_.r,fill_color_.g, fill_color_.b);
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < fill_verts_.size(); i++)
	{
		glVertex3fv(fill_verts_[i]);
	}
	glEnd();

	glColor3ub(outline_color_.r, outline_color_.g, outline_color_.b);
	glBegin(GL_LINE_LOOP);
	for(unsigned int i = 0; i < outline_verts_.size(); i++)
	{
		glVertex3fv(outline_verts_[i]);
	}
	glEnd();

	//Draw children
	for(int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->DrawSelf();
	}

	glPopMatrix();
}

void Section::AddChild(Section* _new_section)
{
	this->sub_sections_.push_back(_new_section);
	_new_section->SetColor(fill_color_);
}

void Section::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, Matrix4f _transform)
{
	Matrix4f translation = Matrix4f::createTranslation(Position.x, Position.y, 0.0f);
	Matrix4f rotation = Matrix4f::createRotationAroundAxis(0,0,-Angle * DEGTORAD);
	_transform =   _transform * translation * rotation ;

	ltv_transform_ = _transform;
	ltv_position_ = _transform.getTranslation();
	
	sub_sections_.erase(std::remove_if(sub_sections_.begin(), sub_sections_.end(),Section::IsSectionDead), sub_sections_.end());

	for(int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
	}
}
bool Section::CheckCollisions(boost::shared_ptr<Projectile> prj)
{
	bool hasCollided = false;
	if(Collisions2f::CirclesIntersect(prj->Position, prj->Radius, ltv_position_, Radius))
	{//Bounding circle test passed, do proper test
		for(int vert = 0; vert < fill_verts_.size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(ltv_transform_ * fill_verts_[vert], ltv_transform_ * fill_verts_[vert+1], ltv_transform_ * fill_verts_[vert+2], prj->Position))
			{
				hasCollided = true; //This is where we decided that the collision has taken place
				Health -= prj->Damage;
				prj->LifeTime = 0;
				break;
			}
		}
	}
	if(!hasCollided)
	{
		for(std::vector<Section*>::iterator it = sub_sections_.begin(); it < sub_sections_.end(); it++)
		{
			hasCollided |= (*it)->CheckCollisions(prj);
			if(hasCollided)
				break;
		}
	}
	return hasCollided;
}
bool Section::IsSectionDead(Section* section)
{
	if(section->Health <= 0)
		return true;
	return false;
}
void Section::fireProjectile(Projectile* _projectile, std::list<boost::shared_ptr<Projectile>>& _spawn)
{
	_projectile->Position = ltv_transform_ * _projectile->Position;
	_projectile->SetVelocity(ltv_transform_ * _projectile->Velocity - ltv_position_);
	_spawn.push_back(boost::shared_ptr<Projectile>(_projectile));
}

void Section::SetColor(GLColor _fill_color)
{
	fill_color_ = _fill_color;
}

void Section::SetTarget(Vector3f _target_position)
{
	target_position_ = _target_position;
}