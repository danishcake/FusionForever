#include "StdAfx.h"
#include "LabelDecoration.h"
#include "Camera.h"
#include "Section.h"
#include "SectionMetadata.h"
#include "Billboard.h"

LabelDecoration::LabelDecoration(BaseEntity* _source, Vector3f _screen_position, float _lifetime)
{
	source_ = _source;
	assert(source_ != NULL);
	source_->AddSubscriber(this);
	source_location_ = source_->GetGlobalPosition();
	lifetime_ = _lifetime;
	full_lifetime_ = _lifetime;
	fill_.SetFillColor(GLColor(255, 255, 255));

	Section_ptr section = (Section_ptr)_source;
	std::vector<std::string> tags = SectionMetadata::GetTags(section->GetSectionType());
	Vector3f screen_position = _screen_position;
	Vector3f icon_spacing(25, 0, 0);
	if(screen_position.x > Camera::Instance().GetWidth() * 0.1f)
		icon_spacing.x *= -1;
	bool interesting = false;

	for(std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it)
	{
		if(!it->compare("Weapon"))
		{
			Billboard* bb = new Billboard("WeaponIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(!it->compare("Beam"))
		{
			Billboard* bb = new Billboard("BeamIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(!it->compare("Shield"))
		{
			Billboard* bb = new Billboard("ShieldIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(!it->compare("Generator"))
		{
			Billboard* bb = new Billboard("GeneratorIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(!it->compare("EnergyStorage"))
		{
			Billboard* bb = new Billboard("EnergyStorageIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(!it->compare("Thruster"))
		{
			Billboard* bb = new Billboard("ThrusterIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
		if(section->IsCore())
		{
			Billboard* bb = new Billboard("CoreIcon", BillboardType::ScreenSpace);
			bb->SetPosition(screen_position);
			labels_.push_back(bb);
			screen_position += icon_spacing;
			interesting = true;
		}
	}
	if(!interesting)
		lifetime_ = -1;

	screen_position_ = screen_position;
}

LabelDecoration::~LabelDecoration(void)
{
	if(source_!=NULL)
		source_->RemoveSubscriber(this);
	for(std::vector<Billboard*>::iterator it = labels_.begin(); it != labels_.end(); ++it)
	{
		delete *it;
	}
}

void LabelDecoration::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	if(source_!=NULL)
	{
		source_location_ = source_->GetGlobalPosition();
	}
	else
	{
		if(lifetime_ > 1.0f)
			lifetime_ = 1.0f;
	}

	float alpha = 0;
	if(full_lifetime_ - lifetime_ < 0.1f)
	{
		alpha = (full_lifetime_ - lifetime_) / 0.1f;
	}else if(lifetime_ < 1.0f && lifetime_ > 0.0f)
	{
		alpha = lifetime_;
	} else if(lifetime_ < 0.0f)
	{
		alpha = 0.0f;
	} else
	{
		alpha = 1.0f;
	}
	fill_.SetFillColor(GLColor(255, 255, 255, alpha));

	SetPosition(Camera::Instance().ScreenToWorld(screen_position_));
	Decoration::Tick(_timespan, _transform, _decoration_spawn);

	for(std::vector<Billboard*>::iterator it = labels_.begin(); it != labels_.end(); ++it)
	{
		(*it)->SetColor(GLColor(255, 255, 255, alpha));
		//(*it)->SetPosition(position_);
		//(*it)->Tick(_timespan, _transform, _decoration_spawn);
	}
}

void LabelDecoration::DrawSelf()
{
	glColor4ub(fill_.GetFillColor().r,fill_.GetFillColor().g, fill_.GetFillColor().b, fill_.GetFillColor().a);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_LINES);
	glVertex3f(position_.x, position_.y, 0);
	glVertex3f(source_location_.x, source_location_.y, 0);
	glEnd();

	glPopMatrix();

	for(std::vector<Billboard*>::iterator it = labels_.begin(); it != labels_.end(); ++it)
	{
		(*it)->Draw();
	}
}

void LabelDecoration::EndSubscription(Subscriber* _source)
{
	if(source_ == _source)
		source_ = NULL;
}
