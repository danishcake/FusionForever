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
  fill_color_ = GLColor(200,50,0);
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
		//lifetime_ = 0;
	}
	Decoration::Tick(_timespan, _transform);
	if(first_run_)
	{
		for(int i = 0; i < MAX_LINE_SECTIONS; i++)
			line_trace_[i] = ltv_position_;
		first_run_ = false;
	}
  while(_timespan >= 0)
  {
	  front_index_++;
	  front_index_ %= MAX_LINE_SECTIONS;
	  line_trace_[front_index_] = ltv_position_;
    _timespan -= LINE_SECTION_TIME;
  }

  line_trace_[MAX_LINE_SECTIONS] = line_trace_[0];
}

void LineTrace::DrawSelf()
{
  glColor4ub(fill_color_.r,fill_color_.g, fill_color_.b, (GLubyte)(255.0f*lifetime_));
	glPushMatrix();
	glLoadIdentity();

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, line_trace_);
	
	//glDrawArrays(GL_LINE_STRIP, 0, MAX_LINE_SECTIONS);
	glDrawArrays(GL_LINE_STRIP, 0, front_index_);
	glDrawArrays(GL_LINE_STRIP, front_index_+1, MAX_LINE_SECTIONS - front_index_);
	
	/*
	//glBegin(GL_LINE_STRIP);
	//int points_left = MAX_LINE_SECTIONS;
	//int index = front_index_;
	while(points_left > 0)
	{
		index++;
		index %= MAX_LINE_SECTIONS;
		glVertex3fv(line_trace_[index]);
		points_left--;
	}
	glEnd();
	*/
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
