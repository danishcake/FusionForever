#include "StdAfx.h"
#include "Section.h"
#include "Puff.h"
#include "Core.h"
#include "ICollisionManager.h"
#include "Property.h"
#include "BillboardDeco.h"
#include "ThrusterTrail.h"

int Section::section_count_ = 0;
int Section::section_freed_ = 0;

bool zero_or_less(float _value)
{
	return (_value <= 0);
}

Section::Section(void)
: BaseEntity()
{
	health_ = FlexFloat(800, 800);
	shield_ = FlexFloat(0, 0);
	has_shield_ = false;
	shield_shock_time_ = FlexFloat(3, 3);
	shield_down_time_ = FlexFloat(10, 10);
	shield_recharge_rate_ = FlexFloat(1, 1);
	shield_recharge_cost_ = FlexFloat(1, 1);
	shield_radius_ = FlexFloat(0, 0);
	energy_ = FlexFloat(10,10);
	power_generation_= FlexFloat(0);
	thrust_ = FlexFloat(0);
	thrust_color_ = GLColor(255, 255, 255);
	thrust_scale_ = -1;
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
	transformed_outline_verts_valid_ = false;
	first_tick_ = true;
	mass_ = 100;
	moment_ = 100;
	time_since_damage_ = 1000; //Not yet taken damage, arbitary large #
}

Section::~Section(void)
{
	section_freed_++;
}

