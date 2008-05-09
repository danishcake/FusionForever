#include "StdAfx.h"
#include "LineTrace.h"

LineTrace::LineTrace(BaseEntity* _source)
{
	source_ = _source;
	assert(source_ != NULL);
	source_->AddSubscriber(this);
	lifetime_ = 20;
	front_index_ = 0;
	first_run_ = true;
}

LineTrace::~LineTrace(void)
{
	if(source_!=NULL)
		source_->RemoveSubscriber(this);
}

void LineTrace::Tick(float _timespan, Matrix4f _transform)
{
	if(source_!=NULL)
	{
		position_ = source_->GetGlobalPosition();
		lifetime_ = 1; // should live as long as source exists
	}
	else
	{
		lifetime_ = 0;
	}
	Decoration::Tick(_timespan, _transform);
	if(first_run_)
	{
		for(int i = 0; i < MAX_LINE_SECTIONS; i++)
			line_trace_[i] = ltv_position_;
		first_run_ = false;
	}
	front_index_++;
	front_index_ %= MAX_LINE_SECTIONS;
	line_trace_[front_index_] = ltv_position_;
}

void LineTrace::DrawSelf()
{
	glColor3b(200, 50, 50);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINE_STRIP);
	int points_left = MAX_LINE_SECTIONS;
	int index = front_index_;
	while(points_left > 0)
	{
		index++;
		index %= MAX_LINE_SECTIONS;
		glVertex3fv(line_trace_[index]);
		points_left--;
	}
	glEnd();
	glPopMatrix();
}

void LineTrace::initialise_fill()
{
}

void LineTrace::EndSubscription(BaseEntity* _source)
{
	if(source_ == _source)
		source_ = NULL;
}