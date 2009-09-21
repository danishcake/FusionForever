#include "StdAfx.h"
#include "SectionDecoration.h"
#include "Section.h"
#include "Core.h"

void SectionDecoration::InitialiseGraphics()
{
	//Do nothing
}

SectionDecoration::SectionDecoration(Section* _section)
{
	//assert(_section->GetSubsections().size() == 0);
	//assert(_section->GetParent() == NULL);
	assert(_section);

	section_ = _section;
	position_ = _section->GetGlobalPosition();
	angle_ = _section->GetGlobalAngle();
	velocity_ = _section->GetRoot()->GetVelocity() + 
				Vector3f(Random::RandomRange(-10, 10), Random::RandomRange(-50, 50), 0);
	spin_ = Random::RandomRange(-360, 360);
	if(section_->GetMass() < 50)
		lifetime_ = 0.25f;
	else if(section_->GetMass() < 1000)
	{
		lifetime_ = Random::RandomRange(0.5, 0.5 + 4.5 * (section_->GetMass() / 1000.0f));
	} else
		lifetime_ = Random::RandomRange(0.5, 5);
		
	full_lifetime_ = lifetime_;
	original_color_ = section_->GetColor();
}

SectionDecoration::~SectionDecoration()
{
	delete section_;
}
void SectionDecoration::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	float speed = this->velocity_.length();
	speed = speed * expf(-_timespan * 1);
	velocity_.normalize();
	velocity_ *= speed;

	Decoration::Tick(_timespan, _transform, _decoration_spawn);
	section_->SetPosition(position_);
	section_->SetAngle(angle_);
	section_->DeathTick();
	GLColor faded = original_color_;
	faded.a = 255 * (lifetime_/ full_lifetime_);
	section_->SetColor(faded);
	section_->SetOutlineColor(GLColor(255,255,255, faded.a));
}

void SectionDecoration::DrawSelf()
{
	section_->DrawSelf();
}