void Section::DrawSelf(void)
{
	//Draw self
	glPushMatrix();
	glLoadMatrixf(ltv_transform_);
	fill_.DrawFillDisplayList();
	outline_.DrawOutlinedDisplayList();
	if(has_shield_ && shield_ > 0 && health_ > 0)
	{
		shield_outline_.DrawOutlinedDisplayList();
	}
	//Draw children
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->DrawSelf();
	}

	glPopMatrix();
}
void Section::DrawEditorSupport(float _grid_size, Section_ptr _selected)
{
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glPushMatrix();
	glLoadIdentity();
	//Draw a line from current position to each child
	glBegin(GL_LINES);
	Vector3f start = GetGlobalPosition();
	BOOST_FOREACH(Section_ptr child, sub_sections_)
	{
		Vector3f end = child->GetGlobalPosition();
		glVertex3f(start.x, start.y, 0);
		glVertex3f(end.x, end.y, 0);
	}

	if(_selected == this)
	{
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	}

	glVertex3f(start.x -_grid_size, start.y -_grid_size, 0);
	glVertex3f(start.x +_grid_size, start.y -_grid_size, 0);

	glVertex3f(start.x -_grid_size, start.y +_grid_size, 0);
	glVertex3f(start.x +_grid_size, start.y +_grid_size, 0);

	glVertex3f(start.x -_grid_size, start.y -_grid_size, 0);
	glVertex3f(start.x -_grid_size, start.y +_grid_size, 0);

	glVertex3f(start.x +_grid_size, start.y -_grid_size, 0);
	glVertex3f(start.x +_grid_size, start.y +_grid_size, 0);

	glEnd();


	glLineWidth(2);
	glPushMatrix();
	glTranslatef(start.x, start.y, 0);
	glRotatef(-GetGlobalAngle(), 0, 0, 1);
	
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	
	glVertex3f(0, 0, 0);
	glVertex3f(_grid_size * 4, 0, 0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	
	glVertex3f(0, 0, 0);
	glVertex3f(0, _grid_size * 4, 0);
	
	glEnd();
	glPopMatrix();
	glLineWidth(1);

	BOOST_FOREACH(Section_ptr child, sub_sections_)
	{
		child->DrawEditorSupport(_grid_size, _selected);
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
	sub_sections_.push_back(child);
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
	_spawn_dec.push_back(new SectionDecoration(this));
	
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->TakeDamage(section->GetHealth(), -1);;
		section->GetDeathSpawn(_spawn_dec);
	}
	BOOST_FOREACH(Vector3f vert, *outline_.GetOutlineVerts())
	{
		Puff* puff = new Puff();
		puff->SetPosition(ltv_transform_ * vert);
		_spawn_dec.push_back(Decoration_ptr(puff));
	}
	sub_sections_.clear();
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
	StopSubscribing();
}

void Section::DeathTick()
{
	BaseEntity::Tick(0, Matrix4f());
}

void Section::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	transformed_fill_verts_valid_ = false;
	transformed_outline_verts_valid_ = false;
	transformed_fill_verts_.clear();
	transformed_outline_verts_.clear();
	BaseEntity::Tick(_timespan, _transform); // Use ltv_transform after this as _transform is passed by value

	GetRoot()->energy_ += (power_generation_ * _timespan);

	if(first_tick_)
	{
		if(root_)
		{
			root_->AddMass(mass_);
			moment_ = mass_ * (GetGlobalPosition() - root_->GetGlobalPosition()).length();
			root_->AddMoment(moment_);
			root_->AddEnergyCap(energy_.GetMaxValue());
			root_->AddThrust(thrust_);
			root_->AddTotalHealth(health_.GetMaxValue());
		} else if(GetRoot() == this)
		{
			/* If the root has it's health scaled it will not be counted. On first tick we correct 
			   this by setting to zero then adding on the true value.								*/
			GetRoot()->AddTotalHealth(-GetRoot()->GetTotalHealth());
			GetRoot()->AddTotalHealth(health_.GetMaxValue());
		}
		BillboardDeco* warp = new BillboardDeco("Warp", 0.5, BillboardDecoType::Warp);
		warp->SetPosition(this->GetGlobalPosition());
		_spawn_dec.push_back(warp);
		if(thrust_ > 0 && thrust_scale_ > 0)
		{
			ThrusterTrail* trail = new ThrusterTrail(this, thrust_scale_, thrust_color_);
			_spawn_dec.push_back(trail);
		}


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
	time_since_damage_ += _timespan;
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

	if(has_shield_)
	{
		if(shield_.GetValue() > 0 && time_since_damage_ > shield_shock_time_.GetValue() &&
			PowerRequirement((shield_recharge_cost_ * shield_recharge_rate_ * _timespan * 5).GetValue()))
		{
			shield_ += shield_recharge_rate_ * _timespan;
			PowerTick(-(shield_recharge_cost_ * shield_recharge_rate_ * _timespan).GetValue());
		}
		if(shield_.GetValue() == 0 && time_since_damage_ > shield_down_time_.GetValue())
		{
			shield_ = shield_.GetMaxValue() * 0.2f;
		}
	}
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
				TakeDamage(_projectile->GetDamage(), GetRoot()->GetSectionID());
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
	float effective_radius;
	if(has_shield_ && shield_ > 0)
		effective_radius = shield_radius_.GetValue();
	else
		effective_radius = radius_;

	if(Collisions2f::LineInCircle(P1, P2, ltv_position_, effective_radius))
	{
		_valid_sections.push_back(this);
		float distance = Collisions2f::Distance(ltv_position_, P1);
		if(distance - effective_radius < _min_distance)
			_min_distance = distance - effective_radius;
		if(distance + effective_radius > _max_distance)
			_max_distance = distance + effective_radius;

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

void Section::CheckCollisions(Vector3f _location, std::vector<Section*>& _sections)
{
	if(Collisions2f::PointInCircle(_location, ltv_position_, radius_))
	{//Bounding circle test passed, do proper test
		if(!transformed_fill_verts_valid_)
		{
			for(unsigned int vert = 0; vert < fill_.GetFillVerts()->size(); vert++)
			{
				transformed_fill_verts_.push_back(ltv_transform_ * (*fill_.GetFillVerts())[vert]);
			}
			transformed_fill_verts_valid_ = true;
		}
		for(unsigned int vert = 0; vert < transformed_fill_verts_.size(); vert+=3)
		{
			if(Collisions2f::PointInTriangle(transformed_fill_verts_[vert], transformed_fill_verts_[vert + 1], transformed_fill_verts_[vert+2], _location))
			{
				_sections.push_back(this);
				break;
			}
		}
	}
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		section->CheckCollisions(_location, _sections);
	}
}

bool Section::CheckCollisions(const Vector3f _lineP1, const Vector3f _lineP2, Vector3f& _collision_point)
{
	if(!transformed_outline_verts_valid_)
	{
		for(unsigned int vert = 0; vert < outline_.GetOutlineVerts()->size(); vert++)
		{
			transformed_outline_verts_.push_back(ltv_transform_ * (*outline_.GetOutlineVerts())[vert]);
		}
		transformed_outline_verts_valid_ = true;
	}

	if(has_shield_ && shield_ > 0)
	{
		Vector3f collision_point;
		if(Collisions2f::LineInCircle(_lineP1, _lineP2, ltv_position_, shield_radius_.GetValue(), collision_point))
		{
			_collision_point = collision_point;
			return true;
		}
	} else
	{
		Vector3f collision_point;
		Vector3f* first_point = &transformed_outline_verts_[0];
		int num_points = static_cast<int>(outline_.GetOutlineVerts()->size());

		if(Collisions2f::LineInPolygon(_lineP1, _lineP2, first_point, num_points, collision_point))
		{
			_collision_point = collision_point;
			return true;
		}
	}
	return false;
}

void Section::SetColor(GLColor _color)
{
	fill_.SetFillColor(_color);
	for(unsigned int i = 0; i < sub_sections_.size(); i++)
	{
		sub_sections_[i]->SetColor(_color);
	}
}

void Section::SetOutlineColor(GLColor _color)
{
	outline_.SetOutlineColor(_color);
}

GLColor Section::GetColor()
{
	return fill_.GetFillColor();
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

/* Decrements health, starts section flashing and reports the damage to the core */
void Section::TakeDamage(float _damage, int /*_section_id*/)
{
	float threat = 0;
	if(has_shield_ && shield_ > 0)
	{
		threat = _damage < shield_.GetValue() ? _damage : shield_.GetValue();
		shield_ -= _damage;
		GetRoot()->ReportDamage(0, threat);		
	} else
	{
		damage_flash_timer_ = SECTION_FLASH_TIME;
		threat = _damage < health_.GetValue() ? _damage : health_.GetValue();
		health_ -= _damage;
		GetRoot()->ReportDamage(threat, 0);
	}
	
	time_since_damage_ = 0;
}

/* Adjusts the Cores maximum health, changed max health of section */
void Section::SetMaxHealth(float _max_health)
{
	if(!first_tick_)
	{
		GetRoot()->AddTotalHealth(-health_.GetMaxValue());
		GetRoot()->AddTotalHealth(_max_health);
	}
	health_.SetMaxValue(_max_health);
	health_ = _max_health;
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
	else
		energy_ += _power_delta;
}

Core_ptr Section::GetRoot()
{
	return root_ != NULL ? root_ : static_cast<Core_ptr>(this);
}

Section* Section::GetSectionByID(int _section_id)
{
	if(section_id_ == _section_id)
		return this;
	BOOST_FOREACH(Section_ptr section, sub_sections_)
	{
		Section* ss = section->GetSectionByID(_section_id);
		if(ss)
			return ss;
	}
	return NULL;
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
	_node->SetAttribute("SectionType", section_type_);
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

/* Static functions to wrap member functions for callbacks */

static float sGetHealth(Section* _section)
{
	return _section->GetHealth();
}
static void sSetHealth(Section* _section, float _value)
{
	_section->SetMaxHealth(_value);
}
static float sGetFiringDelay(Section* _section)
{
	return _section->GetFiringDelay();
}
static void sSetFiringDelay(Section* _section, float _value)
{
	_section->SetFiringDelay(_value);
}

void Section::GetProperties(std::vector<Property*>& _properties )
{
	_properties.push_back(new Property(this, sSetHealth, sGetHealth, "Health"));
	_properties.push_back(new Property(this, sSetFiringDelay, sGetFiringDelay, "FiringDelay"));
}

bool Section::ParseSpecific(TiXmlElement* /*_node*/)
{
	return true;
}

void Section::CollectShields(std::vector<Section_ptr>& _shields)
{
	if(has_shield_)
		_shields.push_back(this);
	BOOST_FOREACH(Section_ptr child, sub_sections_)
	{
		child->CollectShields(_shields);
	}
}

void Section::RegisterMetadata()
{
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Health", health_.GetMaxValue());
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Thrust", thrust_.GetMaxValue());
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Energy", energy_.GetMaxValue());
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Power", power_generation_.GetMaxValue());
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Radius", radius_);
	SectionMetadata::RegisterSectionKeyValue(section_type_, "IsShield", has_shield_);
}