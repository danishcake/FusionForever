#include "StdAfx.h"
#include "ThrusterTrail.h"
#include "TurningRoutines.h"

ThrusterTrail::ThrusterTrail(BaseEntity* _source, float _length_scale, GLColor trail_color_)
{
	source_ = _source;
	assert(source_ != NULL);
	source_->AddSubscriber(this);
	lifetime_ = 20;
	first_run_ = true;
	fill_.SetFillColor(trail_color_);
	ltv_source_position_ = source_->GetGlobalPosition();
	section_length_ = 4.0f;
	length_scale_ = _length_scale;
	for(int i = 0; i < 10; i++)
	{
		points_.push_back(0);
		cartesian_points_.push_back(Vector3f(0,0,0));
	}
}

ThrusterTrail::~ThrusterTrail(void)
{
	if(source_!=NULL)
		source_->RemoveSubscriber(this);
}

void ThrusterTrail::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	if(source_!=NULL)
	{
		Vector3f np = source_->GetGlobalPosition();
		Vector3f dp = np - ltv_source_position_;
		float speed  = dp.length() / _timespan;
		float target_length = speed * 0.02f * length_scale_;
		float new_weight = expf(-_timespan * 0.1f);
		float rotation_rate = (400 + speed) * 10;
		if(section_length_ > 0.1)
			rotation_rate /= section_length_;

		section_length_ = (1.0f - new_weight) * section_length_ + new_weight * target_length;

		points_[0] = source_->GetGlobalAngle();
		cartesian_points_[0] = np;
		for(int i = 1; i < points_.size(); i++)
		{
			TurnData td = GetTurnDirection(points_[i], points_[i-1]);
			points_[i] = points_[i] + td.turn_factor * _timespan * rotation_rate;
			cartesian_points_[i] = cartesian_points_[i-1] + Vector3f(-sin(points_[i] * M_PI / 180.0f), -cos(points_[i] * M_PI / 180.0f), 0) * section_length_ ;
		}

		lifetime_ = 1; // should live as long as source exists
		ltv_source_position_ = source_->GetGlobalPosition();
	}
	Decoration::Tick(_timespan, _transform, _decoration_spawn);
}

void ThrusterTrail::DrawSelf()
{
	glColor4ub(fill_.GetFillColor().r,fill_.GetFillColor().g, fill_.GetFillColor().b, (unsigned char)(255.0f*lifetime_));
	glPushMatrix();
	glLoadIdentity();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &cartesian_points_[0]);
	glDrawArrays(GL_LINE_STRIP, 0, cartesian_points_.size());

	glPopMatrix();
}

void ThrusterTrail::EndSubscription(Subscriber* _source)
{
	if(source_ == _source)
		source_ = NULL;
}
