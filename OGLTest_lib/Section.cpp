#include "StdAfx.h"
#include "Section.h"
#include "Puff.h"
#include "Core.h"
#include "ICollisionManager.h"


int Section::section_count_ = 0;

bool zero_or_less(float _value)
{
	return (_value <= 0);
}

Section::Section(void)
: BaseEntity()
{
	health_ = FlexFloat(800, 800);
	energy_ = FlexFloat(10,10);
	power_generation_= FlexFloat(0);
	thrust_ = FlexFloat(0);
	damage_flash_timer_ = 0;
	flash_timer_ = 0;
	flash_state_ = false;
	firing_delay_ = 0;
	ltv_firing_ = false;
	firing_ = false;
	default_sub_section_position_ = Vector3f(0,0,0);
	section_id_ = section_count_++;
	parent_ = NULL;
	root_ = NULL;
	outline_color_base_ = GLColor(255, 255, 255);
	transformed_fill_verts_valid_ = false;
	first_tick_ = true;
	mass_ = 100;
	moment_ = 1;
}

Section::~Section(void)
{
	if(root_)
	{
		root_->AddMass(-mass_);
		root_->AddMoment(-moment_);
		root_->AddEnergyCap(-energy_.GetMaxValue());
		root_->AddThrust(-thrust_);

		Vector3f rel_position = GetGlobalPosition() - root_->GetGlobalPosition();
		Vector3f rel_position_norm = rel_position;
		if(rel_position_norm.length() > 0)
			rel_position_norm.normalize();
		root_->ImpartMomentum(rel_position_norm * mass_ * 10, rel_position);
	}
}

void Section::DrawSelf(void)
{
	//Draw self
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	fill_.DrawFillDisplayList();
	outline_.DrawOutlinedDisplayList();
	//Draw children
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->DrawSelf();
	}

	glPopMatrix();
}
void Section::AddChild(Section *child)
{
	assert(child!=NULL);
	if(!child->GetPositionSpecified())
	{
		child->SetPosition(default_sub_section_position_);
	}
	this->sub_sections_.push_back(child);
	child->SetColor(fill_.GetFillColor());
	child->SetParentAndRoot(this, root_ == NULL ? static_cast<Core_ptr>(this) : root_);
}

void Section::findRadius(void)
{
	for(std::vector<Vector3f>::iterator it = outline_.GetOutlineVerts()->begin(); it != outline_.GetOutlineVerts()->end(); it++)
	{
		if(it->length() > radius_)
			radius_ = it->length();
	}
}

void Section::GetDeathSpawn(std::vector<Decoration_ptr>& _spawn_dec)
{
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->GetDeathSpawn(_spawn_dec);
	}
	BOOST_FOREACH(Vector3f vert, *outline_.GetOutlineVerts())
	{
		Puff* puff = new Puff();
		puff->SetPosition(ltv_transform_ * vert);
		_spawn_dec.push_back(Decoration_ptr(puff));
	}
}

void Section::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	transformed_fill_verts_valid_ = false;
	transformed_fill_verts_.clear();
	BaseEntity::Tick(_timespan, _transform); // Use ltv_transform after this as _transform is passed by value
	if(root_)
	{
		root_->energy_ += (power_generation_ * _timespan);
	}
	if(first_tick_ && root_)
	{
		root_->AddMass(mass_);
		moment_ = mass_ * (GetGlobalPosition() - root_->GetGlobalPosition()).length();
		root_->AddMoment(moment_);
		root_->AddEnergyCap(energy_.GetMaxValue());
		root_->AddThrust(thrust_);
		first_tick_ = false;
	}
	_collision_manager->Register(this);
	BOOST_FOREACH(float& delay, firing_edges_)
	{
		delay -= _timespan;
		if(delay <= 0)
		{
 			firing_ = !firing_;
		}
	}
	firing_edges_.erase(std::remove_if(firing_edges_.begin(), firing_edges_.end(),
		                zero_or_less), firing_edges_.end());

	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		if(section->GetHealth() <=0)
		{
			section->GetDeathSpawn(_spawn_dec);
		}
		else
		{
			section->SetFiring(firing_);
			section->Tick(_timespan, _spawn_prj, _spawn_dec, ltv_transform_, _enemies, _collision_manager);
		}
	}

	sub_sections_.erase(std::remove_if(sub_sections_.begin(), sub_sections_.end(),
		                Section::IsRemovable), sub_sections_.end());
	damage_flash_timer_ -= _timespan;
	flash_timer_ += _timespan;
	if(damage_flash_timer_ > 0)
	{
		if(flash_timer_ > health_.GetValue() / health_.GetMaxValue() / 2.0f)
		{
			flash_timer_ = 0;
			flash_state_ = !flash_state_;
		}
	}	else
	{
		flash_state_ = 0;
	}
	if(flash_state_)
		outline_.SetOutlineColor(outline_color_base_.GetFaded(0));
	else
		outline_.SetOutlineColor(outline_color_base_);
}

