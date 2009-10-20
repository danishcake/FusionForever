#include "StdAfx.h"
#include "LabelDecoration.h"
#include "Camera.h"

LabelDecoration::LabelDecoration(BaseEntity* _source, Vector3f _screen_position, float _lifetime)
{
	source_ = _source;
	assert(source_ != NULL);
	source_->AddSubscriber(this);
	source_location_ = source_->GetGlobalPosition();
	screen_position_ = _screen_position;
	lifetime_ = _lifetime;
	full_lifetime_ = _lifetime;
	fill_.SetFillColor(GLColor(255, 255, 255));
}

LabelDecoration::~LabelDecoration(void)
{
	if(source_!=NULL)
		source_->RemoveSubscriber(this);
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
	if(full_lifetime_ - lifetime_ < 0.1f)
	{
		fill_.SetFillColor(GLColor(255, 255, 255, (full_lifetime_ - lifetime_) / 0.1f));
	}else if(lifetime_ < 1.0f && lifetime_ > 0)
	{
		fill_.SetFillColor(GLColor(255, 255, 255, lifetime_));
	} else if(lifetime_ < 0)
	{
		fill_.SetFillColor(GLColor(255, 255, 255, 0.0f));
	} else
	{
		fill_.SetFillColor(GLColor(255, 255, 255, 1.0f));
	}
	SetPosition(Camera::Instance().ScreenToWorld(screen_position_));
	Decoration::Tick(_timespan, _transform, _decoration_spawn);
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
}

void LabelDecoration::EndSubscription(Subscriber* _source)
{
	if(source_ == _source)
		source_ = NULL;
}