bool Section::CheckCollisions(Projectile_ptr _projectile)
{
	bool hasCollided = false;
	if(Collisions2f::CirclesIntersect(_projectile->GetPosition(), _projectile->GetRadius(), ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		if(!transformed_fill_verts_valid_)
		{
			for(unsigned int vert = 0; vert < fill_.GetFillVerts()->size(); vert++)
			{
				transformed_fill_verts_.push_back(ltv_transform_ * (*fill_.GetFillVerts())[vert]);
			}
		}
		for(unsigned int vert = 0; vert < transformed_fill_verts_.size(); vert+=3)
		{
			if(Collisions2f::LineTriangleIntersect(transformed_fill_verts_[vert],
												   transformed_fill_verts_[vert+1],
												   transformed_fill_verts_[vert+2],
												   _projectile->GetPosition(),
												   _projectile->GetPreviousPosition()))
			{
				hasCollided = true; //This is where we decided that the collision has taken place
				TakeDamage(_projectile->GetDamage());
				_projectile->SetLifetime(0);
				if(root_)
					root_->ImpartMomentum(_projectile->GetVelocity() * _projectile->GetMass(), _projectile->GetGlobalPosition());
				else
					ImpartMomentum(_projectile->GetVelocity() * _projectile->GetMass(), _projectile->GetGlobalPosition());
				break;
			}
		}
	}
	return hasCollided;
}

void Section::RayCollisionFilter(Vector3f P1, Vector3f P2, std::vector<Section_ptr>& _valid_sections, float& _min_distance, float& _max_distance)
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

bool Section::CheckCollisions(Vector3f _location, Section_ptr& _section)
{
	bool hasCollided = false;

	if(Collisions2f::PointInCircle(_location, ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		if(!transformed_fill_verts_valid_)
		{
			for(unsigned int vert = 0; vert < fill_.GetFillVerts()->size(); vert++)
			{
				transformed_fill_verts_.push_back(ltv_transform_ * (*fill_.GetFillVerts())[vert]);
			}
		}
		for(unsigned int vert = 0; vert < transformed_fill_verts_.size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(transformed_fill_verts_[vert], transformed_fill_verts_[vert + 1], transformed_fill_verts_[vert+2], _location))
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

void Section::SetColor(GLColor _color)
{
	fill_.SetFillColor(_color);
	for(unsigned int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->SetColor(_color);
	}
}


void Section::ScaleHealth(float _factor)
{
	SetMaxHealth(health_.GetMaxValue() * _factor);
	health_.SetMaxValue(health_.GetMaxValue() * _factor);
	health_ = health_.GetMaxValue();
	BOOST_FOREACH(Section_ptr sub_section, sub_sections_)
	{
		sub_section->ScaleHealth(_factor);
	}
}

void Section::SetFiring(bool _firing)
{
	if(_firing != ltv_firing_)
	{
		firing_edges_.push_back(firing_delay_);
		ltv_firing_ = _firing;
	}
}

void Section::SetParentAndRoot(Section* _parent, Core_ptr _root)
{
	parent_ = _parent;
	root_ = _root;

	BOOST_FOREACH(Section_ptr sub_section, sub_sections_)
	{
		sub_section->SetParentAndRoot(this, _root);
	}
}

std::vector<Section_ptr> Section::DetachChildren()
{
	std::vector<Section_ptr> detached_children = sub_sections_;
	sub_sections_.clear();
	return detached_children;
}

void Section::AttachChildren(std::vector<Section_ptr> _children)
{
	BOOST_FOREACH(Section_ptr child, _children)
	{
		child->SetParentAndRoot(this, root_ == NULL ? static_cast<Core_ptr>(this) : root_);
	}
	sub_sections_.insert(sub_sections_.end(),_children.begin(), _children.end());
}

void Section::TakeDamage(float _damage)
{
	health_-=_damage; 
	damage_flash_timer_ = SECTION_FLASH_TIME;
}

bool Section::PowerRequirement(float _minimum_power)
{
	if(root_)
		return root_->PowerRequirement(_minimum_power);
	else
		return energy_ >= _minimum_power;
}

void Section::PowerTick(float _power_delta)
{
	if(root_)
		root_->PowerTick(_power_delta);
}

Core_ptr Section::GetRoot()
{
	return root_ != NULL ? root_ : static_cast<Core_ptr>(this);
}

void Section::ToXML(TiXmlElement* _node)
{
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		//Add new node
		TiXmlElement* child = new TiXmlElement("Section");
		section->ToXML(child);
		_node->LinkEndChild(child);
	}
	//To be overriden in all subclasses
	_node->SetAttribute("x", boost::lexical_cast<std::string, float>(position_.x));
	_node->SetAttribute("y", boost::lexical_cast<std::string, float>(position_.y));
	_node->SetAttribute("Angle", boost::lexical_cast<std::string, float>(angle_));
	_node->SetAttribute("Health", boost::lexical_cast<std::string, float>(health_.GetMaxValue()));
	_node->SetAttribute("Delay", boost::lexical_cast<std::string, float>(firing_delay_));
}

void Section::SaveToXML(std::string _filename)
{
	TiXmlDocument doc = TiXmlDocument();
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* root = new TiXmlElement("Section");
	ToXML(root);
	root->SetAttribute("x", boost::lexical_cast<std::string, float>(0));
	root->SetAttribute("y", boost::lexical_cast<std::string, float>(0));
	root->SetAttribute("angle", boost::lexical_cast<std::string, float>(0));
	doc.LinkEndChild(root);

	doc.SaveFile(_filename);
}